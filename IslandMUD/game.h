/* Jim Viebke
Feb 14, 2015 */

#ifndef GAME_H
#define GAME_H

#include <map>
#include <queue>
#include <thread>
#include <mutex>

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
#include "constants.h"
#include "character.h"
#include "player_character.h"
#include "non_player_character.h"
#include "npc_enemy.h"
#include "npc_enemy_fighter.h"
#include "npc_enemy_worker.h"
#include "npc_enemy_bodyguard.h"
#include "npc_unaffiliated.h"
#include "parse.h"
#include "world.h"
#include "threadsafe_queue.h"
#include "threadsafe_socket_lookup.h"
#include "message.h"

class Game
{
private:
	threadsafe::queue<Message> inbound_queue; // <socket_ID, message>
	threadsafe::queue<Message> outbound_queue; // <socket_ID, message>

	threadsafe::socket_lookup clients;
	
	map<string, shared_ptr<Character>> actors; // active/online PC and NPC ids
	std::mutex actors_mutex; // serves for both of the above types

	World world; // the game world object

public:

	Game();

	void login(const string & user_id);

	void main_test_loop();

	Update_Messages execute_command(const string & actor_id, const vector<string> & command);

	void networking_thread();

private:

	// capture incoming data and write it to the input queue
	void client_thread(SOCKET client_ID);

	// process data, moving it from the input queue to the output queue
	void processing_thread();

	// remove data from the outbound queue and send it the to specified client
	void outbound_thread();

	// close a socket, platform independent
	void close_socket(SOCKET socket);
};

#endif
