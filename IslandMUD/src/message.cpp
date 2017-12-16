
/* Jim Viebke
Nov 14, 2015 */

#include "message.h"

Message::Message(const network::connection_ptr & connection, const std::string & data) :
	connection(connection), data(data)
{

}
