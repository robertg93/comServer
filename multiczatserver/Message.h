#pragma once
#include "CCommunicator.h"
#include <string>
class Message
{
	friend class CCommunicator;
protected:

	int massageType;
	int receivedBytes;
	int senderID;
	int receiverID;
	std::string data;
public:
	int receive(int newDscp);
	int sendMsg(int userDscp);
	Message();
	~Message();
};

