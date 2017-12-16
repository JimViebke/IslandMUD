
/* Jim Viebke
December 16 2016 */

#include <exception>
#include <iostream>
#include <sstream>

#include <errno.h>

#include "port.h"

namespace network
{
	Port::Port(const unsigned & port, const socket_type & type, const protocol & protocol)
	{
#ifdef WIN32
		// ensure WSA has started (destroys automatically)
		detail::WSA_wrapper::startup();
#endif

		listening_socket = socket(AF_INET, type, protocol);

		if (listening_socket == INVALID_SOCKET)
#ifdef WIN32
			throw std::runtime_error("Failed to listen on port. Reason: " + WSAGetLastError());
#else
            throw std::runtime_error("Failed to listen on port. Reason: " + std::to_string(errno));
#endif

		// create port information
		sockaddr_in name;
		memset(&name, 0, sizeof(sockaddr_in));
		name.sin_family = AF_INET;
		name.sin_port = htons(port);
#ifdef WIN32
		name.sin_addr.S_un.S_addr = 0; // open port on all network interfaces
#else
        name.sin_addr.s_addr = 0;
#endif

		// bind port information to the port
		bind(listening_socket, reinterpret_cast<sockaddr*>(&name), sizeof(sockaddr_in));

		// listen for up to 3 clients
		int listen_result = listen(listening_socket, 3);
	}
	Port::~Port()
	{
		if (open) network::close_socket(listening_socket);
	}

	connection_ptr Port::get_connection() const
	{
		sockaddr_in client_information;
		memset(&client_information, 0, sizeof(sockaddr_in));

		// get the next connection
		const SOCKET client_ID = accept(listening_socket, (sockaddr*)&client_information, NULL);

		std::stringstream ss;
		ss << client_ID << " connected.\n";
        std::cout << ss.str();

		// return a shared pointer to a connection object
		return (client_ID != INVALID_SOCKET) ? std::make_shared<Connection>(client_ID) : nullptr;
	}

	void Port::close()
	{
		open = false;
		network::close_socket(listening_socket);
	}
}
