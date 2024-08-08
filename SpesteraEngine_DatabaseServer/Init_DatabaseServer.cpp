#include <boost/asio.hpp>
#include <memory>
#include <thread>

#include "TcpConnector.h"
#include "ConfigReader.h"
#include "DbConnection.h"

int main() {
	try {
		ConfigReader config_reader("Config.ini");

		std::string db_ip = config_reader.get("Connection", "ip");
		int mysql_port = std::stoi(config_reader.get("Connection", "mysql_port"));
		std::string db_name = config_reader.get("Connection", "db_name");
		int tcp_port = std::stoi(config_reader.get("Connection", "tcp_port"));

		std::string acc = config_reader.get("MySql_Account", "acc");
		std::string pwd = config_reader.get("MySql_Account", "password");

		std::string uri = "mysqlx://" + acc + ":" + pwd + "@" + db_ip + ":" + std::to_string(mysql_port) + "/" + db_name;

		std::unique_ptr<DbConnection> db_connection = std::make_unique<DbConnection>(uri);

		std::thread processing_thread([&]() {
			db_connection->process_queries();
			});

		boost::asio::io_context io_context;
		auto tcp_connector = std::make_unique<TcpConnector>(io_context, db_ip, tcp_port, db_connection.get());

		io_context.run();

		if (processing_thread.joinable()) {
			processing_thread.join();
		}

		db_connection->stop();
	}
	catch (mysqlx::Error& err) {
		std::cerr << "MySQL Error: " << err.what() << std::endl;
	}
	catch (std::exception& e) {
		std::cerr << "Exception: " << e.what() << std::endl;
	}

	return 0;
}