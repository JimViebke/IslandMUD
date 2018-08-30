#pragma once

/* Jim Viebke
Nov 13, 2015 */

#include <map>
#include <mutex>

#include "threadsafe/threadsafe_map.hpp"

#include "network/include.h"

#include "types.hpp"

class connection_lookup
{
private:
	class Connections
	{
	public:
		network::connection_ptr user_connection;
		network::connection_ptr map_connection; // the overhead map
	};

	threadsafe::map<character_id, Connections> connections; // map a username to two connections

public:
	connection_lookup();

	// save the user's connection
	void set_connection(const character_id & user_ID, const network::connection_ptr & connection);
	// save the user's map connection
	void set_map_connection(const character_id & user_ID, const network::connection_ptr & connection);

	// retrieve a user's connection
	network::connection_ptr get_connection(const character_id & user_ID) const;
	// retrieve a user's map connection
	network::connection_ptr get_map_connection(const character_id & user_ID) const;

	// retrieve a user ID associated with a connection (reverse lookup)
	character_id get_user_ID(const network::connection_ptr & connection) const;

	// erase the records for a user
	void erase(const character_id & user_ID);
};
