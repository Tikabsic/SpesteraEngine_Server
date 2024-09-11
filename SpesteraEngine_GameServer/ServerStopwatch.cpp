#include "ServerStopwatch.h"

ServerStopwatch::ServerStopwatch(boost::asio::io_context& io_context) :
io_context_(io_context), timer_(io_context)
{
}

void ServerStopwatch::tick(int ms, Callback callback)
{
    callback_ = callback;
    interval_ = ms;
    start_ticking();
}

void ServerStopwatch::tick_once(int ms, Callback callback){
    timer_.expires_after(std::chrono::milliseconds(interval_));
    timer_.async_wait([this](const boost::system::error_code& ec) {
        if (!ec) {
            if (callback_) {
                callback_();
            }
        }
        });
}

void ServerStopwatch::start_ticking(){
    timer_.expires_after(std::chrono::milliseconds(interval_));
    timer_.async_wait([this](const boost::system::error_code& ec) {
        if (!ec) {
            if (callback_) {
                callback_();
            }
            start_ticking();
        }
        });
}
