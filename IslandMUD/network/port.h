#pragma once

#include "network.h"
#include "connection.h"

namespace network
{
	class Port;

	using port_ptr = std::shared_ptr<Port>;

	class Port
	{
#ifdef WIN32
		friend class std::shared_ptr<Port>;
		friend class std::_Ref_count_obj<Port>;
#else
        friend class __gnu_cxx::new_allocator<Port>;

#endif

	private:
		SOCKET listening_socket;
		bool open = true;
		Port(const unsigned & port, const socket_type & type, const protocol & protol);

		// delete copy-constructors
		Port(const Port &) = delete;
		Port & operator=(const Port &) = delete;

	public:
		static port_ptr open_port(const unsigned & port)
		{
			return open_port(port, socket_type::stream, protocol::IPPROTO_TCP);
		}
		static port_ptr open_port(const unsigned & port, const socket_type & type, const protocol & protocol)
		{
			return std::make_shared<Port>(port, type, protocol);
		}

		~Port();

		connection_ptr get_connection() const;
		void close();
	};

}
