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
	ifstream data;
	WSADATA wsaData;

	openInputFile(data, FILENAME);
	int currVer = readInt(data);
	int num1 = readInt(data);
	int num2 = readInt(data);
	data.close();


	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != NO_ERROR)
	{
		printf("Windows DLL error.\n");
	}

	printf("Starting server...\n");

	if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		printf("Could not create socket: %d\n", serverSocket);
		return 1;
	}


	address.sin_family = AF_INET;
	address.sin_port = htons(PORT);
	inet_pton(AF_INET, "127.0.0.1", &address.sin_addr);

	if (bind(serverSocket, (struct sockaddr *)&address, sizeof(address)) == SOCKET_ERROR)
	{
		printf("Could not bind!\n");
		closesocket(serverSocket);
		return 1;
	}


	if (listen(serverSocket, 1) == SOCKET_ERROR)
	{
		printf("Could not listen on socket: %d\n", serverSocket);
		closesocket(serverSocket);
		return 1;
	}

	printf("Server started...\nWaiting for connection...\n");

	while (true)
	{
		if ((newSocket = accept(serverSocket, (struct sockaddr *)&address, (socklen_t*)&addrlen)) == INVALID_SOCKET)
		{
			printf("Could not create connection!");
		}
		else
		{
			counter++;

			if (counter == 5)
			{
				openInputFile(data, FILENAME);
				currVer = readInt(data);
				data.close();
				counter = 1;
			}

			printf("Waiting to recieve version...\n");
			recv(newSocket, (char*)&buffer, sizeof(buffer), 0);
			
			if (buffer == 1)
			{
				printf("Sending current version: %d\n\n", currVer);
				send(newSocket, (char*)&currVer, sizeof(currVer), 0);
				closesocket(newSocket);
			}
			else if (buffer == 2)
			{
				printf("Sending update file...\n\n");
				send(newSocket, (char*)&currVer, sizeof(currVer), 0);
				send(newSocket,(char*)&num1 , sizeof(num1), 0);
				send(newSocket, (char*)&num2, sizeof(num2), 0);
				closesocket(newSocket);
			}
		}
	}
	return 0;
}