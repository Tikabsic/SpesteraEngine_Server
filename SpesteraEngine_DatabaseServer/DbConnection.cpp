#include "DbConnection.h"

DbConnection::DbConnection(const std::string& uri) {
    try {
        session = new mysqlx::Session(uri);
        std::cout << "Database connection established." << std::endl;
    }
    catch (mysqlx::Error& err) {
        std::cerr << "MySQL Error: " << err.what() << std::endl;
    }
    catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
}

DbConnection::~DbConnection() {
    if (session) {
        std::cout << "DbConnection closed" << std::endl;
        cv.notify_one();
        session->close();
        delete session;
    }
}

void DbConnection::process_queries() {
    std::unique_lock<std::mutex> lock(mtx);
    while (running) {
        cv.wait(lock, [this]() { return !queryBuffer.empty() || !running; });

        while (!queryBuffer.empty()) {
            std::string query = queryBuffer.front();
            queryBuffer.pop();
            lock.unlock();
            //execute_query(query);
            lock.lock();
        }
    }
}

void DbConnection::add_query_broker(const std::string& query) {
    std::unique_lock<std::mutex> lock(mtx);
    queryBuffer.push(query);
    cv.notify_one();
}

void DbConnection::stop() {
    std::unique_lock<std::mutex> lock(mtx);
    running = false;
    std::cout << "Connection stop" << std::endl;
    cv.notify_all();
}

void DbConnection::execute_query(const DatabaseRequestWrapper& query) {
    try {
        
    }
    catch (mysqlx::Error& err) {
        std::cerr << "MySQL Error during query execution: " << err.what() << std::endl;
    }
    catch (std::exception& e) {
        std::cerr << "Exception during query execution: " << e.what() << std::endl;
    }
}