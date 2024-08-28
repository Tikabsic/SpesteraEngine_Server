#pragma once

#include <string>

#include "AccountRepository.h"
#include "DatabaseProtocol.pb.h"

class AccountService
{
public:
	AccountService(std::string dburi);
	~AccountService();

	DatabaseResponseWrapper handle_message(const AccountServiceRequestWrapper& wrapper);

private:
	AccountRepository* repository_;
};

