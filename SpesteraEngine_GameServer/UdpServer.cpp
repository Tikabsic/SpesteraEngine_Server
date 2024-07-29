#include "UdpServer.h"
#include "BinaryCompressor.h"
#include "ConnectionsManager.h"
#include <iostream>

UdpServer::UdpServer(boost::asio::io_context& io_context, const std::string& address, int port, ConnectionsManager* connmanager)
    : socket_(io_context, udp::endpoint(boost::asio::ip::make_address(address), port)), conn_manager_(connmanager) {
    receive_data();
}

UdpServer::~UdpServer() {
    socket_.close();
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
                    Wrapper wrapper;
                    if (!wrapper.ParseFromArray(receive_data_, bytes_received)) {
                        throw std::runtime_error("Failed to parse Wrapper message");
                    }

                    if (wrapper.type() == Wrapper::REQUESTLOGIN) {
                        RequestLogin login_request;
                        if (login_request.ParseFromString(wrapper.payload())) {
                            u_short player_id = login_request.player_id();
                            {
                                std::lock_guard<std::mutex> lock(endpoint_map_mutex_);
                                conn_manager_->add_endpoint_to_map(player_id, sender_endpoint_);
                            }
                            this->response_to_login_request(sender_endpoint_);
                        }
                        else {
                            throw std::runtime_error("Failed to parse RequestLogin message");
                        }
                    }
                    else {
                        throw std::runtime_error("Unknown message type");
                    }

                    this->receive_data();
                }
                catch (const std::exception& e) {
                    std::cerr << "Error processing received data: " << e.what() << std::endl;
                    this->receive_data();
                }
            }
            else {
                std::cerr << "Error receiving data: " << ec.message() << std::endl;
                this->receive_data();
            }
        });
}

void UdpServer::response_to_login_request(udp::endpoint endpoint) {
    std::string message = "Hello from server";
    Response response;
    response.set_data(message);
    Wrapper wrapper;
    wrapper.set_type(Wrapper::RESPONSE);
    wrapper.set_payload(response.SerializeAsString());
    std::string serialized_msg = wrapper.SerializeAsString();
    std::string compressed_msg;
    BinaryCompressor compressor;
    compressor.compress_string(serialized_msg, compressed_msg);
    send_buffer_ = compressed_msg;
    socket_.async_send_to(
        boost::asio::buffer(send_buffer_), endpoint,
        [this, endpoint](boost::system::error_code ec, std::size_t bytes_sent) {
            if (!ec) {
            }
            else {
                std::cerr << "Error sending response: " << ec.message() << std::endl;
            }
        });
}

void UdpServer::send_data_to_all_players(const std::string& message) {
    std::lock_guard<std::mutex> lock(endpoint_map_mutex_);

    if (conn_manager_->connections_.empty()) {
        std::cout << "No players online" << std::endl;
        return;
    }

    for (const auto& connection : conn_manager_->connections_) {
        const short& player_id = connection.first;
        const udp::endpoint& endpoint = connection.second->udp_connection_;
        send_buffer_ = message;

        socket_.async_send_to(
            boost::asio::buffer(send_buffer_), endpoint,
            [player_id, message](boost::system::error_code ec, std::size_t bytes_sent) {
                if (!ec) {
                }
                else {
                    std::cerr << "Error sending message to player " << player_id << ": " << ec.message() << std::endl;
                }
            });
    }
}

void UdpServer::send_data_to_other_players(const std::string& message, u_short playerId)
{
    std::lock_guard<std::mutex> lock(endpoint_map_mutex_);

    if (conn_manager_->connections_.empty()) {
        std::cout << "No players online" << std::endl;
        return;
    }

    for (const auto& connection : conn_manager_->connections_) {
        const short& player_id = connection.first;
        const udp::endpoint& endpoint = connection.second->udp_connection_;

        if (player_id == playerId) {
            continue;
        }

        send_buffer_ = message;

        socket_.async_send_to(
            boost::asio::buffer(send_buffer_), endpoint,
            [player_id, message](boost::system::error_code ec, std::size_t bytes_sent) {
                if (!ec) {
                }
                else {
                    std::cerr << "Error sending message to player " << player_id << ": " << ec.message() << std::endl;
                }
            });
    }
}
