﻿/* Jim Viebke
May 15 2015 */

#include <iostream>
#include <memory>

#include "game.h"

#ifndef WIN32 // define some types and values for Linux builds
typedef int SOCKET;
const int INVALID_SOCKET = 0xffff;
#endif

Game::Game()
{
	// load crafting recipes lookup
	Character::recipes.load();

	// load the parse dictionary
	Parse::initialize();

	// load the world
	world.load();
}

void Game::login(const string & user_id)
{
	// create a player character
	PC player(user_id);
	// load the player's data and place the player in the world
	player.login(world);
	// add the character to the actor registry
	actors.insert(pair<string, shared_ptr<PC>>(player.name, make_shared<PC>(player)));
}

void Game::main_test_loop() // debugging
{


	/*{
		// pass name, faction, ai type
		Hostile_NPC_Fighter jeb("Jeb", C::NPC_HOSTILE_FACTION_ID);
		jeb.login(world);

		jeb.move(C::WEST, world); // trust me
		jeb.move(C::SOUTH, world);
		jeb.move(C::SOUTH, world);
		jeb.move(C::EAST, world);
		jeb.move(C::EAST, world);
		jeb.move(C::EAST, world);
		jeb.move(C::EAST, world);

		actors.insert(make_pair(jeb.name, make_shared<Hostile_NPC_Fighter>(jeb)));
		}*/

	{
		shared_ptr<Hostile_NPC_Worker> bob = make_shared<Hostile_NPC_Worker>("Bob");
		bob->login(world);
		actors.insert(make_pair(bob->name, bob));
	}

	{
		shared_ptr<Hostile_NPC_Bodyguard> bill = make_shared<Hostile_NPC_Bodyguard>("Bill", "Bob");
		bill->login(world);
		// bill->set_target
		actors.insert(make_pair(bill->name, bill));
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
		if (U::is<PC>(actors.find("dev")->second))
		{
			shared_ptr<PC> dev = U::convert_to<PC>(actors.find("dev")->second);

			if (auto_advance == 0)
			{
				cout << endl
					<< endl
					<< dev->generate_area_map(world, actors) << endl // a top down map
					<< "Your coordinates are " << dev->x << ", " << dev->y << " (index " << dev->z << ")"
					<< world.room_at(dev->x, dev->y, dev->z)->summary(dev->name) // "You look around and notice..."
					<< dev->print(); // prepend "You have..."

#ifndef _WIN32
				pugi::xml_document document;

				// load the file from the disk into the xml_document in memory
				document.load_file(string("./server-files/example.xml").c_str());

				// select the root/test node, create it if it does not exist
				xml_node root_node = document.child(string("test").c_str());
				if (root_node.empty())
				{
					root_node = document.append_child(string("test").c_str());
				}

				// remove any existing sample node
				root_node.remove_child(string("sample").c_str());

				// create an ostringstream and add the same printout from the console
				ostringstream oss;
				oss << endl
					<< endl
					<< dev->generate_area_map(world, actors) << endl // a top down map
					<< "Your coordinates are " << dev->x << ", " << dev->y << " (index " << dev->z << ")"
					<< world.room_at(dev->x, dev->y, dev->z)->summary(dev->name) // "You look around and notice..."
					<< dev->print(); // prepend "You have..."

				// main print out
				oss << endl
					<< endl
					<< "  " << output << endl
					<< endl
					<< "> " << flush;

				// append a sample node to the test/root node, append an anonymous pcdata node to the sample node,
				// and append the contents of the ostringstream to the anonymous pcdata node
				root_node.append_child(string("sample").c_str()).append_child(node_pcdata).set_value(oss.str().c_str());

				// save the document
				document.save_file(string("./server-files/example.xml").c_str()); // returns an unused boolean
#endif

			}
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
			--auto_advance;
		}
		else // most of the time
		{
			getline(cin, input);
		}

		// process input
		cout << "\nDEBUG sending to Parse::tokenize(): " << input << endl;
		vector<string> tokenized_input = Parse::tokenize(input);
		cout << "\nDEBUG parsed input: ";
		U::print(tokenized_input);

		// hardcoding for development purposes
		if (tokenized_input.size() > 1 && tokenized_input[0] == C::WAIT_COMMAND)
		{
			// tokenize the original input to prevent the number from being removed
			stringstream ss(input);
			const istream_iterator<string> begin(ss);
			vector<string> strings(begin, istream_iterator<string>());

			// extract and save the number of turns to delay
			stringstream auto_advance_count;
			auto_advance_count << strings[1];
			auto_advance_count >> auto_advance;
			--auto_advance; // fix a glitch
		}
		else // auto-advance was not invoked
		{
			// execute processed command against game world
			// only count loaded rooms if total room count is less than or equal to 100K (100*100*10)
			cout << "\nDEBUG Entering execute_command(), rooms loaded: " << ((C::WORLD_X_DIMENSION*C::WORLD_Y_DIMENSION*C::WORLD_Z_DIMENSION <= 100000) ? U::to_string(world.count_loaded_rooms()) : "(too large to count)") << "...";
			Update_Messages updates = execute_command("dev", tokenized_input);
			cout << "\nDEBUG Exited execute_command(), rooms loaded: " << ((C::WORLD_X_DIMENSION*C::WORLD_Y_DIMENSION*C::WORLD_Z_DIMENSION <= 100000) ? U::to_string(world.count_loaded_rooms()) : "(too large to count)") << "...";
		}

		{
			shared_ptr<Character> bob = actors.find("Bob")->second;
			shared_ptr<Character> bill = actors.find("Bill")->second;
			cout << "DEBUG: Distance between bodyguard and worker before update : " << U::diagonal_distance(bob->x, bob->y, bill->x, bill->y) << endl;
		}

		// now execute updates for all NPCs
		for (pair<const string, shared_ptr<Character>> & actor : actors)
		{
			if (U::is<NPC>(actor.second))
			{
				shared_ptr<NPC> npc = U::convert_to<NPC>(actor.second);

				if (auto_advance == 0)
				{
					// cout << npc->get_objectives() << endl; // debugging (before update)
					cout << npc->get_inventory() << endl; // debugging (before update)
				}
				npc->update(world, actors);
				if (auto_advance == 0)
				{
					cout << npc->get_objectives() << endl; // debugging (after update)
					cout << npc->get_inventory() << endl; // debugging (after update)
				}

				actor.second = npc; // make sure to save back
			}
		}

		{
			shared_ptr<Character> bob = actors.find("Bob")->second;
			shared_ptr<Character> bill = actors.find("Bill")->second;
			cout << "DEBUG: Distance between bodyguard and worker after update : " << U::diagonal_distance(bob->x, bob->y, bill->x, bill->y) << endl;
		}
	}
}

