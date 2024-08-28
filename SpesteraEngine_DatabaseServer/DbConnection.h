#pragma once

#include <iostream>
#include <mysqlx/xdevapi.h>
#include <queue>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <string>

#include "DatabaseProtocol.pb.h"

class DbConnection {
public:
    DbConnection(const std::string& uri);
    ~DbConnection();

    void process_queries();
    void add_query_broker(const std::string& query);
    void stop();

private:
    void execute_query(const DatabaseRequestWrapper& query);

    mysqlx::Session* session;
    std::queue<std::string> queryBuffer;
    std::mutex mtx;
    std::condition_variable cv;
    bool running = true;
};
