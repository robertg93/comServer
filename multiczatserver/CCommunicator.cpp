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
		for (i = 0; i <= fdmax; i++) {
			if (FD_ISSET(i, &read_fds)) { // iteracja po deskryptorach w read_fds
				//add new connection
				if (i == listener)
				{
					handleNewConnection();

				}
				// existing connection 
				else
				{
					handleExistnigConnection();
				}
			}
		}
	}
}

void CCommunicator::handleNewConnection()
{
	// handle new connection
	std::cout << "here 2 " << std::endl;
	addrlen = sizeof(remoteaddr);
	newfiledescriptor = accept(listener, (struct sockaddr *) & remoteaddr, &addrlen);

	if (newfiledescriptor == INVALID_SOCKET) // if accept failed
	{
		std::cout << "here 3 " << std::endl;
		perror("accept");
	}
	else // if accept returns new descriptor
	{
		std::cout << "here 4 " << std::endl;
		// add to user list

		if ((recv(newfiledescriptor, buf, 256, 0)) > 0) //if recv returned > 0
		{
			// get user id
			Message newMessage;

			std::string temp = "";
			int userID;
			for (int n = 1; n < 7; n++)
			{
				temp = temp + buf[n];
			}
			userID = stoi(temp);

			if (userss.count(userID) == 0)
			{
				userss.insert(userID);
				userssFD.insert(std::pair<int, int>(userID, newfiledescriptor));
			}

		}
		else //if recv failed
		{
			perror("recv");
		}
		FD_SET(newfiledescriptor, &master); // add to main set
		if (newfiledescriptor > fdmax)  // follow max
		{
			fdmax = newfiledescriptor;
		}

	}
}

void CCommunicator::handleExistnigConnection()
{
	std::cout << "here 5 " << std::endl;
	// handle data from client


	Message newMessage;

	nbytes = recv(i, buf, 256, 0);

	newMessage.massageType = buf[0];
	newMessage.receivedBytes = nbytes;
	std::string temp = "";
	for (int k = 1; k < 7; k++) { temp = temp + buf[k]; }
	newMessage.senderID = stoi(temp);
	temp = "";
	for (int k = 7; k < 13; k++) { temp = temp + buf[k]; }
	newMessage.receiverID = stoi(temp);
	temp = "";
	for (int k = 13; k < nbytes; k++) { temp = temp + buf[k]; }
	newMessage.data = temp;

	if (nbytes <= 0)
	{
		// error
		if (nbytes == 0) {
			// connection lost
			printf("selectserver: socket %d hung up\n", i);
		}
		else {
			perror("recv");
		}
		closesocket(i); // papa!
		FD_CLR(i, &master); // remove from main set
	}
	else {
		std::cout << "tu jestem 6 " << std::endl;
		// some data from client

		if (buf[0] == '2') //to certein user
		{

			std::string temp = "";
			for (int i = 7; i < 13; i++)
			{
				temp = temp + buf[i];
			}
			destfd = userssFD[stoi(temp)];
			if (send(destfd, buf, nbytes, 0) == -1) {
				perror("send");
			}
			std::cout << "here 7 " << std::endl;
		}
		else
		{
			for (j = 0; j <= fdmax; j++) {
				// for all
				if (FD_ISSET(j, &master)) {


					// without as and server
					if (j != listener && j != i) {
						if (send(j, buf, nbytes, 0) == -1) {
							perror("send");
						}
						std::cout << "tu jestem 8 " << std::endl;
					}


				}
			}
		}

	}
}
