
/* Jim Viebke
Nov 14 2015 */

#ifndef MESSAGE_H
#define MESSAGE_H

#include <string>
#include <memory>

#ifdef WIN32
#include <WinSock2.h>
#include <Windows.h>
#pragma comment (lib, "Ws2_32.lib")
#else
#include <sys/socket.h>
#include <netinet/ip.h>
#include <unistd.h>
#include <cstring>
#endif

#include "utilities.h"

class Message
{
public:
	std::string data;
	SOCKET user_socket_ID;
	Message(const SOCKET & set_user_socket_ID, const std::string & set_data);
};

class Update_Messages
{
public:
	std::string to_user; // an update is always sent to the user
	std::unique_ptr<std::string> to_room; // an update may be sent to all players in the room
	std::unique_ptr<std::string> to_area; // an update may be sent to all players in the area
	bool map_update_required;

	explicit Update_Messages(const std::string & user_message, const bool set_map_update = false) : to_user(user_message), map_update_required(set_map_update) {}
	explicit Update_Messages(const std::string & user_message, const std::string & room_message, const bool set_map_update = false) : to_user(user_message), map_update_required(set_map_update)
	{
		to_room = U::make_unique<std::string>(room_message);
	}
	explicit Update_Messages(const std::string & user_message, const std::string & room_message, const std::string & area_message, const bool set_map_update = false) : to_user(user_message), map_update_required(set_map_update)
	{
		to_room = U::make_unique<std::string>(room_message);
		to_area = U::make_unique<std::string>(area_message);
	}
};

#endif
