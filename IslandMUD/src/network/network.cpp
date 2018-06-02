
#include <exception>

#include "network.h"

namespace network
{
	namespace detail
	{
        
#ifdef WIN32
		WSA_wrapper::WSA_wrapper()
		{
			WSADATA WSAData;
			const int startup_result = WSAStartup(MAKEWORD(2, 2), &WSAData);

			if (startup_result != 0)
				throw std::exception("WSAStartup failed: " + startup_result);
		}

		WSA_wrapper::~WSA_wrapper()
		{
			WSACleanup();
		}
#endif // Linux doesn't need anything like this because it initializes its networking code automatically

    }

    void close_socket(const SOCKET & socket)
    {
#ifdef WIN32
        closesocket(socket);
#else
        close(socket);
#endif
    }

}
