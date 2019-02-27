#include "pch.h"
#include "Message.h"


Message::Message()
{
}

int Message::getMessageType()
{
	return messageType;
}

int Message::getSendingUserID()
{
	return sendingUserID;
}

int Message::getReceivingUserID()
{
	return receivingUserID;
}

std::string Message::getData()
{
	return data;
}

int Message::getnumOfReturnedBytes()
{
	return numOfReturnedBytes;
}

int Message::getdescriptor()
{
	return descriptor;
}

void Message::setMessageType(char * tempBuf)
{
	if (tempBuf[0] == '1') messageType = 1;
	if (tempBuf[0] == '2') messageType = 2;
	else messageType = -1;

}


void  Message::setSendingUserID(char * tempBuf)
{
	std::string temp = "";
	
	for (int n = 1; n < 7; n++)
	{
		temp = temp + tempBuf[n];
	}
	sendingUserID = stoi(temp);

}

void  Message::setReceivingUserID(char * tempBuf)
{
	std::string temp = "0";
	for (int n = 7; n < 13; n++)
	{
		temp = temp + tempBuf[n];
	}
	receivingUserID = stoi(temp);
}

void Message::setReceivingUserID(int recv)
{
	receivingUserID = recv;
}

void Message::setData(char * buf, int len)
{
	std::string str = "";
	for (int i = 13; i < len; i++)
	{
		str = str + buf[i];
	}
	data = str;
}
void Message::setnumOfReturnedBytes(int len)
{
	numOfReturnedBytes = len;
}

void Message::setdescriptor(int dsc)
{
	descriptor = dsc;
}



Message::~Message()
{
}
