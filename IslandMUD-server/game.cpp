/* Jim Viebke
May 15 2015 */

#include "game.h"

void Game::main_test_loop() // debugging
{
	{
		// create a player character
		PC player("dev", C::PC_FACTION_ID);
		// load the player's data and place the player in the world
		player.login(world);
		// add the character to the actor registry
		actors.insert(pair<string, shared_ptr<PC>>(player.name, make_shared<PC>(player)));
	}

	{
		Hostile_NPC jeb("Jeb", C::NPC_HOSTILE_FACTION_ID);
		jeb.login(world);

		jeb.move(C::WEST, world); // trust me
		jeb.move(C::SOUTH, world);
		jeb.move(C::SOUTH, world);
		jeb.move(C::EAST, world);
		jeb.move(C::EAST, world);
		jeb.move(C::EAST, world);
		jeb.move(C::EAST, world);

		actors.insert(make_pair(jeb.name, make_shared<Hostile_NPC>(jeb)));
	}

	{
		Neutral_NPC bill("Bill", C::NPC_NEUTRAL_FACTION_ID);
		bill.login(world);
		for (unsigned i = 0; i < 20; ++i)
		{
			bill.move(C::WEST, world);
		}
		actors.insert(make_pair(bill.name, make_shared<Neutral_NPC>(bill)));
	}


	// Uncomment the below block to add a hostile NPC for each char in the string below.
	// The char will be the name of the character.
	// Not a great idea for debug builds.
	/*{
		string temp = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
		for (unsigned i = 0; i < temp.size(); ++i)
		{
		string s(1, temp[i]);
		Hostile_NPC h_npc(s, C::NPC_HOSTILE_FACTION_ID);
		h_npc.login(world);
		actors.insert(make_pair(h_npc.name, make_shared<Hostile_NPC>(h_npc)));
		}
		} */



	// create some holders to support the main debug loop
	int auto_advance = 0; // debugging only - idling is default in the final game
	string input, output = ""; // I/O holders

	while (true) // play indefinitely
	{
		// print out
		if (R::is<PC>(actors.find("dev")->second))
		{
			shared_ptr<PC> dev = R::convert_to<PC>(actors.find("dev")->second);

			cout << endl
				<< endl
				<< dev->generate_area_map(world, actors) << endl // a top down map
				<< "Your coordinates are " << dev->x << ", " << dev->y << " (index " << dev->z << ")"
				<< world.room_at(dev->x, dev->y, dev->z)->summary(dev->name) // "You look around and notice..."
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
		else // most of the time
		{
			getline(cin, input);
		}

		// process input
		cout << "\nDEBUG sending to Parse::tokenize(): " << input << endl;
		vector<string> tokenized_input = Parse::tokenize(input);
		cout << "\nDEBUG parsed input: ";
		R::print(tokenized_input);

		// execute processed command against game world
		// only count loaded rooms if total room count is less than or equal to 100K (100*100*10)
		cout << "\nDEBUG Entering execute_command(), rooms loaded: " << ((C::WORLD_X_DIMENSION*C::WORLD_Y_DIMENSION*C::WORLD_Z_DIMENSION <= 100000) ? R::to_string(world.count_loaded_rooms()) : "(too large to count)") << "...";
		output = execute_command("dev", tokenized_input);
		cout << "\nDEBUG Exited execute_command(), rooms loaded: " << ((C::WORLD_X_DIMENSION*C::WORLD_Y_DIMENSION*C::WORLD_Z_DIMENSION <= 100000) ? R::to_string(world.count_loaded_rooms()) : "(too large to count)") << "...";

		// now execute updates for all NPCs
		for (pair<const string, shared_ptr<Character>> & actor : actors)
		{
			if (R::is<NPC>(actor.second))
			{
				shared_ptr<NPC> npc = R::convert_to<NPC>(actor.second);

				cout << npc->get_objectives() << endl; // debugging (before update)
				cout << npc->get_inventory() << endl; // debugging (before update)
				npc->update(world, actors);
				cout << npc->get_objectives() << endl; // debugging (after update)
				cout << npc->get_inventory() << endl; // debugging (after update)

				actor.second = npc; // make sure to save back
			}
		}
	}
}

void Game::load()
{
	// load crafting recipes lookup
	Character::recipes.load();

	// load the parse dictionary
	Parse::initialize();

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

string Game::execute_command(const string & actor_id, const vector<string> & command)
{
	// "help"
	if (command.size() == 1 && command[0] == C::HELP_COMMAND)
	{
		return string("help:\n") +
			"\nrecipes" +
			"\nmove [compass direction]" +
			"\ntake / drop / craft / equip / dequip [item]" +
			"\nattack [compass direction] wall / door" +
			"\nconstruct [material] ceiling / floor" +
			"\nconstruct [compass direction] [material] wall" +
			"\nconstruct [compass direction] [material] wall with [material] door";			
	}
	// moving: "move northeast" OR "northeast"
	else if ((command.size() == 2 && command[0] == C::MOVE_COMMAND)
		|| command.size() == 1 && R::contains(C::direction_ids, command[0]))
	{
		return actors.find(actor_id)->second->move(command[command.size() - 1], world); // passes direction (last element in command) and world
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
	// making ceiling/floor: "construct stone floor/ceiling"
	else if (command.size() == 3 && command[0] == C::CONSTRUCT_COMMAND)
	{
		return actors.find(actor_id)->second->construct_surface(command[1], command[2], world); // material, direction, world
	}
	// making walls: "construct west stone wall"
	else if (command.size() == 4 && command[0] == C::CONSTRUCT_COMMAND && command[3] == C::WALL)
	{
		return actors.find(actor_id)->second->construct_surface(command[2], command[1], world); // material, direction, world
	}
	// "construct west stone wall with stick door"
	else if (command.size() == 7 && command[0] == C::CONSTRUCT_COMMAND && command[3] == C::WALL && command[4] == C::WITH_COMMAND && command[6] == C::DOOR)
	{
		return actors.find(actor_id)->second->construct_surface_with_door(command[2], command[1], command[5], world); // material, direction, world
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
	// the player is attacking a wall "smash west wall"
	else if (command.size() >= 3 && command[0] == C::ATTACK_COMMAND && R::contains(C::surface_ids, command[1])
		&& command[2] == C::WALL)
	{
		return actors.find(actor_id)->second->attack_surface(command[1], world);
	}
	// the player is attacking a door "smash west door"
	else if (command.size() >= 3 && command[0] == C::ATTACK_COMMAND && R::contains(C::surface_ids, command[1])
		&& command[2] == C::DOOR)
	{
		return actors.find(actor_id)->second->attack_door(command[1], world);
	}
	else if (command.size() == 1 && command[0] == C::LOGOUT_COMMAND)
	{
		return actors.find(actor_id)->second->logout();
	}
	// equip and dequip are still buggy
	else if (command.size() == 2 && command[0] == C::EQUIP_COMMAND)
	{
		return actors.find(actor_id)->second->equip(command[1]);
	}
	else if (command.size() == 2 && command[0] == C::DEQUIP_COMMAND)
	{
		return actors.find(actor_id)->second->unequip(command[1]);
	}

	return "Nothing happens.";
}
