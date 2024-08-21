#include "UdpServer.h"
#include "BinaryCompressor.h"
#include <iostream>
#include "SSKey.h"

UdpServer::UdpServer(boost::asio::io_context& io_context, const std::string& address, int port)
    : socket_(io_context, udp::endpoint(boost::asio::ip::make_address(address), port)){

    conn_manager_ = std::make_shared<ConnectionsManager>();
    receive_data();
    std::cout << "UDP Server on" << std::endl;
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
                    ZSWrapper wrapper;
                    if (!wrapper.ParseFromArray(receive_data_, bytes_received)) {
                        throw std::runtime_error("Failed to parse Wrapper message");
                    }

                    if (wrapper.type() == ZSWrapper::ASSIGNCHARACTER) {
                        AssignCharacter login_request;
                        if (login_request.ParseFromString(wrapper.payload())) {
                            //SSKey key;
                            //key.
                            //{
                            //    std::lock_guard<std::mutex> lock(endpoint_map_mutex_);
                            //    conn_manager_->add_new_connection(player_id, sender_endpoint_);
                            //}
                            //this->response_to_login_request(sender_endpoint_);
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

void UdpServer::send_data_to_player(udp::endpoint endpoint, const std::string& message) {
    auto send_buffer = std::make_shared<std::string>(message);
    socket_.async_send_to(
        boost::asio::buffer(*send_buffer), endpoint,
        [send_buffer](boost::system::error_code ec, std::size_t bytes_sent) {
            if (!ec) {
                // Mo¿esz tutaj dodaæ kod loguj¹cy lub inny kod, który ma byæ wykonany po udanym wys³aniu.
            }
            else {
                std::cerr << "Error sending response: " << ec.message() << std::endl;
            }
        });
}

void UdpServer::response_to_login_request(udp::endpoint endpoint) {;
    PlayerInitialData p_data;
    response.set_data(message);
    Wrapper wrapper;
    wrapper.set_type(Wrapper::RESPONSE);
    wrapper.set_payload(response.SerializeAsString());
    std::string serialized_msg = wrapper.SerializeAsString();
    auto send_buffer = std::make_shared<std::string>(serialized_msg);
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

//void UdpServer::send_data_to_all_players(const std::string& message) {
//
//    if (conn_manager_->connections_.empty()) {
//        std::cout << "No players online" << std::endl;
//        return;
//    }
//
//    for (const auto& connection : conn_manager_->connections_) {
//        const short& player_id = connection.first;
//        const udp::endpoint& endpoint = connection.second->udp_connection_;
//        send_buffer_ = message;
//
//        socket_.async_send_to(
//            boost::asio::buffer(send_buffer_), endpoint,
//            [player_id, message](boost::system::error_code ec, std::size_t bytes_sent) {
//                if (!ec) {
//                }
//                else {
//                    std::cerr << "Error sending message to player " << player_id << ": " << ec.message() << std::endl;
//                }
//            });
//    }
//}

void UdpServer::send_data_to_other_players(const std::string& message, u_short playerId)
{

    if (!conn_manager_->is_anyone_online()) {
        std::cout << "No players online" << std::endl;
        return;
    }

    auto& connections = conn_manager_->get_connections();

    for (const auto& connection : connections) {
        const short& player_id = connection.first;
        const udp::endpoint& endpoint = connection.second;

        if (player_id == playerId) {
            continue;
        }

        auto send_buffer = std::make_shared<std::string>(message);

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
