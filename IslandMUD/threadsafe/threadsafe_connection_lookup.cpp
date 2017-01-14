
/* Jim Viebke
Sept 14 2016 */

#include "threadsafe_connection_lookup.h"

namespace threadsafe
{
	connection_lookup::connection_lookup() {}

	// save the user's connection
	void connection_lookup::set_connection(const std::string & user_ID, const network::connection_ptr & connection)
	{
		std::lock_guard<std::mutex> lock(mutex);
		user_connections[user_ID].user_connection = connection;
	}
	// save the user's map connection
	void connection_lookup::set_map_connection(const std::string & user_ID, const network::connection_ptr & map_connection)
	{
		std::lock_guard<std::mutex> lock(mutex);
		user_connections[user_ID].map_connection = map_connection;
	}

	// retrieve a user's connection
	network::connection_ptr connection_lookup::get_connection(const std::string & user_ID) const
	{
		std::lock_guard<std::mutex> lock(mutex);
		const auto it = user_connections.find(user_ID);
		return (it != user_connections.cend()) ? it->second.user_connection : nullptr;
	}
	// retrieve a user's map connection
	network::connection_ptr connection_lookup::get_map_connection(const std::string & user_ID) const
	{
		std::lock_guard<std::mutex> lock(mutex);
		const auto it = user_connections.find(user_ID);
		return (it != user_connections.cend()) ? it->second.map_connection : nullptr;
	}

	// retrieve a user ID associated with a connection (reverse lookup)
	std::string connection_lookup::get_user_ID(const network::connection_ptr & connection) const
	{
		std::lock_guard<std::mutex> lock(mutex);
		for (const auto & it : user_connections)
		{
			if (it.second.user_connection == connection || it.second.map_connection == connection) return it.first;
		}
		return "";
	}

	// erase the records for a user
	void connection_lookup::erase(const std::string & user_ID)
	{
		std::lock_guard<std::mutex> lock(mutex);
		user_connections.erase(user_ID);
	}
}


