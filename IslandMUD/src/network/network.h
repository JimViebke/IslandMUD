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
    #include <arpa/inet.h>
	#include <unistd.h>
	#include <cstring>
	#include <netinet/in.h>

    // Windows and Linux use the same values by different names
    using SOCKET = int;
    const int SOCKET_ERROR = 1;
	const int INVALID_SOCKET = 0xffff;
#endif

namespace network
{
	enum socket_type
	{
		stream = SOCK_STREAM, // TCP
		dgram = SOCK_DGRAM, // UDP
	};

#ifdef WIN32
	using protocol = IPPROTO; // create an alias
#else
	enum protocol // not using "class" because the C-API needs these to be mapped back to the raw types
	{
		IPPROTO_IP = IPPROTO_IP,
		IPPROTO_TCP = IPPROTO_TCP
	};
#endif

	namespace detail
	{

#ifdef WIN32
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
#endif // Linux doesn't need anything like this because it initializes its networking code automatically

	}

    void close_socket(const SOCKET & socket);
}
