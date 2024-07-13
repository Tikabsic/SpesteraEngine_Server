#include "UdpServer.h"
#include "NetworkProtocol.pb.h"
#include "BinaryCompressor.h"
#include <iostream>

UdpServer::UdpServer(boost::asio::io_context& io_context, const std::string& address, int port)
    : socket_(io_context, udp::endpoint(boost::asio::ip::make_address(address), port)) {
    receive_data();
}

UdpServer::~UdpServer() {
    socket_.close();
}

std::unordered_map<std::string, udp::endpoint> UdpServer::get_endpoint_map() {
    return endpoint_map_;
}

char* UdpServer::get_buffer() {
    return receive_data_;
}

void UdpServer::receive_data() {
    socket_.async_receive_from(
        boost::asio::buffer(receive_data_, max_length), sender_endpoint_,
        [this](boost::system::error_code ec, std::size_t bytes_received) {
            if (!ec && bytes_received > 0) {
                try {
                    BinaryCompressor decompressor;
                    std::string decompressed_data;
                    decompressor.decompress_string(this->get_buffer(), bytes_received, decompressed_data);

                    // Parse Wrapper message
                    Wrapper wrapper;
                    if (!wrapper.ParseFromString(decompressed_data)) {
                        throw std::runtime_error("Failed to parse Wrapper message");
                    }

                    // Extract player ID from the Wrapper
                    std::string player_id = wrapper.player_id();

                    // Update endpoint map with player ID and sender endpoint
                    {
                        std::lock_guard<std::mutex> lock(endpoint_map_mutex_);
                        endpoint_map_[player_id] = sender_endpoint_;
                    }
                    this->response(sender_endpoint_);

                    // Continue to receive data
                    this->receive_data();
                }
                catch (const std::exception& e) {
                    std::cerr << "Error processing received data: " << e.what() << std::endl;
                    this->receive_data();
                }
            } else {
                std::cerr << "Error receiving data: " << ec.message() << std::endl;
                this->receive_data();
            }
        });
}

void UdpServer::response(udp::endpoint endpoint) {
    std::string message = "Hello from server";
    send_buffer_ = message; // Przypisz wiadomoœæ do bufora wysy³ania

    socket_.async_send_to(
        boost::asio::buffer(send_buffer_), endpoint,
        [this, endpoint](boost::system::error_code ec, std::size_t bytes_sent) {
            if (!ec) {
                std::cout << "Sent response to client, bytes_sent: " << bytes_sent << " to client : " << endpoint << std::endl;
            } else {
                std::cerr << "Error sending response: " << ec.message() << std::endl;
            }
        });
}

void UdpServer::send_data_to_all_players(const std::string& message) {
    std::lock_guard<std::mutex> lock(endpoint_map_mutex_);

    if (endpoint_map_.empty()) {
        std::cout << "No players online" << std::endl;
        return;
    }

    for (const auto& pair : endpoint_map_) {
        const std::string& player_id = pair.first;
        const udp::endpoint& endpoint = pair.second;

        send_buffer_ = message;

        socket_.async_send_to(
            boost::asio::buffer(send_buffer_), endpoint,
            [player_id](boost::system::error_code ec, std::size_t bytes_sent) {
                if (!ec) {
                   std::cout << "Sent message to player " << player_id << ", bytes sent: " << bytes_sent << std::endl;
                } else {
                    std::cerr << "Error sending message to player " << player_id << ": " << ec.message() << std::endl;
                }
            });
    }
}