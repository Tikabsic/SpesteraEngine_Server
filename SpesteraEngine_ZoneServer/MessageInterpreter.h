#pragma once
#include "UdpServer.h"
#include "ConnectionsManager.h"

class MessageInterpreter
{
public:
	MessageInterpreter();
	~MessageInterpreter();
	void test();
private:
	UdpServer& udp_server_;

};

