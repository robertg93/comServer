#pragma once

#include "Message.h"
#include <map>
#include <iostream>
#include <string>
#include <set>
#include <stdio.h>
#include <vector>
#include <winsock2.h>
#include <ws2tcpip.h>



#pragma comment(lib, "Ws2_32.lib")

#define PORT 9034
#define DEFAULT_MESSAGE_LEN 256
#define DEFAULT_RECV_FLAG 0
class Message;

class CCommunicator
{
private:



	CCommunicator();
	static CCommunicator* s_ptrComObject;
	fd_set master; // main set of descriptors 
	fd_set read_fds; // additional set if descriptors select()
	struct sockaddr_in myaddr; // server adress
	struct sockaddr_in remoteaddr; // client adress
	int addrlen = 0;
	int fdmax = 0; // maximum value of descriptor
	int listener = 0; // deskryptor of main listenin socket
	int newfiledescriptor = 0; // new accepted file descriptor
	int destfd = 0;
	char buf[256]; // buffor for incoming data 
	int nbytes = 0;
	int yes = 1; // for setsockopt() SO_REUSEADDR, patrz ni¿ej
	int currentFiledscp = 0;

	int i = 0, j = 0;

	std::set<int> userss;
	std::set<int> descriptorSet;
	std::map<int, int> userssFD;
	std::map<int, std::vector<Message>> notSentMessages;
public:

	~CCommunicator();

	static CCommunicator* createObject();
	void run();
	void startConnection();
	void connectionHandling();
	void handleNewConnection();
	void handleExistnigConnection();
};

