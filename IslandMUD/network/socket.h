#pragma once

/* Jim Viebke
Sept 9 2016

Provide a wrapper for socket functions. */

#include <iostream>
#include <thread>

#ifdef WIN32
#define NOMINMAX // fix min() and max() errors
#include <WinSock2.h>
#include <Windows.h>
#pragma comment (lib, "Ws2_32.lib")
#else
#include <sys/socket.h>
#include <netinet/ip.h>
#include <unistd.h>
#include <cstring>
#endif

class Socket
{
public:

	// this never exits - it listens forever
	template <typename T_instance, typename T_function> static void listen(const unsigned & port, T_instance & call_back_instance, T_function & call_back)
	{
		std::cout << "Starting a listening thread for port " << port << "...\n";

#ifdef WIN32
		WSADATA lpWSAData;
		WSAStartup(MAKEWORD(2, 2), &lpWSAData);
#endif

		const SOCKET socket_1 = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

		if (socket_1 == INVALID_SOCKET)
		{
#ifdef WIN32
			std::cout << "invalid socket, error code: " << WSAGetLastError() << std::endl;
#else
			std::cout << "invalid socket, error code: " << errno << std::endl;
#endif
		}

		sockaddr_in name;
		memset(&name, 0, sizeof(sockaddr_in));
		name.sin_family = AF_INET;
		name.sin_port = htons(port);
#ifdef WIN32
		name.sin_addr.S_un.S_addr = 0; // open port on all network interfaces
#else
		name.sin_addr.s_addr = 0;
#endif

		// bind the socket
		bind(socket_1, (sockaddr*)&name, sizeof(sockaddr_in));

		// open the port for clients to connect, maintaining a backlog of up to 3 waiting connections
		int listen_result = ::listen(socket_1, 3); // non-blocking

		// create a holder for incoming client information
		sockaddr_in client_address;
		memset(&client_address, 0, sizeof(sockaddr_in));

		std::cout << "Listening on port " << port << ".\n";

		for (;;)
		{
			// execution pauses inside of accept() until an incoming connection is received
			SOCKET client = accept(socket_1, (sockaddr*)&client_address, NULL); // blocking

			// start a thread to receive messages from this client
			std::thread(call_back, call_back_instance, client).detach(); // detach() because the thread is responsible for destroying itself later
		}

#ifdef WIN32
		WSACleanup();
#endif
	}

};
