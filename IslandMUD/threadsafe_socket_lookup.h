
/* Jim Viebke
Nov 13, 2015 */

#ifndef THREADSAFE_SOCKET_LOOKUP_H
#define THREADSAFE_SOCKET_LOOKUP_H

#include <map>
#include <mutex>

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

		std::map<std::string, Sockets> sockets_map;
		mutable std::mutex mutex; // this mutex can be locked by constant member functions

	public:
		socket_lookup() {}

		void set_socket(const std::string & user_ID, const SOCKET & socket)
		{
			std::lock_guard<std::mutex> lock(mutex);
			sockets_map[user_ID].user_socket = socket;
		}
		void set_map_socket(const std::string & user_ID, const SOCKET & map_socket)
		{
			std::lock_guard<std::mutex> lock(mutex);
			sockets_map[user_ID].map_socket = map_socket;
		}

		SOCKET get_socket(const std::string & user_ID) const
		{
			std::lock_guard<std::mutex> lock(mutex);
			auto it = sockets_map.find(user_ID);
			return (it != sockets_map.cend()) ? it->second.user_socket : -1;
		}
		SOCKET get_map_socket(const std::string & user_ID) const
		{
			std::lock_guard<std::mutex> lock(mutex);
			auto it = sockets_map.find(user_ID);
			return (it != sockets_map.cend()) ? it->second.map_socket : -1;
		}

		std::string get_user_ID(const SOCKET & socket) const
		{
			std::lock_guard<std::mutex> lock(mutex);
			for (auto it = sockets_map.cbegin(); it != sockets_map.cend(); ++it)
			{
				if (it->second.user_socket == socket || it->second.map_socket == socket) return it->first;
			}
			return "";
		}

		void erase(const std::string & user_ID)
		{
			std::lock_guard<std::mutex> lock(mutex);
			sockets_map.erase(user_ID);
		}
	};
}

#endif
