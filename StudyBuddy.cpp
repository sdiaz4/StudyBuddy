#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include "StudyBuddy.h"


#pragma comment (lib, "Ws2_32.lib")

using std::cout;
using std::endl;
using std::cin;
using std::string;
using std::vector;

string getLine(string prompt) {
	string input;
	cout << prompt;
	getline(cin, input);
	return input;
}

void startHost(string username) {
	//opens a socket
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		cout << "WSAStartup failed\n";
		
	}

	SOCKET serverSocket = establishServerSocket();
	if (serverSocket == SOCKET_ERROR) {
		return;
	}
	string location = getLine("Enter your location: \n");
	string courses = getLine("Enter your courses: \n");

	vector<string> members;
	members.push_back(username);

	while (true) {
		char buffer[DEFAULT_BUFLEN];
		sockaddr_in clientAddr;

		int clientAddrSize = sizeof(clientAddr);

		int recvResult = recvfrom(serverSocket, buffer, DEFAULT_BUFLEN, 0, (sockaddr*)&clientAddr, &clientAddrSize);
		if (recvResult == SOCKET_ERROR) {
			cout << "recvfrom failed with error: " << WSAGetLastError() << endl;
			continue;
		}
		buffer[recvResult] = '\0'; 
		cout << "Received message: " << buffer << endl;
		
		if (_stricmp(buffer, Study_QUERY) == 0) {
			string response = Study_NAME + username;
			sendto(serverSocket, response.c_str(), response.size() + 1, 0, (sockaddr*)&clientAddr, clientAddrSize);
		}
		else if (_stricmp(buffer, Study_WHERE) == 0) {
			string response = Study_LOC + location;
			sendto(serverSocket, response.c_str(), response.size() + 1, 0, (sockaddr*)&clientAddr, clientAddrSize);
		}
		else if (_stricmp(buffer, Study_WHAT) == 0) {
			string response = Study_COURSES + courses;
			sendto(serverSocket, response.c_str(), response.size() + 1, 0, (sockaddr*)&clientAddr, clientAddrSize);
		}
		else if (_stricmp(buffer, Study_MEMBERS) == 0) {
			string response = Study_MEMLIST;
			for (const string& member : members) {
				response += member + ",";
			}
			if (!members.empty()) {
				response.pop_back(); 
			}
			sendto(serverSocket, response.c_str(), response.size() + 1, 0, (sockaddr*)&clientAddr, clientAddrSize);
		}
		else if (strncmp(buffer, Study_JOIN, strlen(Study_JOIN)) == 0) {
			string memberName = buffer + strlen(Study_JOIN);
			members.push_back(memberName);
			string response = Study_CONFIRM;
			sendto(serverSocket, response.c_str(), response.size() + 1, 0, (sockaddr*)&clientAddr, clientAddrSize);
		}

		


	}

}
void findServers();

SOCKET establishServerSocket() {
	SOCKET StudySocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
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

	WSACleanup();
}


