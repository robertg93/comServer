#pragma once
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
	Message();
	~Message();
};

