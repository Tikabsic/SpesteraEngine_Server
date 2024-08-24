#pragma once

#include <string>

#include "AccountRepository.h"
#include "DatabaseProtocol.pb.h"

class AccountService
{
public:
	AccountService(std::string dburi);
	~AccountService();

	ResponseWrapper handle_message(const RequestWrapper& wrapper);

private:
	AccountRepository* repository_;
};

