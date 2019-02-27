#include "pch.h"
#include "MessageReciver.h"



MessageReciver::MessageReciver()
{
}


MessageReciver::~MessageReciver()
{
}

Message MessageReciver::receive(int descriptor)
{
	
	char buf[DEFAULT_MESSAGE_LEN];
	Message msg;
	int bytesNum = recv(descriptor, buf, DEFAULT_MESSAGE_LEN, DEFAULT_RECV_FLAG);
	msg.setMessageType(buf);
	msg.setSendingUserID(buf);
	msg.setReceivingUserID(buf);
	msg.setData(buf, bytesNum);
	msg.setnumOfReturnedBytes(bytesNum);
	
	

	return msg;
}
