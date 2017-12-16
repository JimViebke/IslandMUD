
/* Jim Viebke
Sept 14 2016 */

#include "connection_lookup.h"

connection_lookup::connection_lookup() {}

// save the user's connection
void connection_lookup::set_connection(const std::string & user_ID, const network::connection_ptr & connection)
{
	connections.get(user_ID, connections.get_write_lock()).user_connection = connection;
}
// save the user's map connection
void connection_lookup::set_map_connection(const std::string & user_ID, const network::connection_ptr & map_connection)
{
	connections.get(user_ID, connections.get_write_lock()).map_connection = map_connection;
}

// retrieve a user's connection
network::connection_ptr connection_lookup::get_connection(const std::string & user_ID) const
{
	return connections.get(user_ID, connections.get_read_lock()).user_connection;
}
// retrieve a user's map connection
network::connection_ptr connection_lookup::get_map_connection(const std::string & user_ID) const
{
	return connections.get(user_ID, connections.get_read_lock()).map_connection;
}

// retrieve a user ID associated with a connection (reverse lookup)
std::string connection_lookup::get_user_ID(const network::connection_ptr & connection) const
{
	for (auto & it = connections.cbegin(); it != connections.cend(); ++it)
	{
		if (it->second.user_connection == connection || it->second.map_connection == connection) return it->first;
	}
	return "";
}

// erase the records for a user
void connection_lookup::erase(const std::string & user_ID)
{
	connections.erase(user_ID, connections.get_write_lock());
}
