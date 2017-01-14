
#include <exception>

#include "network.h"

namespace network
{
	namespace detail
	{
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
	}
}
