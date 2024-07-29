#include "External_Connection.h"

External_Connection::External_Connection(boost::asio::ip::tcp::socket socket) :
	socket_(std::move(socket))
{

}