Update_Messages Game::execute_command(const string & actor_id, const vector<string> & command)
{
	// "help"
	if (command.size() == 1 && command[0] == C::SHOW_HELP_COMMAND)
	{
		return Update_Messages(string("help:\n") +
			"\nlegend" +
			"\nrecipes" +
			"\nrecipes [search keyword]" +
			"\nmove [compass direction]" +
			"\ntake / drop / craft / mine / equip / dequip / chop / smash [item]" +
			"\nequipped" +
			"\nadd / place / put / drop [item] into chest" +
			"\ntake [item] from chest" +
			"\nchest" +
			"\nattack [compass direction] wall / door" +
			"\nconstruct [material] ceiling / floor" +
			"\nconstruct [compass direction] [material] wall" +
			"\nconstruct [compass direction] [material] wall with [material] door");
	}
	else if (command.size() == 1 && command[0] == C::LEGEND_COMMAND)
	{
		return Update_Messages(string("legend:\n") +
			"\n " + C::FOREST_CHAR + "     forest" +
			"\n " + C::WATER_CHAR + "     water" +
			"\n " + C::LAND_CHAR + "     land" +
			"\n " + C::GENERIC_MINERAL_CHAR + "     a mineral deposit" +
			"\n" +
			"\n " + C::PLAYER_CHAR + "     you" +
			"\n 1     number of militants" +
			"\n " + C::NPC_NEUTRAL_CHAR + "     one or more neutral inhabitants" +
			"\n" +
			"\n " + C::ITEM_CHAR + "     one or more items" +
			"\n " + C::CHEST_CHAR + "     a chest" +
			"\n" +
			"\n " + C::WE_WALL + C::WE_WALL + C::WE_WALL + "   a wall" +
			"\n " + C::WE_WALL + C::WE_DOOR + C::WE_WALL + "   a wall with a door" +
			"\n " + C::WE_WALL + C::RUBBLE_CHAR + C::WE_WALL + "   a smashed door or wall (traversable)");
	}
	// moving: "move northeast" OR "northeast"
	else if ((command.size() == 2 && command[0] == C::MOVE_COMMAND)
		|| command.size() == 1 && U::contains(C::direction_ids, command[0]))
	{
		return actors.find(actor_id)->second->move(command[command.size() - 1], world); // passes direction (last element in command) and world
	}
	// take: "take branch"
	else if (command.size() == 2 && command[0] == C::TAKE_COMMAND)
	{
		return Update_Messages(actors.find(actor_id)->second->take(command[1], world)); // (item, worlds)
	}
	// dropping item: "drop staff"
	else if (command.size() == 2 && command[0] == C::DROP_COMMAND)
	{
		return Update_Messages(actors.find(actor_id)->second->drop(command[1], world)); // (item_id, world)
	}
	// crafting: "craft sword"
	else if (command.size() == 2 && command[0] == C::CRAFT_COMMAND)
	{
		return Update_Messages(actors.find(actor_id)->second->craft(command[1], world)); // (item_id, world)
	}
	else if (command.size() == 2 && command[0] == C::MINE_COMMAND)
	{
		return Update_Messages(actors.find(actor_id)->second->craft(command[1], world)); // (item_id, world)
	}
	// making ceiling/floor: "construct stone floor/ceiling"
	else if (command.size() == 3 && command[0] == C::CONSTRUCT_COMMAND)
	{
		return Update_Messages(actors.find(actor_id)->second->construct_surface(command[1], command[2], world)); // material, direction, world
	}
	// making walls: "construct west stone wall"
	else if (command.size() == 4 && command[0] == C::CONSTRUCT_COMMAND && command[3] == C::WALL)
	{
		return Update_Messages(actors.find(actor_id)->second->construct_surface(command[2], command[1], world)); // material, direction, world
	}
	// "construct west stone wall with stick door"
	else if (command.size() == 7 && command[0] == C::CONSTRUCT_COMMAND && command[3] == C::WALL && command[4] == C::WITH_COMMAND && command[6] == C::DOOR)
	{
		return Update_Messages(actors.find(actor_id)->second->construct_surface_with_door(command[2], command[1], command[5], world)); // material, direction, world
	}
	// waiting: "wait"
	else if (command.size() == 1 && command[0] == C::WAIT_COMMAND)
	{
		return Update_Messages("You wait."); // (item_id, world)
	}
	// printing out the full library of recipes: "recipes"
	else if (command.size() == 1 && command[0] == C::PRINT_RECIPES_COMMAND)
	{
		return Update_Messages(Character::recipes.get_recipes()); // (item_id, world)
	}
	// print out any recipes where the name of the recipe contains the 2nd command
	else if (command.size() > 1 && command[0] == C::PRINT_RECIPES_COMMAND)
	{
		return Update_Messages(Character::recipes.get_recipes_matching(command[1]));
	}
	// the player is attacking a wall "smash west wall"
	else if (command.size() >= 3 && command[0] == C::ATTACK_COMMAND && U::contains(C::surface_ids, command[1])
		&& command[2] == C::WALL)
	{
		return Update_Messages(actors.find(actor_id)->second->attack_surface(command[1], world));
	}
	// the player is attacking a door "smash west door"
	else if (command.size() >= 3 && command[0] == C::ATTACK_COMMAND && U::contains(C::surface_ids, command[1])
		&& command[2] == C::DOOR)
	{
		return Update_Messages(actors.find(actor_id)->second->attack_door(command[1], world));
	}
	// the player is attacking an item
	else if (command.size() == 2 && command[0] == C::ATTACK_COMMAND)
	{
		return Update_Messages(actors.find(actor_id)->second->attack_item(command[1], world));
	}
	// logout
	else if (command.size() == 1 && command[0] == C::LOGOUT_COMMAND)
	{
		return Update_Messages(actors.find(actor_id)->second->logout());
	}
	// equip [item]
	else if (command.size() == 2 && command[0] == C::EQUIP_COMMAND)
	{
		return actors.find(actor_id)->second->equip(command[1]);
	}
	// dequip (2nd arg is optional and ignored)
	else if ((command.size() == 1 || command.size() == 2) && command[0] == C::DEQUIP_COMMAND)
	{
		return actors.find(actor_id)->second->unequip();
	}
	// put item in chest
	else if (command.size() == 4 && command[0] == C::DROP_COMMAND && command[2] == C::INSERT_COMMAND && command[3] == C::CHEST_ID)
	{
		return Update_Messages(actors.find(actor_id)->second->add_to_chest(command[1], world));
	}
	// chest
	else if (command.size() == 1 && command[0] == C::CHEST_ID)
	{
		return Update_Messages(actors.find(actor_id)->second->look_inside_chest(world));
	}
	// take [item] from chest
	else if (command.size() == 4 && command[0] == C::TAKE_COMMAND && command[2] == C::FROM_COMMAND && command[3] == C::CHEST_ID)
	{
		return Update_Messages(actors.find(actor_id)->second->take_from_chest(command[1], world));
	}
	else if (command.size() == 1 && (command[0] == C::EQUIP_COMMAND || command[0] == C::ITEM_COMMAND))
	{
		// extract the actor
		const shared_ptr<Character> actor = actors.find(actor_id)->second;
		if (U::is<PC>(actor)) // if the actor is a Player_Character
		{
			// convert the actor to a Player_Character
			return Update_Messages(U::convert_to<PC>(actor)->get_equipped_item_info());
		}
	}

	return Update_Messages("Nothing happens.");
}

