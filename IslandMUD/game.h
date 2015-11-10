/* Jim Viebke
Feb 14, 2015 */

#ifndef GAME_H
#define GAME_H

#include <map>
#include <queue>
#include <thread>
#include <mutex>

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

class Game
{
private:

	World world; // the game world object
	map<string, shared_ptr<Character>> actors; // active/online PC and NPC ids

	// user ID, command
	queue<pair<string, string>> input_queue; // contains user commands to execute against the game world
	mutex input_queue_mutex;

	// user ID, message
	queue<pair<string, string>> output_queue; // contains outbound messages to players
	mutex output_queue_mutex;

public:

	Game();

	/*



	while (true) // PROCESS_COMMAND_THREAD
	{
	-- sleep 1/10 second (prevent high CPU while waiting for commands)

	-- while commands.size() > 0
	-- -- destructively take command from front of queue
	-- -- -- attempt to execute against game world

	}

	Every time a user performs an action:	agent.timestamp = current_time + action_cost
	I don't really like that. Condsider revisiting it.

	*/

	/*void play() // loops forever
	{
	vector<thread> thread_vec;

	// this probably won't help
	thread_vec.reserve(3);

	thread_vec.push_back(thread(acceptorLoop));
	this_thread::sleep_for(chrono::milliseconds(1000));

	thread_vec.push_back(thread(requestLoop));
	this_thread::sleep_for(chrono::milliseconds(1000));

	thread_vec.push_back(thread(responseLoop));
	this_thread::sleep_for(chrono::milliseconds(1000));


	// now the game runs


	// wait for all threads to exit (should only occur on shutdown
	for (thread & thread : thread_vec)
	{
	if (thread.joinable())
	{
	thread.join();
	}
	}

	cout << "Game ended.\n";
	cin.ignore();
	}*/


	void main_test_loop();

private:
	string execute_command(const string & actor_id, const vector<string> & command);
};

#endif
