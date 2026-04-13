#include <iostream>
#include <string>
#include <vector>
#include <map>
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


string getLine(string prompt) {
	string input;
	cout << prompt;
	getline(cin, input);
	return input;
}

void startHost(string username) {

	//opens the socket network
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		cout << "WSAStartup failed\n";
	}
	// binds to a port with helper established in .h file
	SOCKET serverSocket = establishServerSocket();
	if (serverSocket == SOCKET_ERROR) {
		cout << "Failed to establish server socket." << endl;
		return;
	}

	cout << "Server started successfully. Waiting for clients to join..." << endl;

	//asks for user input for location and courses
	string location = getLine("Enter your location: \n");
	string courses = getLine("Enter your courses: \n");

	// creates and adds user to the list of members in the study group
	vector<string> members;
	members.push_back(username);


	// waits for messages from clients and responds to accordingly
	while (true) {
		// buffers and variables to receive messages from clients
		char buffer[DEFAULT_BUFLEN];
		sockaddr_in clientAddr;

		// size of the client address structure, needed for recvfrom
		int clientAddrSize = sizeof(clientAddr);

		// receives message from client and checks for errors
		int recvResult = recvfrom(serverSocket, buffer, DEFAULT_BUFLEN, 0, (sockaddr*)&clientAddr, &clientAddrSize);
		if (recvResult == SOCKET_ERROR) {
			cout << "recvfrom failed with error: " << WSAGetLastError() << endl;
			continue;
		}

		// null-terminate the received message and outputs message to console
		buffer[recvResult] = '\0'; 
		cout << "Received message: " << buffer << endl;

		// checks the received message and responds accordingly
		if (_stricmp(buffer, Study_QUERY) == 0) {
			string response = Study_NAME + username;
			sendto(serverSocket, response.c_str(), response.size() + 1, 0, (sockaddr*)&clientAddr, clientAddrSize);
			cout << "Sending response: " << response << endl;
		}
		else if (_stricmp(buffer, Study_WHERE) == 0) {
			string response = Study_LOC + location;
			sendto(serverSocket, response.c_str(), response.size() + 1, 0, (sockaddr*)&clientAddr, clientAddrSize);
			cout << "Sending response: " << response << endl;
		}
		else if (_stricmp(buffer, Study_WHAT) == 0) {
			string response = Study_COURSES + courses;
			sendto(serverSocket, response.c_str(), response.size() + 1, 0, (sockaddr*)&clientAddr, clientAddrSize);
			cout << "Sending response: " << response << endl;
		}
		else if (_stricmp(buffer, Study_MEMBERS) == 0) {
			string response = Study_MEMLIST;
			for (const string& member : members) {
				response += member + "\n";
			}
			if (!members.empty()) {
				response.pop_back(); 
			}
			sendto(serverSocket, response.c_str(), response.size() + 1, 0, (sockaddr*)&clientAddr, clientAddrSize);
			cout << "Sending response: " << response << endl;
		}
		else if (strncmp(buffer, Study_JOIN, strlen(Study_JOIN)) == 0) {
			string memberName = buffer + strlen(Study_JOIN);
			members.push_back(memberName);
			string response = Study_CONFIRM;
			sendto(serverSocket, response.c_str(), response.size() + 1, 0, (sockaddr*)&clientAddr, clientAddrSize);
			cout << "Sending response: " << response << endl;
		}

	}

}
SOCKET findServers(string name) {


	//opens the socket network
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		cout << "WSAStartup failed\n";
		return SOCKET_ERROR;
	}
	
	// creates a socket to send and receive messages
	SOCKET StudySocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (StudySocket == INVALID_SOCKET) {
		cout << "Error creating socket: " << WSAGetLastError() << endl;
		WSACleanup();
		return SOCKET_ERROR;
	}

	// buffer to hold the list of servers found
	ServerStruct servers[MAX_SERVERS];

	// calls helper function to get the list of servers and the number of servers found
	int serverNum = getServers(StudySocket, servers);

	// outputs the list of servers found to the console
	cout << "Available servers:" << endl;
	if (serverNum == 0) {
		cout << "No servers found." << endl;
		return SOCKET_ERROR;
	}
	else {
		 cout << "Found " << serverNum << " server(s)." << endl;
		 for (int i = 0; i < serverNum; i++) {
			 if (servers[i].name[0] != '\0') {
				 cout << i + 1 << ". " << servers[i].name << endl;
			 }
		 }
	}

	// prompts user to select a server to join and checks for valid input
	int choiceNum = stoi(getLine("Which group would you like to join? \n"));
	if (choiceNum < 1 || choiceNum > serverNum) {
		cout << "Invalid choice." << endl;
		return SOCKET_ERROR;
	}

	// gets the address of the selected server to send messages to
	sockaddr_in serverAddr = servers[choiceNum - 1].addr;

	// main loop to send messages to the server and receive responses
	while (true) {

		cout << "Menu:\n" <<
			"1. Where?\n" <<
			"2. What?\n" <<
			"3. Members?\n" <<
			"4. Join group\n" <<
			"5. Exit\n";

		int action = stoi(getLine("Select an option: \n"));

		string message;

		if (action == 1) {
			message = Study_WHERE;
		}
		else if (action == 2) {
			message = Study_WHAT;
		}
		else if (action == 3) {
			message = Study_MEMBERS;
		}
		else if (action == 4) {
			message = Study_JOIN + name;
		}
		else {
			cout << "exiting...";
			break;
		}
			

		// sends the selected message to the server
		sendto(StudySocket, message.c_str(), message.size() + 1, 0, (sockaddr*)&serverAddr, sizeof(serverAddr));
		cout << "Sent message: " << message << endl;


		// buffer and variables to receive response from server
		char buffer[DEFAULT_BUFLEN];
		sockaddr_in fromAddr;
		int fromAddrSize = sizeof(fromAddr);

		// receives response from server and checks for errors
		int len = recvfrom(StudySocket, buffer, DEFAULT_BUFLEN, 0, (sockaddr*)&fromAddr, &fromAddrSize);
		if (len == SOCKET_ERROR) {
			cout << "recvfrom failed with error: " << WSAGetLastError() << endl;
			continue;
		}
		
		if (len > 0) {
			buffer[len] = '\0';
			cout << "Received response: " << buffer << endl;
		}
		
		if (action == 4) {
			cout << "Joined group. Exiting" << endl;
		}
		
	}
	// closes the socket and cleans up Winsock
	closesocket(StudySocket);
	WSACleanup();
	return StudySocket;
}



int main() {
	string name = getLine("What is your name?\n");
	int action = std::stoi(getLine("Host(1) or Join(2)\n"));

	while (action != 1 && action != 2) {
		action = std::stoi(getLine("Invalid input. Please enter either 1 to host or 2 to join."));
	}

	if (action == 1) {
		startHost(name);
	}
	else {
		findServers(name);
	}

	
}