void Game::networking_thread()
{
	cout << "\nStarting the server...";

#ifdef WIN32
	WSADATA lpWSAData;
	WSAStartup(MAKEWORD(2, 2), &lpWSAData);
#endif

	const SOCKET socket_1 = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (socket_1 == INVALID_SOCKET)
	{
		std::cout << "invalid socket: " << WSAGetLastError() << std::endl;
	}

	sockaddr_in name;
	memset(&name, 0, sizeof(sockaddr_in));
	name.sin_family = AF_INET;
	name.sin_port = htons(C::GAME_PORT_NUMBER);
	name.sin_addr.S_un.S_addr = 0; // open port on all network interfaces

	// bind the socket
	::bind(socket_1, (sockaddr*)&name, sizeof(sockaddr_in));

	// open the port for clients to connect, maintaining a backlog of up to 3 waiting connections
	int listen_result = listen(socket_1, 3); // non-blocking

	// create a holder for incoming client information
	sockaddr_in client_address;
	memset(&client_address, 0, sizeof(sockaddr_in));

	// start the thread responsible for processing data between the input and output queue
	std::thread(&Game::processing_thread, this).detach();
	// start the thread responsible for dispatching output to connected clients
	std::thread(&Game::outbound_thread, this).detach();

	cout << "\nServer started on port " << C::GAME_PORT_NUMBER << ".";

	for (;;)
	{
		// execution pauses inside of accept() until an incoming connection is received
		SOCKET client = accept(socket_1, (sockaddr*)&client_address, NULL); // blocking

		// start a thread to receive messages from this client
		std::thread(&Game::client_thread, this, client).detach(); // detach() because the thread is responsible for destroying itself
	}

#ifdef WIN32
	WSACleanup();
#endif
}



