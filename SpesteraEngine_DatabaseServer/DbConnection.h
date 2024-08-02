#ifndef DBCONNECTION_H
#define DBCONNECTION_H

#include <iostream>
#include <mysqlx/xdevapi.h>
#include <queue>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <string>

class DbConnection {
public:
    DbConnection(const std::string& uri);
    ~DbConnection();

    void process_queries();
    void add_query_broker(const std::string& query);
    mysqlx::RowResult get_data(std::string const query);
    void stop();

private:
    void execute_query(const std::string& query);

    mysqlx::Session* session;
    std::queue<std::string> queryBuffer;
    std::mutex mtx;
    std::condition_variable cv;
    bool running = true;
};

#endif // DBCONNECTION_H