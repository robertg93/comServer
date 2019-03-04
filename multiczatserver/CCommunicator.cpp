#include "pch.h"
#include "CCommunicator.h"

CCommunicator* CCommunicator::s_ptrComObject = nullptr;

CCommunicator::CCommunicator()
{
	s_ptrComObject = this;
}

CCommunicator::~CCommunicator()
{
	s_ptrComObject = nullptr;
}

CCommunicator* CCommunicator::run()
{
	if (s_ptrComObject == nullptr) { new CCommunicator(); }
	return s_ptrComObject;
}



void CCommunicator::startConnection()
{
	FD_ZERO(&master); // clear main and additional set
	FD_ZERO(&read_fds);

	WSADATA wsaData;

	int iResult;
	// initialize WINSOCK
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0)
	{
		printf(" WSAStartup failed");
		//return 1;
	}


	// creatr listening socket
	if ((listener = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		exit(1);
	}

	//skip the error "address already in use"
	if (setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, (char*)&yes, sizeof(int)) == -1) {
		perror("setsockopt");
		exit(1);
	}

	// bind
	myaddr.sin_family = AF_INET;
	myaddr.sin_addr.s_addr = INADDR_ANY;
	myaddr.sin_port = htons(PORT);
	memset(&(myaddr.sin_zero), '\0', 8);
	if (bind(listener, (struct sockaddr *) & myaddr, sizeof(myaddr)) == -1) {
		perror("bind");
		exit(1);
	}

	// listen
	if (listen(listener, 10) == -1) {
		perror("listen");
		exit(1);
	}

	// add to main set 
	FD_SET(listener, &master);

	// followe the bigest descriptor
	fdmax = listener; // this is max now
}

void CCommunicator::connectionHandling()
{
	std::cout << "main loop " << std::endl;
	// main loop
	while (true)
	{
		std::cout << "waiting for action " << std::endl;
		read_fds = master; // copy it
		if (select(fdmax + 1, &read_fds, NULL, NULL, NULL) == -1) {
			perror("select");
			exit(1);
		}

		// przejdŸ przez obecne po³¹czenia szukaj¹c danych do odczytania
		for (i = 0; i <= fdmax; i++)
		{
			 currentFiledscp = i;
			if (FD_ISSET(currentFiledscp, &read_fds))  // iteracja po deskryptorach w read_fds
			{
				if (currentFiledscp == listener)  handleNewConnection();		    //add new connection
				else handleExistnigConnection();									// existing connection 
			}
		}
	}
}

void CCommunicator::handleNewConnection()
{
	// handle new connection
	std::cout << "adding new user... " << std::endl;
	addrlen = sizeof(remoteaddr);
	newfiledescriptor = accept(listener, (struct sockaddr *) & remoteaddr, &addrlen);

	if (newfiledescriptor == INVALID_SOCKET) // if accept failed
	{
		perror("accept");
	}
	else // if accept returns new descriptor
	{
		std::cout << "new user " << std::endl;
		// add to user list
		Message message;
		if (message.receive(newfiledescriptor) > 0) //if recv returned > 0
		{
			// get user id
			int userID = message.senderID;
			if (userss.count(userID) == 0) { userss.insert(userID); }    //add user to set 
			userssFD.insert(std::pair<int, int>(userID, newfiledescriptor)); // add pair (user,fd) to map
			
		}
		else {perror("recv");}					// if recv failed
		
		descriptorSet.insert(newfiledescriptor);
		FD_SET(newfiledescriptor, &master);				// add to main set
		if (newfiledescriptor > fdmax)				 // follow max
		{
			fdmax = newfiledescriptor;
		}
	}
}

void CCommunicator::handleExistnigConnection()
{
	std::cout << "destination choice " << std::endl;
	// handle data from client
	
	Message newMessage;
	newMessage.receive(currentFiledscp);

	if (newMessage.receivedBytes <= 0)
	{
		// error
		if (newMessage.receivedBytes== 0) {
			// connection lost
			printf("selectserver: socket %d hung up\n", currentFiledscp);
		}
		else {
			perror("recv");
		}
		closesocket(currentFiledscp); // papa!
		FD_CLR(currentFiledscp, &master); // remove from main set
		userssFD.erase(newMessage.senderID);
	}
	else {
		std::cout << "senging... " << std::endl;
		// some data from client
		
		if (newMessage.massageType == 2) //to certein user
		{
			destfd = userssFD[newMessage.receiverID];
			if (newMessage.sendMsg(destfd) == -1) {
				perror("send");
			}
			std::cout << "msg was send " << std::endl;
		}
		else
		{	// for all
			for (auto iter = descriptorSet.begin(); iter != descriptorSet.end(); ++iter)
			{
				if (*iter != currentFiledscp && *iter != listener) newMessage.sendMsg(*iter);
			}

		}
	}
}
