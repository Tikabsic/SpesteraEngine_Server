#pragma once

#include "Repository.h"

class AccountRepository : Repository
{
public:
    AccountRepository(const std::string& uri) : Repository(uri) {};
    ~AccountRepository() {};

    ResponseAccountData get_account_data(const RequestAccountData& data);

private:
    std::string table_name_ = "accounts";
};

