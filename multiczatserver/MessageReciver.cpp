
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
	int bytesNum;
	bytesNum = recv(descriptor, buf, DEFAULT_MESSAGE_LEN, DEFAULT_RECV_FLAG);
	msg.setnumOfByes(bytesNum);
	
	
	//to do
	
	return msg;
}

int MessageReciver::getMessageType(char * tempBuf)
{
	if (tempBuf[0] == '0' ) return 0;
	if (tempBuf[0] == '1') return 1;
	else return -1;

}


int MessageReciver::getSenderID(char * tempBuf)
{
	std::string temp = "";
	int userID;
	for (int n = 1; n < 7; n++)
	{
		temp = temp + tempBuf[n];
	}
	return stoi(temp);
	
}

int MessageReciver::getDestinationID(char * tempBuf)
{
	std::string temp = "";
	int userID;
	for (int n = 7; n < 13; n++)
	{
		temp = temp + tempBuf[n];
	}
	return stoi(temp);
}
