#pragma once

/* Jim Viebke
December 16 2016 */

#ifdef WIN32
#define NOMINMAX // fix min() and max() errors
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#include <Windows.h>
#pragma comment (lib, "Ws2_32.lib")
#else
#include <sys/socket.h>
#include <netinet/ip.h>
#include <unistd.h>
#include <cstring>
const int INVALID_SOCKET = 0xffff; // Linux uses this value, but not by this name
#endif

namespace network
{
	enum socket_type
	{
		stream = SOCK_STREAM, // TCP
		dgram = SOCK_DGRAM, // UDP
	};
	using protocol = IPPROTO; // create an alias

	namespace detail
	{
		class WSA_wrapper
		{
		private:
			WSA_wrapper();

		public:
			static void startup()
			{
				static WSA_wrapper wrapper;
			}
			~WSA_wrapper();
		};
	}
}
