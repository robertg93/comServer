#include "pch.h"


using namespace std;


int main()
{
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

	set<User> users;

	set<int> userss;
	map<int, sockaddr_in> usersAddress;
	map<int, int> usersFD;

	FD_ZERO(&master); // wyczyść główny i pomocniczy zestaw
	FD_ZERO(&read_fds);

	WSADATA wsaData;

	int iResult;
	// initialize WINSOCK
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0)
	{
		printf(" WSAStartup failed");
		return 1;
	}


	// utwórz gniazdo nasłuchująceo
	if ((listener = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		exit(1);
	}

	//zgub wkurzający komunikat błędu "address already in use"
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

	// dodaj gniazdo nasłuchujące do głównego zestawu
	FD_SET(listener, &master);

	// śledź najwyższy numer deskryptora pliku
	fdmax = listener; // póki co, ten jest największy

	cout << "petla glowna " << endl;
	// pętla główna
	for (;; ) {
		read_fds = master; // copy it
		if (select(fdmax + 1, &read_fds, NULL, NULL, NULL) == -1) {
			perror("select");
			exit(1);
		}
		cout << "tu jestem 1 " << endl;
		
		// przejdź przez obecne połączenia szukając danych do odczytania
		for (i = 0; i <= fdmax; i++) {
			if (FD_ISSET(i, &read_fds)) { // iteracja po deskryptorach w read_fds
				if (i == listener) //new or existing connection
				{
					// handle new connection
					User newuser;
					//cout << "tu jestem 2 " << endl;
					addrlen = sizeof(remoteaddr);
					newfd = accept(listener, (struct sockaddr *) & remoteaddr, &addrlen);

					if (newfd == INVALID_SOCKET) // if accept failed
					{
						cout << "tu jestem 3 " << endl;
							perror("accept");
					}
					else // if accept returns new descriptor
					{
						cout << "tu jestem 4 " << endl;
						// dodanie do listy uzytkowników
						if ((recv(newfd, buf, 256, 0)) > 0) //if recv returned > 0
						{
							// get user id
							string temp = "";
							int userID;
								for (int n = 1; n < 7; n++)
								{
									temp = temp + buf[n];
								}
								userID = stoi(temp);
								if (userss.count(userID) == 0)
								{
									userss.insert(userID);
									usersAddress.insert(pair<int, sockaddr_in>(userID, remoteaddr));
									usersFD.insert(pair<int, int>(userID, newfd));
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
					cout << "tu jestem 5 " << endl;
					// obsłuż dane od klienta
					nbytes = recv(i, buf, 256, 0);
					if ( nbytes <= 0) 
					{
						// błąd lub połączenie zostało zerwane
						if (nbytes == 0) {
							// połączenie zerwera
							printf("selectserver: socket %d hung up\n", i);
						}
						else {
							perror("recv");
						}
						closesocket(i); // papa!
						FD_CLR(i, &master); // usuń z głównego zestawu
					}
					else {
						cout << "tu jestem 6 " << endl;
						// mamy trochę danych od klienta

						if (buf[0] == '2') //to certein user
						{
							string temp = "";
							for (int n = 7; n < 13; n++)
							{
								temp = temp + buf[n];
							}
							//cout << temp << endl;
							destfd = usersFD[stoi(temp)];
							if (send(destfd, buf, nbytes, 0) == -1) {
								perror("send");
							}
							cout << "tu jestem 7 " << endl;
						}
						else
						{
							for (j = 0; j <= fdmax; j++) {
								// wyślij do wszystkich!
								if (FD_ISSET(j, &master)) {


									// oprócz nas i gniazda nasłuchującego
									if (j != listener && j != i) {
										if (send(j, buf, nbytes, 0) == -1) {
											perror("send");
										}
										cout << "tu jestem 8 " << endl;
									}


								}
							}
						}
						
					}
				} // jakie to BRZYDKIE!!
			}
		}
	}


}
