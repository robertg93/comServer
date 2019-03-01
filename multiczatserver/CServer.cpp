#include "pch.h"
#include "CServer.h"

CServer* CServer::ms_ptrObject = NULL;	

CServer::CServer() { ms_ptrObject = this; }

CServer * CServer::Object()
{
	if (ms_ptrObject == NULL) CServer();
	return ms_ptrObject;
}

void CServer::run()
{
}

CServer::~CServer() { ms_ptrObject = NULL; }


