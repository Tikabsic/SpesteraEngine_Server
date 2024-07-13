#include "UDP_Server.h"
#include "Session.h"

extern std::unordered_map<std::string, std::shared_ptr<Session>> session_map_;

UDP_Server::UDP_Server(boost::asio::io_context& io_context, const std::string& address, int port)
    : socket_(io_context, udp::endpoint(boost::asio::ip::make_address(address), port)) {
    do_receive();
}

void UDP_Server::do_receive() {
    socket_.async_receive_from(
        boost::asio::buffer(data_, max_length), sender_endpoint_,
        [this](boost::system::error_code ec, std::size_t bytes_recvd) {
            if (!ec) {
                BinaryCompressor decompressor;
                std::string decompressed_data;
                try {
                    decompressor.decompress_string(data_, bytes_recvd, decompressed_data);
                    Wrapper wrapper;
                    if (wrapper.ParseFromArray(decompressed_data.data(), decompressed_data.size())) {
                        auto it = session_map_.find(wrapper.player_id());
                        if (it != session_map_.end()) {
                            it->second->set_udp_endpoint(sender_endpoint_);
                            it->second->handle_message(wrapper);
                        }
                    }
                }
                catch (const std::runtime_error& e) {
                    std::cerr << "Decompression error: " << e.what() << std::endl;
                }
                do_receive();
            }
        });
}

void UDP_Server::send_to_player(const std::string& player_id, const Wrapper& msg) {
    auto it = session_map_.find(player_id);
    if (it != session_map_.end()) {
        std::string serialized_msg;
        msg.SerializeToString(&serialized_msg);
        std::string compressed_msg;
        BinaryCompressor compressor;
        compressor.compress_string(serialized_msg, compressed_msg);

        socket_.async_send_to(
            boost::asio::buffer(compressed_msg), it->second->get_udp_endpoint(),
            [](boost::system::error_code /*ec*/, std::size_t /*bytes_sent*/) {}
        );
    }
}

void UDP_Server::send_to_all_players(const Wrapper& msg) {
    std::string serialized_msg;
    msg.SerializeToString(&serialized_msg);
    std::string compressed_msg;
    BinaryCompressor compressor;
    compressor.compress_string(serialized_msg, compressed_msg);

    for (const auto& session_pair : session_map_) {
        auto udp_endpoint = session_pair.second->get_udp_endpoint();
        socket_.async_send_to(
            boost::asio::buffer(compressed_msg),
            udp_endpoint,
            [](boost::system::error_code /*ec*/, std::size_t /*bytes_sent*/) {
                // Handle send completion
            });
    }
}
