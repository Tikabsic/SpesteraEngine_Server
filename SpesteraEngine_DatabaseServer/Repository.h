#pragma once

#include <iostream>
#include <mysqlx/xdevapi.h>
#include <queue>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <string>

#include "DatabaseProtocol.pb.h"

class Repository
{
protected:
	Repository(const std::string& uri);
	~Repository();

	mysqlx::Session* db_;
};

