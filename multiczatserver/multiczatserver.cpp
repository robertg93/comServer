#include "pch.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <iostream>
#include <set>
#include <map>

#pragma comment(lib, "Ws2_32.lib")

#define PORT 9034

using namespace std;


int main()
{
	fd_set master; // główna lista deskryptorów plików
	fd_set read_fds; // pomocnicza lista deskryptorów dla select()
	struct sockaddr_in myaddr; // adres serwera
	struct sockaddr_in remoteaddr; // adres klienta
	int fdmax; // maksymalny numer deskryptora pliku
	int listener; // deskryptor gniazda nasłuchującego
	int newfd; // nowozaakceptowany deskryptor gniazda
	char buf[256]; // bufor na dane pochodzące od klienta
	int nbytes;
	int yes = 1; // dla setsockopt() SO_REUSEADDR, patrz niżej
	int addrlen;
	int i, j;
	set<int> users;
	map<int, int> usersTempAddress;

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
			if (FD_ISSET(i, &read_fds)) { // mamy jednego!! sprawdzanie po kolei od i do fdmax,
				if (i == listener) //jeżeli 
				{
					// obsłuż nowe połączenie
					cout << "tu jestem 2 " << endl;
					addrlen = sizeof(remoteaddr);
					newfd = accept(listener, (struct sockaddr *) & remoteaddr, &addrlen);

					if (newfd == INVALID_SOCKET) 
					{
						cout << "tu jestem 3 " << endl;
							perror("accept");
					}
					else 
					{
						cout << "tu jestem 4 " << endl;
						FD_SET(newfd, &master); // dodaj do głównego zestawu
						if (newfd > fdmax)  // śledź maksymalny
						{
							fdmax = newfd;
						}
						//char str[INET_ADDRSTRLEN];
						//inet_ntop(AF_INET, &remoteaddr, str, INET_ADDRSTRLEN);
						//printf("selectserver: new connection from %s on "
						//	"socket %d\n",*str, newfd);
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
						for (j = 0; j <= fdmax; j++) {
							// wyślij do wszystkich!
							if (FD_ISSET(j, &master)) {
								// oprócz nas i gniazda nasłuchującego
								if (j != listener) {
									if (send(j, buf, nbytes, 0) == -1) {
										perror("send");
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

// Uruchomienie programu: Ctrl + F5 lub menu Debugowanie > Uruchom bez debugowania
// Debugowanie programu: F5 lub menu Debugowanie > Rozpocznij debugowanie

// Porady dotyczące rozpoczynania pracy:
//   1. Użyj okna Eksploratora rozwiązań, aby dodać pliki i zarządzać nimi
//   2. Użyj okna programu Team Explorer, aby nawiązać połączenie z kontrolą źródła
//   3. Użyj okna Dane wyjściowe, aby sprawdzić dane wyjściowe kompilacji i inne komunikaty
//   4. Użyj okna Lista błędów, aby zobaczyć błędy
//   5. Wybierz pozycję Projekt > Dodaj nowy element, aby utworzyć nowe pliki kodu, lub wybierz pozycję Projekt > Dodaj istniejący element, aby dodać istniejące pliku kodu do projektu
//   6. Aby w przyszłości ponownie otworzyć ten projekt, przejdź do pozycji Plik > Otwórz > Projekt i wybierz plik sln
