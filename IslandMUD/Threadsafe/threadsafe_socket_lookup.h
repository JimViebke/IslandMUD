
/* Jim Viebke
Nov 13, 2015 */

#ifndef THREADSAFE_SOCKET_LOOKUP_H
#define THREADSAFE_SOCKET_LOOKUP_H

#include <map>
#include <mutex>

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

namespace threadsafe
{
	class socket_lookup
	{
	private:
		class Sockets
		{
		public:
			SOCKET user_socket = -1;
			SOCKET map_socket = -1; // the overhead map
			// Sockets(const SOCKET & user, const SOCKET & map) : user_socket(user), map_socket(map) {}
		};

		std::map<std::string, Sockets> sockets_map; // map a username to two sockets
		mutable std::mutex mutex; // this mutex can be locked by constant member functions

	public:
		socket_lookup();

		// save the user's socket
		void set_socket(const std::string & user_ID, const SOCKET & socket);
		// save the user's map socket
		void set_map_socket(const std::string & user_ID, const SOCKET & map_socket);

		// retrieve a user's socket
		SOCKET get_socket(const std::string & user_ID) const;
		// retrieve a user's map socket
		SOCKET get_map_socket(const std::string & user_ID) const;

		// retrieve a user ID associated with a socket (reverse lookup)
		std::string get_user_ID(const SOCKET & socket) const;

		// erase the records for a user
		void erase(const std::string & user_ID);
	};
}

#endif
