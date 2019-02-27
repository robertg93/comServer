#pragma once
#include "pch.h"


class MessageReciver
{

public:
	MessageReciver();
	~MessageReciver();

	static Message receive(int descriptor);
};
	

