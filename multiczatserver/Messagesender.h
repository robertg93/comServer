#pragma once
#include "pch.h"


class Messagesender
{
public:
	
	Messagesender();
	
	 static int sender(Message newmsg,int descriptor);
	~Messagesender();
};

