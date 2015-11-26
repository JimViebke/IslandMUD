
/* Jim Viebke
Nov 14, 2015 */

#include "message.h"

Message::Message(const SOCKET & set_user_socket_ID, const std::string & set_data) :
user_socket_ID(set_user_socket_ID), data(set_data)
{

}
