/*
Jim Viebke
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


	void main_test_loop() // debugging
	{
		// temporary scope to create player object
		{
			// create a player character
			PC player;

			// set the PC's username to "dev"
			player.name = "dev";

			// add the character to the actor registry
			actors.insert(pair<string, shared_ptr<PC>>(player.name, make_shared<PC>(player)));
		}

		// load the area around the player's spawn
		cout << "\nLoading in world around player spawn...";
		world.load_view_radius_around(C::DEFAULT_SPAWN_X, C::DEFAULT_SPAWN_Y, "dev"); // hard coded

		// add the player to the world based on name
		cout << "\nAdding player to spawn...";
		world.room_at(C::DEFAULT_SPAWN_X, C::DEFAULT_SPAWN_Y, C::DEFAULT_SPAWN_Z)->add_actor("dev"); // hard coded again

		// create some objects and primites to help run the game
		Parse parse; // this could be made to use static functions for a microscopic memory optimization
		int auto_advance = 0; // debugging only
		string input, output = ""; // I/O holders

		while (true) // play indefinitely
		{
			// print out
			if (R::is<PC>(actors.find("dev")->second))
			{
				shared_ptr<PC> dev = R::convert_to<PC>(actors.find("dev")->second);

				cout << endl
					<< endl
					<< world.generate_area_map_for(dev->x, dev->y, dev->z) << endl // a top down map
					<< "Your coordinates are " << dev->x << ", " << dev->y << " (index " << dev->z << ")" << endl
					<< world.room_at(dev->x, dev->y, dev->z)->summary() << endl // "You look around and notice..."
					<< endl
					<< dev->print(); // prepend "You have..."
			}

			// main print out
			cout << endl
				<< endl
				<< "  " << output << endl
				<< endl
				<< "> " << flush;

			// get or set input
			if (auto_advance > 0)
			{
				input = "wait"; // automatically set user input instead of getting it
			}
			else
			{
				getline(cin, input);
			}

			// process input
			cout << "\nDEBUG sending to parse.tokenize: " << input << endl;
			vector<string> tokenized_input = parse.tokenize(input);
			cout << "\nDEBUG parsed input: ";
			R::print(tokenized_input);

			// execute processed command against game world
			cout << "\nDEBUG Entering execute_command(), rooms loaded: " << world.count_loaded_rooms() << "...";
			output = execute_command("dev", tokenized_input);
			cout << "\nDEBUG Exited execute_command(), rooms loaded: " << world.count_loaded_rooms() << "...";
		}
	}

	void load()
	{
		// load the parse dictionary
		Parse::initialize();

		// load the recipe lookup
		Character::recipes = Recipes();

		// load and create the world. All rooms will be on disk.
		world.load();

		// accept incoming signals to the network, and adds then to the raw input queue
		// thread accept_input_thread;

		// destructively removes input from raw input queue, parses into a lowercase synonym-replaced syntatically correct command, adds to command queue
		// also checks the user's timestamp
		// thread parse_input_thread;

		// attempts to execute commands against the game world (attack, buy, sell). Writes update message to output queue for PCs only.
		// NPCs write directly to PROCESS_COMMAND_THREAD
		// thread process_commands_thread;

		// destructively takes update messages, sends to correct user
		// thread dispatch_output_thread;
	}

	string execute_command(const string & actor_id, const vector<string> & command)
	{
		// "help"
		if (command.size() == 1 && command[0] == C::GET_HELP_COMMAND)
		{
			return string(
				"help:\n\nmove [compass direction]\n") +
				"take [item]\n" +
				"drop [item]\n" +
				"craft [item]\n" +
				"construct [material] [ceiling/floor]\n" +
				"construct [compass direction] [material] wall";
		}
		// moving: "move northeast"
		else if (command.size() == 2 && command[0] == C::MOVE_COMMAND)
		{
			return actors.find(actor_id)->second->move(command[1], world); // (direction, world)
		}
		// take: "take branch"
		else if (command.size() == 2 && command[0] == C::TAKE_COMMAND)
		{
			return actors.find(actor_id)->second->take(command[1], world); // (item, worlds)
		}
		// dropping item: "drop staff"
		else if (command.size() == 2 && command[0] == C::DROP_COMMAND)
		{
			return actors.find(actor_id)->second->drop(command[1], world); // (item_id, world)
		}
		// crafting: "craft sword"
		else if (command.size() == 2 && command[0] == C::CRAFT_COMMAND)
		{
			return actors.find(actor_id)->second->craft(command[1], world); // (item_id, world)
		}
		// making room surfaces: "construct stone floor/ceiling" or "construct north/east/south/west stone wall"
		else if ((command.size() == 3 || command.size() == 4) && command[0] == C::CONSTRUCT_COMMAND)
		{
			// Arg 1: material is always the second-last word
			// Arg 2: 3 commands, surface_id is the 3rd. 4 commands, surface is the 2nd.
			return actors.find(actor_id)->second->construct_surface(command[command.size() - 2], (command.size() == 3) ? command[2] : command[1], world);
		}
		// waiting: "wait"
		else if (command.size() == 1 && command[0] == C::WAIT_COMMAND)
		{
			return "You wait."; // (item_id, world)
		}
		// printing out the full library of recipes: "recipes"
		else if (command.size() == 1 && command[0] == C::PRINT_RECIPES_COMMAND)
		{
			return Character::recipes.get_recipes(); // (item_id, world)
		}

		return "Nothing happens.";
	}



};

#endif
