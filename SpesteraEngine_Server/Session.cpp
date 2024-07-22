#include "Session.h"
#include "ServerHeartbeat.h"

#include <iostream>

Session::Session(boost::asio::ip::tcp::socket socket, int id, ServerHeartbeat& heartbeat)
    : socket_(std::move(socket)), id_(id), server_heartbeat_(heartbeat) {
}

void Session::start() {
    PlayerInitialData player_initial_data;
    player_initial_data.set_player_id(playerId_);
    player_initial_data.set_player_movementspeed(5);
    player_initial_data.set_position_x(100);
    player_initial_data.set_position_y(1);
    player_initial_data.set_position_z(100);
    player_initial_data.set_rotation_y(100);

    Wrapper wrapper;
    wrapper.set_type(Wrapper::PLAYERINITIALDATA);
    wrapper.set_payload(player_initial_data.SerializeAsString());
    write_msgs_.push_back(wrapper.SerializeAsString());

    character_ = std::make_shared<Player_Character>(100, 1, 100, 5, 180, playerId_);

    server_heartbeat_.push_player_character(character_);

    do_write();
    do_read();
}

void Session::compress_to_write(const Wrapper& msg) {
    bool write_in_progress = !write_msgs_.empty();
    std::string compressed_msg;
    BinaryCompressor compressor;
    compressor.compress_string(msg.SerializeAsString(), compressed_msg);
    write_msgs_.push_back(compressed_msg);
    std::cout << compressed_msg << std::endl;
    if (!write_in_progress) {
        do_write();
    }
}

void Session::set_player_id(u_short pid)
{
    playerId_ = pid;
}


void Session::handle_message(const Wrapper& wrapper) {
    switch (wrapper.type()) {
    case Wrapper::CLIENTLOGOUT: {
        std::cout << "Client logged out : " << playerId_ << std::endl;
        break;
    }
    case Wrapper::PLAYERPOSITION: {
        PlayerPosition position;
        if (position.ParseFromArray(wrapper.payload().data(), wrapper.payload().size())) {
            character_->move_player_character(position);
        }
        else {
            std::cerr << "Failed to parse PlayerPosition message" << std::endl;
        }
        break;
    }
    default:
        std::cout << "Unknown message type" << std::endl;
        break;
    }
}

u_short Session::get_player_id()
{
    return playerId_;
}

void Session::do_read() {
    auto self(shared_from_this());
    socket_.async_read_some(boost::asio::buffer(data_, max_length),
        [this, self](boost::system::error_code ec, std::size_t length) {
            if (!ec) {
                try{
                    Wrapper wrapper;
                    if (wrapper.ParseFromArray(data_, length)) {
                        handle_message(wrapper);
                    }
                    do_read();
                }
                catch (const std::runtime_error& e) {

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
                std::cout << "Sent data to player"<< std::endl;
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