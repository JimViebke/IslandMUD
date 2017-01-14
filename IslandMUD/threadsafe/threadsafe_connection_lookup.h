#pragma once

/* Jim Viebke
Nov 13, 2015 */

#include <map>
#include <mutex>

#include "../network/include.h"

namespace threadsafe
{
	class connection_lookup
	{
	private:
		class Connections
		{
		public:
			network::connection_ptr user_connection;
			network::connection_ptr map_connection; // the overhead map
		};

		std::map<std::string, Connections> user_connections; // map a username to two connections
		mutable std::mutex mutex; // this mutex can be locked by constant member functions

	public:
		connection_lookup();

		// save the user's connection
		void set_connection(const std::string & user_ID, const network::connection_ptr & connection);
		// save the user's map connection
		void set_map_connection(const std::string & user_ID, const network::connection_ptr & connection);

		// retrieve a user's connection
		network::connection_ptr get_connection(const std::string & user_ID) const;
		// retrieve a user's map connection
		network::connection_ptr get_map_connection(const std::string & user_ID) const;

		// retrieve a user ID associated with a connection (reverse lookup)
		std::string get_user_ID(const network::connection_ptr & connection) const;

		// erase the records for a user
		void erase(const std::string & user_ID);
	};
}
