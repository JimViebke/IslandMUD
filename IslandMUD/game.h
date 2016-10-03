/* Jim Viebke
Feb 14, 2015 */

#ifndef GAME_H
#define GAME_H

#include <map>
#include <queue>
#include <thread>
#include <mutex>

#include "Network\socket.h"
#include "constants.h"
#include "character.h"
#include "npc.h"
#include "parse.h"
#include "world.h"
#include "Threadsafe\threadsafe_queue.h"
#include "Threadsafe\threadsafe_socket_lookup.h"
#include "message.h"

class Game
{
private:
	threadsafe::queue<Message> inbound_queue; // <socket_ID, message>
	threadsafe::queue<Message> outbound_queue; // <socket_ID, message>

	threadsafe::socket_lookup clients;

	std::map<std::string, std::shared_ptr<Character>> actors; // active/online PC and NPC ids
	World world; // the game world object

	std::mutex game_state; // provides safe access and modification of the above two structures

public:
	
	typedef void(Game::*client_thread_type)(SOCKET);

	Game();

	// execute a command against the game world
	Update_Messages execute_command(const std::string & actor_id, const std::vector<std::string> & command);

	// process data, moving it from the input queue to the output queue
	void processing_thread();

private:

	// typedef void(Game::*client_thread_type)(SOCKET);

	// Listen on port [listening_port].
	// When a user connects, start a thread using [client_thread_pointer], passing in the user's unique socket ID
	void networking_thread(const unsigned & listening_port, client_thread_type client_thread_pointer);

	// capture incoming data and write it to the input queue
	void client_thread(SOCKET client_ID);

	// send map updates to the user's second client instance
	void client_map_thread(SOCKET client_ID);

	// handle everything to do with NPCs
	void NPC_thread();
	void NPC_spawn_logic();
	void NPC_update_logic();

	// remove data from the outbound queue and send it the to specified client
	void outbound_thread();

	// helper functions

	// close a socket, platform independent
	void close_socket(SOCKET socket);

	// return the user_ID of a user after they log in or sign up
	std::string login_or_signup(SOCKET client_ID);

	// use an Update_Messages object to generate outbound messages to players
	void generate_outbound_messages(const std::string & user_ID, const Update_Messages & message_updates);
};

#endif
