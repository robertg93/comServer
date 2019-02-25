#pragma once
#include "pch.h"
#include "Message.h"

class MessageReciver
{
public:
	MessageReciver();
	~MessageReciver();

	Message receive(int descriptor);
	int getMessageType(char *);
	int getSenderID(char *);
	int getDestinationID(char *);
};

