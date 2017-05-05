#pragma once

/* Jim Viebke
December 13 2016 */

#include <string>
#include <memory>

#include "network.h"

namespace network
{
	using connection_id = SOCKET; // rename

	class Connection; // forward declaring
	using connection_ptr = std::shared_ptr<Connection>;

	// socket wrapper
	class Connection
	{
		friend class Port;
#ifdef WIN32
		friend class std::shared_ptr<Connection>;
		friend class std::_Ref_count_obj<Connection>;
#else
        friend class __gnu_cxx::new_allocator<Connection>;

#endif

	private:
		SOCKET _socket;
		bool open = true;

		Connection(const SOCKET & set_socket);

		// delete copy-constructors
		Connection(const Connection &) = delete;
		Connection & operator=(const Connection &) = delete;

	public:
		connection_id get_id() const { return _socket; } // this method should be able to be removed - it is only used below

		void send(const std::string & message) const;
		std::string read() const;
		void close();

		Connection(const std::string & ip_address, const unsigned & port);
		Connection(const std::string & ip_address, const unsigned & port, const socket_type & type, const protocol & protocol);

		~Connection();
	};

}

inline bool operator==(const network::connection_ptr & a, const network::connection_ptr & b)
{
	// this is a faster way of evaluating "if (a is null or b is null) return a is null and b is null)"
	if (a == nullptr) return b == nullptr;
	if (b == nullptr) return false;

	return a->get_id() == b->get_id();
}
