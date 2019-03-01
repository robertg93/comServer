#include "pch.h"





int main()
{
	CServer* ptr = CServer::Object();

	//ptr->run();

	fd_set master; // main set of descriptors 
	fd_set read_fds; // additional set if descriptors select()
	struct sockaddr_in myaddr; // server adress
	struct sockaddr_in remoteaddr; // client adress
	int fdmax; // maximum value of descriptor
	int listener; // deskryptor of main listenin socket
	int newfd; // new accepted file descriptor
	int destfd;
	char buf[256]; // buffor for incoming data 
	int nbytes;
	int yes = 1; // for setsockopt() SO_REUSEADDR, patrz niżej
	int addrlen;
	int i, j;
	int iResult;

	std::set<int> userss;
	std::map<int, int > userssFD;

	WSADATA wsaData;

	FD_ZERO(&master); // clear main and additional set
	FD_ZERO(&read_fds);




	// initialize WINSOCK
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0)
	{
		printf(" WSAStartup failed");
		//return 1;
	}


	// create lintening socket
	if ((listener = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		//exit(1);
	}

	// skip  "address already in use"
	if (setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, (char*)&yes, sizeof(int)) == -1) {
		perror("setsockopt");
		//exit(1);
	}

	// bind
	myaddr.sin_family = AF_INET;
	myaddr.sin_addr.s_addr = INADDR_ANY;
	myaddr.sin_port = htons(PORT);
	memset(&(myaddr.sin_zero), '\0', 8);
	if (bind(listener, (struct sockaddr *) & myaddr, sizeof(myaddr)) == -1) {
		perror("bind");
		//exit(1);
	}

	// listen
	if (listen(listener, 10) == -1) {
		perror("listen");
		//exit(1);
	}

	// add listening socket to mian set
	FD_SET(listener, &master);

	// follow max descriptor
	fdmax = listener; // in this moment listenr is max

	std::cout << "petla glowna " << std::endl;
	// main loop
	for (;; ) {
		read_fds = master; // copy it because selcet cleans unused connections
		if (select(fdmax + 1, &read_fds, NULL, NULL, NULL) == -1) {
			perror("select");
			//exit(1);
		}
		std::cout << "here 1 " << std::endl;

		// go through the current connection for data to be read
		for (i = 0; i <= fdmax; i++) {
			if (FD_ISSET(i, &read_fds)) {
				if (i == listener) //new or existing connection
				{
					// handle new connection
					User newuser;


					std::cout << "here 2" << std::endl;
					addrlen = sizeof(remoteaddr);
					newfd = accept(listener, (struct sockaddr *) & remoteaddr, &addrlen);

					if (newfd == INVALID_SOCKET) // if accept failed
					{
						std::cout << "here 3 " << std::endl;
						perror("accept");
					}
					else // if accept returns new descriptor
					{
						Message newmsg = MessageReciver::receive(newfd);  // get message
						std::cout << "here 4 " << std::endl;
						// add to ussers set
						if (newmsg.getnumOfReturnedBytes() > 0) //if recv returned > 0
						{
							// get user id
							std::cout << "add user" << std::endl;
							int userID = newmsg.getSendingUserID();
							if (userss.count(userID) == 0)
							{
								userss.insert(userID);
								userssFD.insert(std::pair<int, int>(userID, newfd));
							}
						}
						else //if recv failed
						{
							perror("recv");
						}

						FD_SET(newfd, &master); // add to main set
						if (newfd > fdmax)  // follow max
						{
							fdmax = newfd;
						}

					}
				}
				else {
					std::cout << "tu jestem 5 " << std::endl;
					// handle data from client

					Message newmsg = MessageReciver::receive(i);

					nbytes = newmsg.getnumOfReturnedBytes();
					//nbytes = recv(newfd, buf, 256, 0);
					if (nbytes <= 0) //newmsg.getnumOfReturnedBytes
					{
						// error or lost connection
						if (nbytes == 0) {
							printf("selectserver: socket %d hung up\n", i);
						}
						else {
							perror("recv");
						}
						closesocket(i); // papa!
						FD_CLR(i, &master); // reamove from main set
						//userssFD.erase(newmsg.getSendingUserID());
					}
					else {
						std::cout << "tu jestem 6 " << std::endl;
						// handle data
						std::cout << newmsg.getMessageType() << std::endl;
						if (newmsg.getMessageType() == 2) //to certein user
						{
							if ((Messagesender::sender(newmsg, userssFD[newmsg.getReceivingUserID()])) == -1) {
								perror("send");
							}
						}
						else
						{
							for (j = 0; j <= fdmax; j++) {
								// wyślij do wszystkich!
								if (FD_ISSET(j, &master)) {


									// oprócz nas i gniazda nasłuchującego
									if (j != listener && j != i) {
										if ((Messagesender::sender(newmsg, j)) == -1) {
											perror("send");
										}
										std::cout << "here 8 " << std::endl;
									}


								}
							}
						}

					}
				}
			}
		}
	}
}



