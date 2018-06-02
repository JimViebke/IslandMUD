
#include <string>
#include <sstream>

#include <errno.h>

#include "connection.h"

namespace network
{
	Connection::Connection(const std::string & ip_address, const unsigned & port) : Connection(ip_address, port, socket_type::stream, protocol::IPPROTO_TCP) {}
	Connection::Connection(const std::string & ip_address, const unsigned & port, const socket_type & type, const protocol & protocol)
	{
#ifdef WIN32
		// ensure WSA has started (destroys automatically)
		detail::WSA_wrapper::startup();
#endif
        
		// create the socket
		_socket = socket(AF_INET, type, protocol);

		// create connection information
		sockaddr_in serverAddress = { 0 };
		serverAddress.sin_family = AF_INET;
		serverAddress.sin_port = htons(port);
		serverAddress.sin_addr.s_addr = inet_addr(ip_address.c_str());

		// connect to the socket
		const int res = connect(_socket, reinterpret_cast<sockaddr*>(&serverAddress), sizeof(sockaddr_in));

		// if connection failed
		if (res == SOCKET_ERROR)
		{
			network::close_socket(_socket);
#ifdef WIN32
			throw std::runtime_error("Error on socket connect: " + WSAGetLastError());
#else
            throw std::runtime_error("Error on socket connect: " + std::to_string(errno));
#endif
		}
	}

	// private constructor
	Connection::Connection(const SOCKET & set_socket) : _socket(set_socket)
	{
#ifdef WIN32
		// ensure WSA has started (destroys automatically)
		detail::WSA_wrapper::startup();
#endif
	}

	void Connection::send(const std::string & message) const
	{
		::send(_socket, message.c_str(), (int)message.size(), 0);
	}

	std::string Connection::read() const
	{
		// read data in 1KB chunks
		char input[1024];

		for (;;)
		{
			// block until data is sent, or the connection closes or fails
			int data_read = recv(_socket, input, 1024, 0);

			// check for a graceful disconnect OR a less graceful disconnect, respectively
			if (data_read == 0 || data_read == -1)
				throw std::runtime_error("disconnected - socket returned " + data_read);

			// read received data into a stringstream
			std::stringstream ss;
			for (int i = 0; i < data_read; ++i)
				ss << input[i];

			// return as a string
			return ss.str();
		}
	}

	void Connection::close()
	{
		network::close_socket(_socket);
		open = false;
	}

	Connection::~Connection()
	{
		if (open) network::close_socket(_socket);
	}

}
