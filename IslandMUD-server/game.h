/* Jim Viebke
Feb 14, 2015 */

#ifndef GAME_H
#define GAME_H

#include <map>
#include <queue> // for queues

#include "resources.h"
#include "constants.h"
#include "character.h"
#include "player_character.h"
#include "non_player_character.h"
#include "parse.h"
#include "world.h"

class Game
{
public:

	World world; // the game world object
	map<string, shared_ptr<Character>> actors; // active/online PC and NPC ids

	queue<pair<string, string>> player_input_queue; // (player_id, command) input queue (raw input)
	queue<pair<string, string>> command_queue; // (player_id, command) syntactially valid commands to attempt execution against the game world
	queue<pair<string, string>> player_response_queue; // (player_id, response) output queue (responses to players)

	Game() {}

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

	void load();

	string execute_command(const string & actor_id, const vector<string> & command);
};

#endif
