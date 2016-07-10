/* Jim Viebke
May 15 2015 */

#include <iostream>
#include <memory>

#include "game.h"

#ifndef WIN32 // define some types and values for Linux builds
const int INVALID_SOCKET = 0xffff;
#endif

Game::Game()
{
	// start the threads for listening on port numbers
	std::thread(&Game::networking_thread, this, C::GAME_PORT_NUMBER, &Game::client_thread).detach();
	std::thread(&Game::networking_thread, this, C::GAME_MAP_PORT_NUMBER, &Game::client_map_thread).detach();

	// start the thread responsible for dispatching output to connected clients
	std::thread(&Game::outbound_thread, this).detach();

	// start the thread responsible for all things NPC
	std::thread(&Game::NPC_thread, this).detach();
}

Update_Messages Game::execute_command(const std::string & actor_id, const std::vector<std::string> & command)
{
	// "help"
	if (command.size() == 1 && command[0] == C::SHOW_HELP_COMMAND)
	{
		return Update_Messages(std::string("help:\n") +
			"\nlegend" +
			"\ninventory / inv / i" +
			"\nlook / l" +
			"\nrecipes" +
			"\nrecipes [search keyword]" +
			"\nmove [compass direction]" +
			"\ntake / drop / craft / mine / equip / dequip / chop / smash [item]" +
			"\nequipped" +
			"\nadd / place / put / drop [item] into chest" +
			"\ntake [item] from chest" +
			"\nchest" +
			"\nattack [compass direction] wall / door" +
			"\nattack [name]" +
			"\nconstruct [material] ceiling / floor" +
			"\nconstruct [compass direction] [material] wall" +
			"\nconstruct [compass direction] [material] wall with [material] door");
	}
	// "legend"
	else if (command.size() == 1 && command[0] == C::LEGEND_COMMAND)
	{
		return Update_Messages(std::string("legend:\n") +
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
			"\n " + C::TABLE_CHAR + "     a table" +
			"\n" +
			"\n " + C::WALL_CHAR + C::WALL_CHAR + C::WALL_CHAR + "   a wall" +
			"\n " + C::WALL_CHAR + C::DOOR_CHAR + C::WALL_CHAR + "   a wall with a door" +
			"\n " + C::WALL_CHAR + C::RUBBLE_CHAR + C::WALL_CHAR + "   a smashed door or wall (traversable)");
	}
	// "i", "inv", "inventory"
	else if (command.size() == 1 && command[0] == C::INVENTORY_COMMAND)
	{
		// if the actor is a Player_Character
		if (const std::shared_ptr<PC> & actor = U::convert_to<PC>(actors.find(actor_id)->second))
		{
			return Update_Messages(actor->get_inventory_info());
		}
	}
	// "l", "look"
	else if (command.size() == 1 && command[0] == C::LOOK_COMMAND)
	{
		// if the actor is a Player_Character
		if (const std::shared_ptr<PC> & player = U::convert_to<PC>(actors.find(actor_id)->second))
		{
			return Update_Messages(world.room_at(player->x, player->y, player->z)->summary(player->name));
		}
	}
	// moving: "move northeast" OR "northeast"
	else if ((command.size() == 2 && command[0] == C::MOVE_COMMAND)
		|| command.size() == 1 && U::contains(C::direction_ids, command[0]))
	{
		// get a shared_ptr to the acting charater
		std::shared_ptr<Character> character = actors.find(actor_id)->second;

		// get the result of making the move
		Update_Messages result = character->move(command[command.size() - 1], world); // passes direction (last element in command) and world

		// if the acting character is a player character
		if (const std::shared_ptr<PC> player = U::convert_to<PC>(character))
			// append a summary of the new area
			result.to_user += world.room_at(player->x, player->y, player->z)->summary(player->name);

		return result;
	}
	// take: "take branch"
	else if (command.size() == 2 && command[0] == C::TAKE_COMMAND)
	{
		return actors.find(actor_id)->second->take(command[1], world); // (item, world)
	}
	// take: "take [n] branches"
	else if (command.size() == 3 && command[0] == C::TAKE_COMMAND)
	{
		return actors.find(actor_id)->second->take(command[2], world, command[1]); // (item, world, count)
	}
	// dropping item: "drop staff"
	else if (command.size() == 2 && command[0] == C::DROP_COMMAND)
	{
		return actors.find(actor_id)->second->drop(command[1], world); // (item, world)
	}
	// dropping item: drop n staffs" (the tokenizer will have already converted plurals to singular)
	else if (command.size() == 3 && command[0] == C::DROP_COMMAND)
	{
		return actors.find(actor_id)->second->drop(command[2], world, command[1]);
	}
	// crafting: "craft sword"
	else if (command.size() == 2 && command[0] == C::CRAFT_COMMAND)
	{
		return actors.find(actor_id)->second->craft(command[1], world); // (item_id, world)
	}
	// mining: "mine iron"
	else if (command.size() == 2 && command[0] == C::MINE_COMMAND)
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
	// printing out the full library of recipes: "recipes"
	else if (command.size() == 1 && command[0] == C::PRINT_RECIPES_COMMAND)
	{
		return Update_Messages(Character::recipes->get_recipes()); // (item_id, world)
	}
	// print out any recipes where the name of the recipe contains the 2nd command
	else if (command.size() > 1 && command[0] == C::PRINT_RECIPES_COMMAND)
	{
		return Update_Messages(Character::recipes->get_recipes_matching(command[1]));
	}
	// the player is attacking a wall "smash west wall"
	else if (command.size() >= 3 && command[0] == C::ATTACK_COMMAND && U::contains(C::surface_ids, command[1])
		&& command[2] == C::WALL)
	{
		return actors.find(actor_id)->second->attack_surface(command[1], world);
	}
	// the player is attacking a door "smash west door"
	else if (command.size() >= 3 && command[0] == C::ATTACK_COMMAND && U::contains(C::surface_ids, command[1])
		&& command[2] == C::DOOR)
	{
		return actors.find(actor_id)->second->attack_door(command[1], world);
	}
	// the player is attacking a person or item
	else if (command.size() == 2 && command[0] == C::ATTACK_COMMAND)
	{
		// extract a shared pointer to the player
		const auto & player = actors.find(actor_id)->second;

		// if the target is another player or NPC in the room
		if (world.room_at(player->x, player->y, player->z)->is_occupied_by(command[1]))
		{
			// if the target is not a friendly player character
			auto & target = actors.find(command[1])->second;
			if (U::is<NPC>(target)) // the player exists, and can be attacked
			{
				return player->attack_character(target, world);
			}
			else // the player exists, but is a human
			{
				return Update_Messages(command[1] + " is friendly.");
			}
		}
		else // the second argument is not a nearby NPC
		{
			return actors.find(actor_id)->second->attack_item(command[1], world);
		}
	}
	// save
	else if (command.size() == 1 && command[0] == C::SAVE_COMMAND)
	{
		return actors.find(actor_id)->second->save();
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
	// put [item] in chest
	else if (command.size() == 4 && command[0] == C::DROP_COMMAND && command[2] == C::INSERT_COMMAND && command[3] == C::CHEST_ID)
	{
		return actors.find(actor_id)->second->add_to_chest(command[1], world);
	}
	// put [n] [items] in chest
	else if (command.size() == 5 && command[0] == C::DROP_COMMAND && command[3] == C::INSERT_COMMAND && command[4] == C::CHEST_ID)
	{
		return actors.find(actor_id)->second->add_to_chest(command[2], world, command[1]);
	}
	// look at chest: "chest"
	else if (command.size() == 1 && command[0] == C::CHEST_ID)
	{
		return actors.find(actor_id)->second->look_inside_chest(world);
	}
	// take [item] from chest
	else if (command.size() == 4 && command[0] == C::TAKE_COMMAND && command[2] == C::FROM_COMMAND && command[3] == C::CHEST_ID)
	{
		return actors.find(actor_id)->second->take_from_chest(command[1], world);
	}
	// take [n] [item] from chest
	else if (command.size() == 5 && command[0] == C::TAKE_COMMAND && command[3] == C::FROM_COMMAND && command[4] == C::CHEST_ID)
	{
		return actors.find(actor_id)->second->take_from_chest(command[2], world, command[1]);
	}
	// put [item] on table
	else if (command.size() == 4 && command[0] == C::DROP_COMMAND && command[2] == C::INSERT_COMMAND && command[3] == C::TABLE_ID)
	{
		return actors.find(actor_id)->second->add_to_table(command[1], world);
	}
	// put [n] [item] on table
	else if (command.size() == 5 && command[0] == C::DROP_COMMAND && command[3] == C::INSERT_COMMAND && command[4] == C::TABLE_ID)
	{
		return actors.find(actor_id)->second->add_to_table(command[2], world, command[1]);
	}
	// look at table: "table"
	else if (command.size() == 1 && command[0] == C::TABLE_ID)
	{
		return actors.find(actor_id)->second->look_at_table(world);
	}
	// take [item] from table
	else if (command.size() == 4 && command[0] == C::TAKE_COMMAND && command[2] == C::FROM_COMMAND && command[3] == C::TABLE_ID)
	{
		return actors.find(actor_id)->second->take_from_table(command[1], world);
	}
	// take [n] [items] from table
	else if (command.size() == 5 && command[0] == C::TAKE_COMMAND && command[3] == C::FROM_COMMAND && command[4] == C::TABLE_ID)
	{
		return actors.find(actor_id)->second->take_from_table(command[2], world, command[1]);
	}
	// get equipped item name
	else if (command.size() == 1 && (command[0] == C::EQUIP_COMMAND || command[0] == C::ITEM_COMMAND))
	{
		// if the actor is a Player_Character
		if (const std::shared_ptr<PC> & actor = U::convert_to<PC>(actors.find(actor_id)->second))
		{
			// convert the actor to a Player_Character
			return Update_Messages(actor->get_equipped_item_info());
		}
	}
	// debugging
	else if (command.size() == 1 && command[0] == "coord")
	{
		if (const std::shared_ptr<PC> player = U::convert_to<PC>(actors.find(actor_id)->second))
		{
			std::stringstream coord;
			coord << "Your coordinates are " << player->x << ", " << player->y << " (index " << player->z << ")";
			return Update_Messages(coord.str());
		}
	}

	return Update_Messages("Nothing happens.");
}

void Game::processing_thread()
{
	// the processing thread handles input from users who are already logged in

	for (;;)
	{
		// destructively get the next inbound message
		const Message inbound_message = inbound_queue.get(); // user return by reference (blocking) to get the next message in the inbound_queue

		// get the user ID for the inbound socket
		const std::string user_ID = clients.get_user_ID(inbound_message.user_socket_ID);

		// don't allow the actors structure to be modified
		std::unique_lock<std::mutex> lock(game_state);

		// execute the user's parsed command against the world
		const Update_Messages update_messages = execute_command(user_ID, Parse::tokenize(inbound_message.data));

		// generate_outbound_messages uses an Update_Messages object to generate all 
		generate_outbound_messages(user_ID, update_messages);
	}
}

// private member functions

void Game::networking_thread(const unsigned & listening_port, client_thread_type client_thread_pointer)
{
	std::cout << "\nStarting a listening thread for port " << listening_port << "...";

#ifdef WIN32
	WSADATA lpWSAData;
	WSAStartup(MAKEWORD(2, 2), &lpWSAData);
#endif

	const SOCKET socket_1 = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (socket_1 == INVALID_SOCKET)
	{
#ifdef WIN32
		std::cout << "invalid socket, error code: " << WSAGetLastError() << std::endl;
#else
		std::cout << "invalid socket, error code: " << errno << std::endl;
#endif
	}

	sockaddr_in name;
	memset(&name, 0, sizeof(sockaddr_in));
	name.sin_family = AF_INET;
	name.sin_port = htons(listening_port);
#ifdef WIN32
	name.sin_addr.S_un.S_addr = 0; // open port on all network interfaces
#else
	name.sin_addr.s_addr = 0;
#endif

	// bind the socket
	::bind(socket_1, (sockaddr*)&name, sizeof(sockaddr_in));

	// open the port for clients to connect, maintaining a backlog of up to 3 waiting connections
	int listen_result = listen(socket_1, 3); // non-blocking

	// create a holder for incoming client information
	sockaddr_in client_address;
	memset(&client_address, 0, sizeof(sockaddr_in));

	std::cout << "\nListening for port " << listening_port << ".";

	for (;;)
	{
		// execution pauses inside of accept() until an incoming connection is received
		SOCKET client = accept(socket_1, (sockaddr*)&client_address, NULL); // blocking

		// start a thread to receive messages from this client
		std::thread(client_thread_pointer, this, client).detach(); // detach() because the thread is responsible for destroying itself
	}

#ifdef WIN32
	WSACleanup();
#endif
}

void Game::client_thread(SOCKET client_ID)
{
	outbound_queue.put(Message(client_ID, "Welcome to IslandMUD!\n\n"));

	// allocate a buffer on the stack to store incoming messages
	char input[1024];

	const std::string user_ID = login_or_signup(client_ID); // execution stays in here until the user is signed in

	if (user_ID == "") return; // the user disconnected before signing in, the function above already cleaned up

	// finish other login/connection details
	{
		// add user to the lookup
		clients.set_socket(user_ID, client_ID);

		// log the player in
		std::lock_guard<std::mutex> lock(game_state); // lock the actors structure while we modify it
		std::shared_ptr<PC> player = std::make_shared<PC>(user_ID);
		player->login(world);
		actors.insert(std::make_pair(user_ID, player));

		// send a welcome message through the outbound queue
		outbound_queue.put(Message(client_ID, "Welcome to IslandMUD! You are playing as \"" + user_ID + "\".\n\n"));
	}

	// continuously read data from the client until the client disconnects
	for (;;)
	{
		// execution pauses inside of recv() until the user sends data
		int data_read = recv(client_ID, input, 1024, 0);

		// check if reading the socket failed
		if (data_read == 0 || data_read == -1) // graceful disconnect, less graceful disconnect (respectively)
		{
			std::lock_guard<std::mutex> lock(game_state); // gain exclusive hold of the client map for modification
			close_socket(client_ID); // close socket (platform-independent)

			// get the user's ID
			const std::string user_ID = clients.get_user_ID(client_ID); // find username
			if (user_ID == "") return; // should never happen

			// create a reference to the user's Character object
			std::shared_ptr<Character> user = actors.find(user_ID)->second; // save the user's data

			// clean up the world
			world.attempt_unload_radius(user->x, user->y, user_ID);

			// clean up the user
			user->save(); // save the user's data
			actors.erase(user_ID); // erase the user from the actor's map

			// clean up networking stuff
			clients.erase(user_ID); // erase the client record

			// clean up the user's thread
			return;
		}

		std::stringstream user_input;
		for (int i = 0; i < data_read; ++i)
			user_input << input[i];

		inbound_queue.put(Message(client_ID, user_input.str()));
	}
}

void Game::client_map_thread(SOCKET client_map_ID)
{
	outbound_queue.put(Message(client_map_ID, "Welcome to IslandMUD!\n\n"));

	// allocate a buffer on the stack to store incoming messages
	char input[1024];

	std::string user_ID;

	// loop in here until the user logs in
	for (;;)
	{
		const std::string login_instructions =
			"This is your map view. Log in on your main client, then log in here using \"username\" \"password\".\n"
			"To create an account or play the game, use port " + U::to_string(C::GAME_PORT_NUMBER) + " in another window.\n\n";

		// set instructions
		outbound_queue.put(Message(client_map_ID, login_instructions));

		// execution pauses inside of recv() until the user sends data (one of these for each user in their own thread)
		int data_read = recv(client_map_ID, input, 1024, 0);

		// check if reading the socket failed
		if (data_read == 0 || data_read == -1) // graceful disconnect, less graceful disconnect (respectively)
		{
			close_socket(client_map_ID); // close socket (platform-independent)
			return; // the user lost connection before logging in
		}

		// convert user input to parsed vector of words
		std::stringstream user_input;
		for (int i = 0; i < data_read; ++i)
			user_input << input[i];
		const std::istream_iterator<std::string> begin(user_input);
		const std::vector<std::string> input(begin, std::istream_iterator<std::string>());

		if (input.size() == 2) // only valid input size
		{
			{ // temporary scope to destroy mutex as soon as possible
				std::lock_guard<std::mutex> lock(game_state);
				if (actors.find(input[0]) == actors.cend()) continue; // repeat message if the user is not logged in
			}

			const std::string user_file = C::user_data_directory + "/" + input[0] + ".xml";

			if (!U::file_exists(user_file))
			{
				outbound_queue.put(Message(client_map_ID, "User \"" + input[0] + "\" does not exist."));
				continue;
			}

			pugi::xml_document user_data_xml;
			user_data_xml.load_file(user_file.c_str());

			if (input[1] != user_data_xml
				.child(C::XML_USER_ACCOUNT.c_str())
				.attribute(C::XML_USER_PASSWORD.c_str())
				.as_string())
			{
				outbound_queue.put(Message(client_map_ID, "Incorrect password for user \"" + input[0] + "\"."));
				continue;
			}

			// the password was correct
			user_ID = input[0];
			break;
		}

		// wrong input length, loop
	}

	// finish other login/connection details
	{
		// add user to the lookup
		clients.set_map_socket(user_ID, client_map_ID);

		// generate an area map from the current player's perspective, send it to the newly connect map socket
		std::lock_guard<std::mutex> lock(game_state);
		if (const std::shared_ptr<PC> player = U::convert_to<PC>(actors.find(user_ID)->second))
		{
			outbound_queue.put(Message(client_map_ID, "\n\n" + player->generate_area_map(world, actors)));
		}
	}

	// loop in here forever
	for (;;)
	{
		outbound_queue.put(Message(client_map_ID, "\n\nThis is your overhead map client. Use your other client on port " + U::to_string(C::GAME_PORT_NUMBER) + " to play."));

		// execution pauses inside of recv() until the user sends data
		int data_read = recv(client_map_ID, input, 1024, 0);

		// check if reading the socket failed
		if (data_read == 0 || data_read == -1) // graceful disconnect, less graceful disconnect (respectively)
		{
			close_socket(client_map_ID); // close socket (platform-independent)
			clients.set_map_socket(user_ID, -1); // reset map socket
			return; // the client's personal map thread is destroyed
		}
	}
}

void Game::NPC_thread()
{
	// use booleans to toggle spawning of certain NPCs for debugging purposes

	// create three workers, each with a bodyguards
	if (false)
	{
		const std::vector<std::string> workers = { "Jeb", "Bill", "Bob" };
		const std::vector<std::string> bodyguards = { "Alpha", "Beta", "Gamma" };

		// create worker NPCs with bodyguards
		for (unsigned i = 0; i < std::min(workers.size(), bodyguards.size()); ++i)
		{
			std::lock_guard<std::mutex> lock(game_state);

			std::shared_ptr<Hostile_NPC_Worker> worker = std::make_shared<Hostile_NPC_Worker>(workers[i]);
			worker->login(world);
			actors.insert(make_pair(worker->name, worker));

			std::shared_ptr<Hostile_NPC_Bodyguard> bodyguard = std::make_shared<Hostile_NPC_Bodyguard>(bodyguards[i], workers[i]);
			bodyguard->login(world);
			actors.insert(make_pair(bodyguard->name, bodyguard));
		}
	}

	// add a corporal
	if (false)
	{
		std::lock_guard<std::mutex> lock(game_state);

		std::shared_ptr<Hostile_NPC_Corporal> hunter = std::make_shared<Hostile_NPC_Corporal>("Hunter");
		hunter->login(world);
		actors.insert(make_pair(hunter->name, hunter));
	}

	if (false)
	// add a bodyguard
	{
		std::lock_guard<std::mutex> lock(game_state);

		std::shared_ptr<Hostile_NPC_Bodyguard> guardian = std::make_shared<Hostile_NPC_Bodyguard>("Guardian", "Hunter");
		guardian->login(world);
		actors.insert(make_pair(guardian->name, guardian));
	}

	// add a bunch more corporals
	if (false)
	{
		for (char name = 'A'; name < 'z'; ++name) // test code
		{
			std::shared_ptr<Hostile_NPC_Corporal> corporal = std::make_shared<Hostile_NPC_Corporal>(std::string() + name);
			corporal->login(world);
			actors.insert(make_pair(corporal->name, corporal));
		}
	}

	std::this_thread::sleep_for(std::chrono::seconds(15)); // put a delay between server startup and NPCs' first action

	std::cout << std::endl;

	for (;;)
	{
		std::this_thread::sleep_for(std::chrono::seconds(1));

		std::cout << "NPC thread locking game_state...";
		std::lock_guard<std::mutex> lock(game_state);
		std::cout << " game_state locked.\n";

		for (auto & actor : actors) // for each actor
		{
			if (std::shared_ptr<NPC> npc = U::convert_to<NPC>(actor.second)) // if the actor is an NPC
			{
				std::cout << "Calling NPC::update() on " << actor.first << "...";

				const Update_Messages update_messages = npc->update(world, actors); // call update, passing in the world and actors

				std::cout << " (located at " << npc->x << ", " << npc->y << ") ";

				std::cout << " done.\nGenerating outbound messages...";

				generate_outbound_messages(npc->name, update_messages);

				std::cout << " done.\n";
			}
		}
	}
}

void Game::outbound_thread()
{
	for (;;)
	{
		const Message message = outbound_queue.get(); // (blocking)

		// dispatch data to the user (because we're using TCP, data is lossless unless total failure occurs)
		send(message.user_socket_ID, message.data.c_str(), message.data.size(), 0);
	}
}

// helper functions

void Game::close_socket(const SOCKET socket)
{
#ifdef WIN32
	closesocket(socket);
#else
	close(socket);
#endif
}

std::string Game::login_or_signup(const SOCKET client_ID)
{
	// return the user_ID of a user after they log in or sign up

	// allocate a buffer on the stack to store incoming messages
	char input[1024];

	for (;;) // return after the user logs in or creates an account
	{
		const std::string login_instructions =
			"Type \"username\" \"password\" to log in.\n"
			"Type \"username\" \"password\" \"repeat password\" to create an account.\n"
			"*** Password encryption has not yet been implemented. ***";

		// set instructions
		outbound_queue.put(Message(client_ID, login_instructions));

		// execution pauses inside of recv() until the user sends data (one of these for each user in their own thread)
		int data_read = recv(client_ID, input, 1024, 0);

		// check if reading the socket failed
		if (data_read == 0 || data_read == -1) // graceful disconnect, less graceful disconnect (respectively)
		{
			close_socket(client_ID); // close socket (platform-independent wrapper)
			return ""; // the user lost connection before logging in
		}

		// convert user input to parsed vector of words
		std::stringstream user_input;
		for (int i = 0; i < data_read; ++i)
			user_input << input[i];
		const std::istream_iterator<std::string> begin(user_input);
		const std::vector<std::string> input(begin, std::istream_iterator<std::string>());

		if (input.size() == 3) // new account
		{
			const std::string user_file = C::user_data_directory + "/" + input[0] + ".xml";

			// check if the username is taken
			if (U::file_exists(user_file))
			{
				outbound_queue.put(Message(client_ID, "User \"" + input[0] + "\" already exists."));
				continue;
			}

			// check if the passwords match
			if (input[1] != input[2])
			{
				outbound_queue.put(Message(client_ID, "Passwords don't match."));
				continue;
			}

			// the user's file does not exist yet, create it using the username and password

			pugi::xml_document user_data_xml;
			user_data_xml
				.append_child(C::XML_USER_ACCOUNT.c_str())
				.append_attribute(C::XML_USER_PASSWORD.c_str())
				.set_value(input[1].c_str());

			user_data_xml.save_file(user_file.c_str());

			return input[0]; // account created
		}
		else if (input.size() == 2) // returning user
		{
			const std::string user_file = C::user_data_directory + "/" + input[0] + ".xml";

			if (!U::file_exists(user_file))
			{
				outbound_queue.put(Message(client_ID, "User \"" + input[0] + "\" does not exist.\n"));
				continue;
			}

			// check if the user is already logged in
			{ // temporary scope to delete mutex lock
				std::lock_guard<std::mutex> lock(game_state);
				if (actors.find(input[0]) != actors.cend())
				{
					outbound_queue.put(Message(client_ID, "User \"" + input[0] + "\" already logged in.\n"));
					continue; // try again
				}
			}

			pugi::xml_document user_data_xml;
			user_data_xml.load_file(user_file.c_str());

			if (input[1] != user_data_xml
				.child(C::XML_USER_ACCOUNT.c_str())
				.attribute(C::XML_USER_PASSWORD.c_str())
				.as_string())
			{
				outbound_queue.put(Message(client_ID, "Incorrect password for user \"" + input[0] + "\".\n"));
				continue;
			}

			// the password was correct
			return input[0]; // the username of the user that just logged in or signed up
		}

		// wrong input length, loop
	}
}

void Game::generate_outbound_messages(const std::string & user_ID, const Update_Messages & update_messages)
{
	// Make sure the calling function has a lock on the actors_mutex, because this function does not acquire it.

	// create a stringstream to assemble the return message
	std::stringstream action_result;

	const std::shared_ptr<Character> character = actors.find(user_ID)->second;

	// save the player's coordinates in case a map update is required
	const int player_x = character->x;
	const int player_y = character->y;

	// add the update message to the end of the outbound message
	action_result << update_messages.to_user;

	// if a map update is required for all players within view distance
	if (update_messages.map_update_required)
	{
		// for each room within view distance
		for (int cx = player_x - (int)C::VIEW_DISTANCE; cx <= player_x + (int)C::VIEW_DISTANCE; ++cx)
		{
			for (int cy = player_y - (int)C::VIEW_DISTANCE; cy <= player_y + (int)C::VIEW_DISTANCE; ++cy)
			{
				// skip if the room is out of bounds
				if (!U::bounds_check(cx, cy, C::GROUND_INDEX)) continue;

				// get a list of the users in the room
				const std::vector<std::string> users_in_range = world.room_at(cx, cy, C::GROUND_INDEX)->get_actor_ids();
				// for each user in the room
				for (const std::string & user : users_in_range)
				{
					// if the user is a player character
					if (const std::shared_ptr<PC> player = U::convert_to<PC>(actors.find(user)->second))
					{
						// get the player's map socket
						SOCKET outbound_socket = clients.get_map_socket(user);
						// if the player does not have a map socket, send the map to the player's main socket
						if (outbound_socket == -1) outbound_socket = clients.get_socket(user);

						// generate an area map from the current player's perspective, send it to the correct socket
						outbound_queue.put(Message(outbound_socket, "\n\n" + player->generate_area_map(world, actors)));
					}
				}
			}
		}
	}

	// if the update requires a map update for one or more players
	if (update_messages.additional_map_update_users != nullptr)
	{
		// for each player that requires an update
		for (auto player_it = (*update_messages.additional_map_update_users).cbegin();
		player_it != (*update_messages.additional_map_update_users).cend(); ++player_it)
		{
			// if the referenced character is a player character
			if (const std::shared_ptr<PC> player = U::convert_to<PC>(actors.find(*player_it)->second))
			{
				// get the player's map socket
				SOCKET outbound_socket = clients.get_map_socket(*player_it);
				// if the player does not have a map socket, send the map to the player's main socket
				if (outbound_socket == -1) outbound_socket = clients.get_socket(*player_it);

				// generate an area map from the current player's perspective, send it to the correct socket
				outbound_queue.put(Message(outbound_socket, player->generate_area_map(world, actors)));
			}
		}
	}

	// if the user that made the action is a human
	if (U::is<PC>(character))
	{
		// create an outbound message to the client in question
		const SOCKET socket_ID = clients.get_socket(user_ID);
		if (socket_ID != -1) outbound_queue.put(Message(socket_ID, user_ID + ": " + action_result.str()));
	}

	// if a message needs to be sent to all other player characters in the room
	if (update_messages.to_room != nullptr)
	{
		// get a list of all players in the room
		const std::vector<std::string> area_actor_ids = world.room_at(character->x, character->y, character->z)->get_actor_ids();

		for (const std::string & actor_id : area_actor_ids) // for each player in the room
		{
			// if the player is not "self" and the player is a human
			if (actor_id != user_ID && U::is<PC>(actors.find(actor_id)->second))
			{
				// send the room update to the player
				outbound_queue.put(Message(clients.get_socket(actor_id), *update_messages.to_room));
			}
		}
	}

	// if a custom message needs to be sent to a specific user
	if (update_messages.custom_message)
	{
		// add the message to the queue
		outbound_queue.put(Message(clients.get_socket(update_messages.custom_message->first), update_messages.custom_message->second));
	}
}
