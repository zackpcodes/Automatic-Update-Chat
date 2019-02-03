// Author's: Zackary Pulaski, Ivan Carlson, Mitchell Kobelinski, Isaac Shepherd.
//Server.cpp handles requests for update files from client's
//


#include <WinSock2.h>
#include <Ws2tcpip.h>
#include <iostream>
#include "FileHelper.h"
using namespace std;

#pragma comment(lib, "Ws2_32.lib")

int main(int argc, char const *argv[])
{
	SOCKADDR_IN address;
	int addrlen = sizeof(address);
	int serverSocket, newSocket;
	int buffer = 0;
	const int PORT = 50000;
	const char FILENAME[] = "data.bin";
	int counter = 0;
	const int QUERY = 1;
	const int REQUEST = 2;
	ifstream data;
	WSADATA wsaData;

	int sendBuffer[3];
	
	//sendBuffer[0] Is the current version in data file.
	//sendBuffer[1] and sendBuffer[2] Contain the Data to be sent to the client.
	openInputFile(data, FILENAME);
	sendBuffer[0] = readInt(data);
	sendBuffer[1] = readInt(data);
	sendBuffer[2] = readInt(data);
	data.close();


	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != NO_ERROR)
	{
		printf("Windows DLL error.\n");
	}

	//Creates socket and stores information about socket in
	//serverSocket.
	if ((serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
	{
		printf("Could not create socket: %d\n", serverSocket);
		WSACleanup();
		return 1;
	}
	
	//
	address.sin_family = AF_INET;
	address.sin_port = htons(PORT);
	inet_pton(AF_INET, "127.0.0.1", &address.sin_addr);

	//Attaching serverSocket to PORT forcefully.
	if (bind(serverSocket, (struct sockaddr *)&address, sizeof(address)) == SOCKET_ERROR)
	{
		printf("Could not bind!\n");
		closesocket(serverSocket);
		WSACleanup();
		return 1;
	}

	//Begins listening on PORT for new connections.
	if (listen(serverSocket, 1) == SOCKET_ERROR)
	{
		printf("Could not listen on socket: %d\n", serverSocket);
		closesocket(serverSocket);
		WSACleanup();
		return 1;
	}

	printf("Update server\n");
	printf("Current data file version: v%d\n", sendBuffer[0]);
	printf("Running on port number: %d\n\n", PORT);
	
	//Continually waits for a conection. When a connection is
	//receieved, the loop sends information about version number,
	//and if requested sends a updated FILENAME.
	while (true)
	{
		printf("Waiting for connections...\n");
		if ((newSocket = accept(serverSocket, (struct sockaddr *)&address, (socklen_t*)&addrlen)) == INVALID_SOCKET)
		{
			printf("Could not create connection!\n");
		}
		else
		{
			//Checks FILENAME for updated version of file
			//and stores in sendBuffer.
			counter++;
			if (counter%5 == 0)
			{
				openInputFile(data, FILENAME);
				sendBuffer[0] = readInt(data);
				sendBuffer[1] = readInt(data);
				sendBuffer[2] = readInt(data);
				data.close();
			}

			printf("Connection received.\n");
			recv(newSocket, (char*)&buffer, sizeof(buffer), 0);
			
			//If buffer equals 1, sends current version number.
			//If buffer equals 2, sends update FILENAME to client.
			//If buffer equals anything other than 1 or 2 then socket is closed
			//and server returns to listening for new connections.
			if (buffer == QUERY)
			{
				printf("\tRequest for current version number: v%d\n", sendBuffer[0]);
				send(newSocket, (char*)&sendBuffer[0], sizeof(sendBuffer[0]), 0);
				closesocket(newSocket);
				printf("\tConnection closed.\n");
				printf("Total requests handled: %d\n\n", counter);
			}
			else if (buffer == REQUEST)
			{
				printf("\tRequest for update: v%d\n", sendBuffer[0]);
				send(newSocket, (char*)&sendBuffer, sizeof(sendBuffer), 0);			
				closesocket(newSocket);
				printf("\tConnection closed.\n");
				printf("Total requests handled: %d\n\n", counter);
			}
			else
			{
				closesocket(newSocket);
			}
		}
	}
	WSACleanup();
	return 0;
}