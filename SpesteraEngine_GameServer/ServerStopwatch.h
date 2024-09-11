#include <boost/asio.hpp>
#include <functional>
#include <memory>
#include <iostream>

class ServerStopwatch : public std::enable_shared_from_this<ServerStopwatch> {
public:
    using Callback = std::function<void()>;

    ServerStopwatch(boost::asio::io_context& io_context);

    void tick(int ms, Callback callback);
    void tick_once(int ms, Callback callback);

private:
    void start_ticking();

    boost::asio::io_context& io_context_;
    boost::asio::steady_timer timer_;
    int interval_;
    Callback callback_;
};