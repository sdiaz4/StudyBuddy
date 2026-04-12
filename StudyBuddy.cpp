#include <iostream>
#include <string>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include "StudyBuddy.h"

#pragma comment (lib, "Ws2_32.lib")

using std::cout;
using std::endl;
using std::cin;
using std::string;

string getLine(string prompt) {
	string input;
	cout << prompt;
	getline(cin, input);
	return input;
}

void startHost();
void findServers();

SOCKET establishServerSocket() {
	SOCKET StudySocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_TCP);
	if (StudySocket == INVALID_SOCKET) {
		cout << "Error creating socket: " << WSAGetLastError() << endl;
		WSACleanup();
		return SOCKET_ERROR;
	}


	struct sockaddr_in myAddr;
	myAddr.sin_family = AF_INET;
	myAddr.sin_port = htons(DEFAULT_PORT);
	myAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	int iResult = bind(StudySocket, (sockaddr*)&myAddr, sizeof(myAddr));
	if (iResult == SOCKET_ERROR) {
		cout << "bind failed with error: " << WSAGetLastError() << endl;
		closesocket(StudySocket);
		WSACleanup();
		return SOCKET_ERROR;
	}
	return StudySocket;
}


int main() {
	string name = getLine("What is your name?\n");
	int action = std::stoi(getLine("Host(1) or Join(2)\n"));

	while (action != 1 && action != 2) {
		action = std::stoi(getLine("Invalid input. Please enter either 1 to host or 2 to join."));
	}

	if (action == 1) {
		cout << "you chose to host" << endl;
	}
	else {
		cout << "you chose to join" << endl;
	}
	

	
}


