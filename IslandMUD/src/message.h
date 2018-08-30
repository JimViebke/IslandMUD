
/* Jim Viebke
Nov 14 2015 */

#ifndef MESSAGE_H
#define MESSAGE_H

#include <string>
#include <memory>

#include "network/include.h"

#include "types.hpp"
#include "utilities.h"

class Message
{
public:
	network::connection_ptr connection;
	std::string data;
	Message(const network::connection_ptr & connection, const std::string & data);
};

class Update_Messages
{
public:
	std::string to_user; // an update is always sent to the user
	std::shared_ptr<std::string> to_room; // an update may be sent to all players in the room
	std::shared_ptr<std::string> to_area; // an update may be sent to all players in the area
	std::shared_ptr<std::vector<character_id>> additional_map_update_users; // users that require a map update AND are not in sight range of the player
	bool map_update_required; // if true, all users in sight range of the player will receive a map update
	std::shared_ptr<std::pair<character_id, std::string>> custom_message; // stores <user_ID, message>

	explicit Update_Messages(const std::string & user_message, const bool set_map_update = false) : to_user(user_message), map_update_required(set_map_update) {}
	explicit Update_Messages(const std::string & user_message, const std::string & room_message, const bool set_map_update = false) : Update_Messages(user_message, set_map_update)
	{
		to_room = std::make_shared<std::string>(room_message);
	}
	explicit Update_Messages(const std::string & user_message, const std::string & room_message, const std::pair<character_id, std::string> & set_custom_message) : Update_Messages(user_message, room_message)
	{
		custom_message = std::make_shared<std::pair<character_id, std::string>>(set_custom_message);
	}
	explicit Update_Messages(const std::string & user_message, const std::string & room_message, const std::string & area_message, const bool set_map_update = false) : Update_Messages(user_message, room_message, set_map_update)
	{
		to_area = std::make_shared<std::string>(area_message);
	}
};

#endif
