
/* Jim Viebke
Sept 14 2016 */

#include "threadsafe_socket_lookup.h"

threadsafe::socket_lookup::socket_lookup() {}

// save the user's socket
void threadsafe::socket_lookup::set_socket(const std::string & user_ID, const SOCKET & socket)
{
	std::lock_guard<std::mutex> lock(mutex);
	sockets_map[user_ID].user_socket = socket;
}
// save the user's map socket
void threadsafe::socket_lookup::set_map_socket(const std::string & user_ID, const SOCKET & map_socket)
{
	std::lock_guard<std::mutex> lock(mutex);
	sockets_map[user_ID].map_socket = map_socket;
}

// retrieve a user's socket
SOCKET threadsafe::socket_lookup::get_socket(const std::string & user_ID) const
{
	std::lock_guard<std::mutex> lock(mutex);
	const auto it = sockets_map.find(user_ID);
	return (it != sockets_map.cend()) ? it->second.user_socket : -1;
}
// retrieve a user's map socket
SOCKET threadsafe::socket_lookup::get_map_socket(const std::string & user_ID) const
{
	std::lock_guard<std::mutex> lock(mutex);
	const auto it = sockets_map.find(user_ID);
	return (it != sockets_map.cend()) ? it->second.map_socket : -1;
}

// retrieve a user ID associated with a socket (reverse lookup)
std::string threadsafe::socket_lookup::get_user_ID(const SOCKET & socket) const
{
	std::lock_guard<std::mutex> lock(mutex);
	for (auto it = sockets_map.cbegin(); it != sockets_map.cend(); ++it)
	{
		if (it->second.user_socket == socket || it->second.map_socket == socket) return it->first;
	}
	return "";
}

// erase the records for a user
void threadsafe::socket_lookup::erase(const std::string & user_ID)
{
	std::lock_guard<std::mutex> lock(mutex);
	sockets_map.erase(user_ID);
}