void Game::client_thread(SOCKET client_ID)
{
	{
		// assemble initial username
		stringstream ss;
		ss << "User_" << client_ID;
		const std::string user_ID = ss.str();

		// add client to clients list
		clients.insert(make_pair(client_ID, user_ID));

		// log the player in
		std::unique_lock<std::mutex> lock(actors_mutex); // lock the actors structure while we modify it
		shared_ptr<PC> player = make_shared<PC>(user_ID);
		player->login(world);
		actors.insert(make_pair(user_ID, player));

		// send a welcome message through the outbound queue
		outbound_queue.put(Message(client_ID, "Welcome to IslandMUD! Your username is \"" + user_ID + "\".\n\n"));
	}

	// allocate a buffer on the stack to store incoming messages
	char input[1024];

	for (;;)
	{
		// execution pauses inside of recv() until the user sends data (one of these for each user in their own thread)
		int data_read = recv(client_ID, input, 1024, 0);

		// check if reading the socket failed
		if (data_read == 0 || data_read == -1) // graceful disconnect, less graceful disconnect (respectively)
		{
			std::unique_lock<std::mutex> lock(actors_mutex); // gain exclusive hold of the client map for modification
			close_socket(client_ID); // close socket (platform-independent)
			const std::string user_ID = clients.find(client_ID)->second; // find username
			actors.find(user_ID)->second->logout(); // logout the user
			actors.erase(user_ID); // erase the user
			clients.erase(client_ID); // erase the client record
			return; // the client's personal thread is destroyed
		}

		std::stringstream user_input;
		for (int i = 0; i < data_read; ++i)
			user_input << input[i];

		inbound_queue.put(Message(client_ID, user_input.str()));
	}
}

