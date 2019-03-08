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

CCommunicator* CCommunicator::createObject()
{
	if (s_ptrComObject == nullptr) { new CCommunicator(); }

	return s_ptrComObject;
}

void CCommunicator::run()
{
	//try to start connection
	bool tryFlag = true;
	while (tryFlag)
	{
		tryFlag = false;
		try
		{
			CCommunicator::startConnection();
		}
		catch (std::string info)
		{
			std::cout << info << std::endl;
			tryFlag = true;
		}
		catch (...)
		{
			std::cout << "other error" << std::endl;
			tryFlag = true;
		}

	}
	//try connection handling
	tryFlag = true;
	while (tryFlag)
	{
		tryFlag = false;
		try
		{
			CCommunicator::connectionHandling();
		}
		catch (std::string info)
		{
			std::cout << info << std::endl;
			tryFlag = true;
		}
		catch (...)
		{
			std::cout << "other error" << std::endl;
			tryFlag = true;
		}

	}

	
	
}



void CCommunicator::startConnection()
{
	FD_ZERO(&master); // clear main and additional set
	FD_ZERO(&read_fds);
	WSADATA wsaData;
	std::string exception;

	// initialize WINSOCK
	if ((WSAStartup(MAKEWORD(2, 2), &wsaData) == -1)) throw(exception = " WSAStartup failed");
	
	// creatr listening socket
	if ((listener = socket(AF_INET, SOCK_STREAM, 0)) == -1) throw(exception = " Socket error");

	//skip the error "address already in use"
	if (setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, (char*)&yes, sizeof(int)) == -1)throw(exception = " setsockopt error");

	// bind
	myaddr.sin_family = AF_INET;
	myaddr.sin_addr.s_addr = INADDR_ANY;
	myaddr.sin_port = htons(PORT);
	memset(&(myaddr.sin_zero), '\0', 8);
	if (bind(listener, (struct sockaddr *) & myaddr, sizeof(myaddr)) == -1) throw(exception = " bind error");
	// listen
	if (listen(listener, 10) == -1)throw(exception = " listen error");

	// add to main set 
	FD_SET(listener, &master);

	// followe the bigest descriptor
	fdmax = listener; // this is max now
}

void CCommunicator::connectionHandling()
{
	std::cout << "main loop " << std::endl;
	// main loop
	try 
	{
		while (true)
		{
			std::string exception;
			std::cout << "waiting for action " << std::endl;
			read_fds = master; // copy it
			if (select(fdmax + 1, &read_fds, NULL, NULL, NULL) == -1) throw(exception = " select failed");

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
	catch (...)
	{
		throw;
	}
}

void CCommunicator::handleNewConnection()
{
	// handle new connection
	std::string exception;
	std::cout << "adding new user... " << std::endl;
	addrlen = sizeof(remoteaddr);
	newfiledescriptor = accept(listener, (struct sockaddr *) & remoteaddr, &addrlen);
	// if accept failed
	if (newfiledescriptor == INVALID_SOCKET) { throw(exception = " invalid socket"); }
	else // if accept returns new descriptor
	{
		std::cout << "new user " << std::endl;
		// add to user list
		std::unique_ptr<Message> ptrMessage(new Message());

		if (ptrMessage->receive(newfiledescriptor) > 0) //if recv returned > 0
		{
			// get user id
			int userID = ptrMessage->senderID;
			if (userss.count(userID) == 0)  //add user to set 
			{
				userss.insert(userID);
				userssFD.insert(std::pair<int, int>(userID, newfiledescriptor));
			}   
			else
			{
				userssFD[userID] = newfiledescriptor;
			}
			 // add pair (user,fd) to map
			
			if (notSentMessages.count(ptrMessage->senderID) == 1)
			{
				for (auto iter = notSentMessages[ptrMessage->senderID].begin(); iter != notSentMessages[ptrMessage->senderID].end(); ++iter)
				{
					iter->sendMsg(newfiledescriptor);
				}
				notSentMessages.erase(ptrMessage->senderID);
			}

		}
		else { throw(exception = " receive error"); }					// if recv failed
		
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
	
	std::unique_ptr<Message> ptrMessage(new Message());
	ptrMessage->receive(currentFiledscp);

	if (ptrMessage->receivedBytes <= 0)
	{
		std::string exception;
		// error
		if (ptrMessage->receivedBytes== 0) { throw(exception = " socket hang up"); }
		else {
			closesocket(currentFiledscp); // papa!
			FD_CLR(currentFiledscp, &master); // remove from main set
			//userssFD.erase(newMessage.senderID);
			throw(exception = " user has closed connection");
		}
		
	}
	else {
		std::cout << "senging... " << std::endl;
		// some data from client
		if (ptrMessage->massageType == 2) //to certein user
		{
			destfd = userssFD[ptrMessage->receiverID];
			if (ptrMessage->sendMsg(destfd) == -1) {
				perror("send");
				//int userid = newMessage.receiverID;
				if(notSentMessages.count(ptrMessage->receiverID) == 0)
				{
					std::vector<Message> msgVec;
					msgVec.push_back(*ptrMessage);
					notSentMessages.insert(std::pair<int, std::vector<Message>>(ptrMessage->receiverID, msgVec));
				}
				else 
				{
					notSentMessages[ptrMessage->receiverID].push_back(*ptrMessage);
				}
				std::cout << notSentMessages[ptrMessage->receiverID][0].data << std::endl;
			}
			else std::cout << "msg was send " << std::endl;
			
		}
		else
		{	// for all
			for (auto iter = descriptorSet.begin(); iter != descriptorSet.end(); ++iter)
			{
				if (*iter != currentFiledscp && *iter != listener) ptrMessage->sendMsg(*iter);
			}
		}
	}
}
