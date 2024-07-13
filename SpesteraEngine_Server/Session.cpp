#include "Session.h"
#include <iostream>

Session::Session(boost::asio::ip::tcp::socket socket, int id)
    : socket_(std::move(socket)), id_(id) {
}

void Session::start() {
    AssignId assign_id;
    assign_id.set_id(std::to_string(id_));

    Wrapper wrapper;
    wrapper.set_channel(Wrapper::TCP);
    wrapper.set_type(Wrapper::ASSIGN_ID);
    wrapper.set_player_id(playerId_);
    wrapper.set_payload(assign_id.SerializeAsString());

    compress_to_write(wrapper);

    do_read();
}

std::string Session::GenerateSessionID() {
    std::string letters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";
    std::string playerId;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, letters.size() - 1);

    for (int i = 0; i < 12; i++) {
        playerId.push_back(letters[dist(gen)]);
    }
    playerId_ = playerId;
    return playerId;
}

void Session::compress_to_write(const Wrapper& msg) {
    bool write_in_progress = !write_msgs_.empty();
    std::string compressed_msg;
    BinaryCompressor compressor;
    compressor.compress_string(msg.SerializeAsString(), compressed_msg);
    write_msgs_.push_back(compressed_msg);

    if (!write_in_progress) {
        do_write();
    }
}


void Session::handle_message(const Wrapper& wrapper) {

    switch (wrapper.type()) {
    case Wrapper::HEARTBEAT:
        std::cout << "Received Heartbeat" << std::endl;
        break;
    case Wrapper::REQUEST:
    {
        Request request;
        request.ParseFromString(wrapper.payload());
        std::cout << "Received Request: " << request.data() <<"from player: " << wrapper.player_id() << std::endl;
        // Handle request
        Response response;
        response.set_data("Response to " + request.data());

        Wrapper response_wrapper;
        response_wrapper.set_channel(Wrapper_Channel_TCP);
        response_wrapper.set_type(Wrapper::RESPONSE);
        response_wrapper.set_player_id(playerId_);
        response_wrapper.set_payload(response.SerializeAsString());
        compress_to_write(response_wrapper);
    }
    break;
    default:
        std::cout << "Unknown message type" << std::endl;
        break;
    }
}

void Session::do_read() {
    auto self(shared_from_this());
    socket_.async_read_some(boost::asio::buffer(data_, max_length),
        [this, self](boost::system::error_code ec, std::size_t length) {
            if (!ec) {
                BinaryCompressor decompressor;
                std::string decompressed_data;
                try {
                    decompressor.decompress_string(data_, length, decompressed_data);
                    Wrapper wrapper;
                    if (wrapper.ParseFromString(decompressed_data)) {
                        handle_message(wrapper);
                    }
                    do_read();
                }
                catch (const std::runtime_error& e) {
                    std::cerr << "Decompression error: " << e.what() << std::endl;
                }
            }
        });
}

void Session::do_write() {
    auto self(shared_from_this());
    boost::asio::async_write(socket_,
        boost::asio::buffer(write_msgs_.front()),
        [this, self](boost::system::error_code ec, std::size_t length) {
            if (!ec) {
                write_msgs_.pop_front();
                if (!write_msgs_.empty()) {
                    do_write();
                }
            }
            else {
                std::cerr << "Write error for session ID " << id_ << ": " << ec.message() << std::endl;
                socket_.close(); // Or any other appropriate error handling
            }
        });
}