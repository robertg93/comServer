#pragma once
#include "pch.h"
class Message
{

	friend class MassgeReceiver;
private:
	int messageType;
	int sendingUserID;
	int receivingUserID;
	std::string data;
	int numOfReturnedBytes;
	int descriptor;
	

public:
	Message();
	
	void setMessageType(char *);
	void setSendingUserID(char *);
	void setReceivingUserID(char *);
	void setReceivingUserID(int);
	void setData(char *buf, int len);
	void setnumOfReturnedBytes(int len);
	void setdescriptor(int);
	

	int getMessageType();
	int getSendingUserID();
	int getReceivingUserID();
	std::string getData();
	int getnumOfReturnedBytes();
	int getdescriptor();

	~Message();
};

