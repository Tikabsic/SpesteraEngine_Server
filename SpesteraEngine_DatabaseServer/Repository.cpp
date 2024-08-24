#include "Repository.h"

Repository::Repository(const std::string& uri) {
    try {
        db_ = new mysqlx::Session(uri);
        std::cout << "AccountRepository connection established." << std::endl;
    }
    catch (mysqlx::Error& err) {
        std::cerr << "MySQL Error: " << err.what() << std::endl;
    }
    catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
}

Repository::~Repository() {
    if (db_) {
        delete db_;
        db_ = nullptr;
    }
}
