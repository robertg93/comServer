#include "pch.h"
#include "Messagesender.h"
#include <string>

Messagesender::Messagesender()
{
}

int Messagesender::sender(Message msg,int descriptor)
{
	std::string temp = msg.getData();
	int bufsize = msg.getnumOfReturnedBytes();
	char buf[256];
	int ID = msg.getReceivingUserID();
	
	
	for (int i = 0; i < temp.size(); i++)
		{
			buf[i] = temp[i];
		}
	return send(descriptor, buf, temp.size() , 0);
}


Messagesender::~Messagesender()
{
}
