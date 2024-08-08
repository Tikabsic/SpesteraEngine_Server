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

ResponseWrapper DbConnection::get_data(const RequestWrapper& query) {
    ResponseWrapper wrapper;
    if (query.request_type() == RequestWrapper::REQUESTPLAYERINITIALDATA) {


        RequestPlayerInitialData request;
        request.ParseFromString(query.request_payload());

        mysqlx::Table gameCharacters = session->getSchema("spestera_db").getTable("game_characters");
        mysqlx::RowResult res = gameCharacters.select("player_name", "position_x", "position_y", "position_z", "rotation_y", "player_movementspeed")
            .where("player_id = :player_id")
            .bind("player_id", request.player_guid())
            .execute();

        mysqlx::Row row = res.fetchOne();
        if (!row.isNull()) {
            DbPlayerInitialData dbData;
            dbData.set_player_name(row[0].get<std::string>());
            dbData.set_position_x(row[1].get<float>());
            dbData.set_position_y(row[2].get<float>());
            dbData.set_position_z(row[3].get<float>());
            dbData.set_rotation_y(row[4].get<uint32_t>());
            dbData.set_player_movementspeed(row[5].get<uint32_t>());

            std::string payload;
            dbData.SerializeToString(&payload);

            wrapper.set_response_type(ResponseWrapper::RESPONSEPLAYERINITIALDATA);
            wrapper.set_response_id(query.request_id());
            wrapper.set_response_payload(payload);

        }
        else {
            std::cerr << "Player not found with ID: " << request.player_guid() << std::endl;
        }
    }

    return wrapper;
}

void DbConnection::stop() {
    std::unique_lock<std::mutex> lock(mtx);
    running = false;
    std::cout << "Connection stop" << std::endl;
    cv.notify_all();
}

void DbConnection::execute_query(const RequestWrapper query) {
    try {
        
    }
    catch (mysqlx::Error& err) {
        std::cerr << "MySQL Error during query execution: " << err.what() << std::endl;
    }
    catch (std::exception& e) {
        std::cerr << "Exception during query execution: " << e.what() << std::endl;
    }
}