void Game::processing_thread()
{
	for (;;)
	{
		// destructively get the next inbound message
		const Message inbound_message = inbound_queue.get(); // user return by reference (blocking) to get the next message in the inbound_queue

		// get the user ID for the inbound socket
		const string user_ID = clients.get(inbound_message.user_socket_ID);

		// create a stringstream to assemble the return message
		stringstream action_result;
		action_result << "\n\n";

		// don't allow the actors structure to be modified
		const std::unique_lock<mutex> lock(actors_mutex);

		// extract a copy of the user
		const shared_ptr<Character> character = actors.find(user_ID)->second;

		// execute the user's parsed command against the world
		const Update_Messages update_messages = execute_command(user_ID, Parse::tokenize(inbound_message.data));

		// gather some more information to add to the response message
		if (U::is<PC>(character))
		{
			const shared_ptr<PC> player = U::convert_to<PC>(character);

			action_result << player->generate_area_map(this->world, this->actors) << endl // a top down map
				<< "Your coordinates are " << player->x << ", " << player->y << " (index " << player->z << ")"
				<< this->world.room_at(player->x, player->y, player->z)->summary(player->name) // "You look around and notice..."
				<< player->print(); // prepend "You have..."
		}

		// add the update message to the end of the outbound message
		action_result << update_messages.to_user;

		// create an outbound message to the client in question
		outbound_queue.put(Message(inbound_message.user_socket_ID, user_ID + ": " + action_result.str()));

		if (update_messages.to_room != nullptr) // determine if a message needs to be sent to all other player characters in the room
		{
			// get a list of all players in the room
			const vector<string> area_actor_ids = world.room_at(character->x, character->y, character->z)->get_actor_ids();

			for (const string & actor_id : area_actor_ids) // for each player in the room
			{
				// if the player is not "self" and the player is a human
				if (actor_id != user_ID && U::is<PC>(actors.find(actor_id)->second))
				{
					// send the room update to the player
					outbound_queue.put(Message(clients.find(actor_id)->second, *update_messages.to_room));
				}
			}
		}
	}
}

void Game::outbound_thread()
{
	for (;;)
	{
		const Message message = outbound_queue.get(); // return by reference (blocking)

		// dispatch data to the user (nonblocking) (because we're using TCP, data is lossless unless total failure occurs)
		send(message.user_socket_ID, message.data.c_str(), message.data.size(), 0);
	}
}

void Game::close_socket(SOCKET socket)
{
#ifdef WIN32
	closesocket(socket);
#else
	close(socket);
#endif
}
