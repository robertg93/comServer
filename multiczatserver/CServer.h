#include "pch.h"
#pragma once
class CServer
{
private:
	CServer();
	static CServer* ms_ptrObject;

public:

	static CServer* Object();
	void run();
	~CServer();

};

