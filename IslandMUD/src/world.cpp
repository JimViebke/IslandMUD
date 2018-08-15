/* Jim Viebke
May 15 2015 */

#include <iomanip>
#include <memory>

#include "world.h"
#include "parse.h"

World::World()
{
	create_world_container();
	load_or_generate_terrain_and_mineral_maps();

	// create a thread to save data to the disk in the background, off of the main thread
	background_room_unloading_thread = std::thread(&World::unload_rooms_to_disk, this);
}

World::~World()
{
	// if the World object is falling out of scope, assume the server is in shutdown

	// empty the unload queue
	{
		std::unique_lock<std::recursive_mutex> lock(unload_queue_mutex);

		// unload every room in the unload queue
		while (unload_queue.size() > 0)
		{
			// pull the room out of the list
			std::unique_ptr<Room> room = std::move(unload_queue.front());
			// clear the nullptr at the beginning of the queue
			unload_queue.pop_front();
			// save the room to disk
			unload_room(room);
		}
	}

	// shutdown the background thread
	std::cout << "World object waiting for internal background thread to exit...";
	unload_queue_cv.notify_one();
	background_room_unloading_thread.join();
	std::cout << " done\n";
}

// access a room using its coordinates
std::unique_ptr<Room>::pointer World::room_at(const Coordinate & coordinate)
{
	return (world.data() + coordinate.get_hash())->get();
}
const std::unique_ptr<Room>::pointer World::room_at(const Coordinate & coordinate) const
{
	return (world.data() + coordinate.get_hash())->get();
}
std::unique_ptr<Room> & World::reference_room_at(const Coordinate & coordinate)
{
	return *(world.data() + coordinate.get_hash());
}

// debugging
unsigned World::count_loaded_rooms() const
{
	/* This function is only used for debugging/development, to verify rooms are loading in and out as required.

	If only one player is playing (IE, the dev) there should be (2d+1)^2 rooms loaded, where d == view distance.
	For a view distance of 5, (5+1+5)*(5+1+5) (121) rooms should be in memory. */

	unsigned loaded_rooms = 0;
	for (int x = 0; x < C::WORLD_X_DIMENSION; ++x)
	{
		for (int y = 0; y < C::WORLD_Y_DIMENSION; ++y)
		{
			if (room_at(Coordinate(x, y)) != nullptr)
			{
				++loaded_rooms;
			}
		}
	}
	return loaded_rooms;
}

// load rooms around a player spawning in
void World::load_view_radius_around(const Coordinate & coordinate, const std::string & character_ID)
{
	const int x = coordinate.get_x(), y = coordinate.get_y();

	// current x ranges from x-view to x+view
	for (int cx = x - (int)C::VIEW_DISTANCE; cx <= x + (int)C::VIEW_DISTANCE; ++cx)
	{
		// current y ranges from y-view to y+view
		for (int cy = y - (int)C::VIEW_DISTANCE; cy <= y + (int)C::VIEW_DISTANCE; ++cy)
		{
			// create a temporary coordinate to the room
			const Coordinate current(cx, cy);

			// if the coordinates are not within world bounds
			if (!current.is_valid())
			{
				continue; // go to next coordinate
			}

			// if the room is not already in memory
			if (room_at(current) == nullptr)
			{
				// move the room from disk to world
				load_room_to_world(current);
			}

			// whoever loaded this can see it
			room_at(current)->add_viewing_actor(character_ID);
		}
	}
}

// loading and unloading rooms at the edge of vision
void World::remove_viewer_and_attempt_unload(const Coordinate & coordinate, const std::string & viewer_ID)
{
	// if the referenced room is out of bounds or not loaded
	if (!coordinate.is_valid() || room_at(coordinate) == nullptr) return; // nothing to remove or unload

	// remove the viewer from the room's viewing list
	room_at(coordinate)->remove_viewing_actor(viewer_ID);

	// if there is no one in the room or able to see it, move the room into the unload queue
	if (room_at(coordinate)->is_unloadable())
	{
		// lock the queue
		std::unique_lock<std::recursive_mutex> lock(unload_queue_mutex);
		// move the room into the queue, leaving the unique_ptr as nullptr
		unload_queue.push_back(std::move(reference_room_at(coordinate)));
		// wake the unload thread
		unload_queue_cv.notify_one();
	}
}
void World::remove_viewer_and_attempt_unload(const std::vector<Coordinate> & coordinates, const std::string & viewer_ID)
{
	// Lock the queue. Using this function offers an optomization to the calling thread because it only locks the unload queue
	// once for many writes into the queue.
	std::unique_lock<std::recursive_mutex> lock(unload_queue_mutex);

	for (const auto & coordinate : coordinates)
		remove_viewer_and_attempt_unload(coordinate, viewer_ID);
}

// unloading of all rooms in view distance (for logging out or dying)
void World::attempt_unload_radius(const Coordinate & coordinate, const std::string & player_ID)
{
	// remove the player from the room
	if (coordinate.is_valid())
		room_at(coordinate)->remove_actor(player_ID);

	const int x = coordinate.get_x(), y = coordinate.get_y();

	// for each room within the player's view distance
	for (int cx = x - C::VIEW_DISTANCE; cx <= x + C::VIEW_DISTANCE; ++cx)
	{
		for (int cy = y - C::VIEW_DISTANCE; cy <= y + C::VIEW_DISTANCE; ++cy)
		{
			const Coordinate current(cx, cy);

			// if the room is in bounds
			if (current.is_valid())
			{
				// remove the player as a viewer and attempt to move the room to disk
				remove_viewer_and_attempt_unload(current, player_ID);
			}
		}
	}
}

// test if a room can be removed from memory
bool World::is_unloadable(const Coordinate & coordinate) const
{
	return room_at(coordinate)->is_unloadable();
}

// room information
bool World::room_has_surface(const Coordinate & coordinate, const std::string & direction_ID) const
{
	// if the room is outside of bounds
	if (!coordinate.is_valid()) return false;

	// if the room is not loaded
	if (room_at(coordinate) == nullptr) return false;

	// test if the passed direction_ID exists as a wall
	return room_at(coordinate)->has_surface(direction_ID);
}



/* Private member functions */



void World::create_world_container()
{
	std::cout << "Creating world container...\n";

	world = std::vector<std::unique_ptr<Room>>(C::WORLD_X_DIMENSION * C::WORLD_Y_DIMENSION);
}
void World::load_or_generate_terrain_and_mineral_maps()
{
	// terrain and mineral maps are only used to generate rooms that do not already exist on disk

	std::cout << "Loading world terrain and mineral maps...\n";

	// generate a new world terrain map if needed
	if (load_existing_world_terrain())
	{
		std::cout << "Loaded existing world terrain map...\n";
	}
	else
	{
		// create the generator object
		Generator gen("world terrain map");

		// generate a biome map
		std::vector<char> biome_map = gen.generate_biome_map(C::LAND_CHAR, C::FOREST_CHAR, 3, 1, 25); // hardcoding a bit here
		gen.to_file(biome_map, 25 /* hardcoding, sorry */, gen.get_generated_terrain_dir() + "/biome_map.txt");

		// use the biome map to generate static in a full size map
		std::vector<char> world_map = gen.generate_static_using_biome_map(biome_map, 25, C::LAND_CHAR, C::FOREST_CHAR); // hardcoding again
		gen.to_file(world_map, C::WORLD_X_DIMENSION, gen.get_generated_terrain_dir() + "/static.txt");

		gen.game_of_life(world_map, 5, C::LAND_CHAR, C::FOREST_CHAR); gen.save_intermediate_map(world_map, C::WORLD_X_DIMENSION);
		gen.fill(world_map, 2, C::LAND_CHAR, C::FOREST_CHAR);  gen.save_intermediate_map(world_map, C::WORLD_X_DIMENSION);
		gen.clean(world_map, 3, C::LAND_CHAR, C::FOREST_CHAR); gen.save_intermediate_map(world_map, C::WORLD_X_DIMENSION);
		gen.fill(world_map, 4, C::LAND_CHAR, C::FOREST_CHAR);  gen.save_intermediate_map(world_map, C::WORLD_X_DIMENSION); // this is the same as fill(12), but each call has a seperate printout this way
		gen.fill(world_map, 4, C::LAND_CHAR, C::FOREST_CHAR);  gen.save_intermediate_map(world_map, C::WORLD_X_DIMENSION);
		gen.fill(world_map, 4, C::LAND_CHAR, C::FOREST_CHAR);  gen.save_intermediate_map(world_map, C::WORLD_X_DIMENSION);

		// save the final terrain to disk
		gen.to_file(world_map, C::WORLD_X_DIMENSION, C::world_terrain_file_location);

		this->terrain = std::make_unique<std::vector<char>>(world_map);
	}

	// generate a new iron ore mineral map if needed
	if (load_existing_iron_deposit_map())
	{
		std::cout << "Loaded existing iron ore mineral map...\n";
	}
	else
	{
		// create the generator object
		Generator gen("iron ore mineral map");

		// generate a biome map
		std::vector<char> biome_map = gen.generate_biome_map(C::LAND_CHAR, C::GENERIC_MINERAL_CHAR, 1, 19, 25); // hardcoding a bit here

		// use the biome map to generate static in a full size map
		std::vector<char> mineral_map = gen.generate_static_using_biome_map(biome_map, 25, C::LAND_CHAR, C::GENERIC_MINERAL_CHAR); // hardcoding again

		gen.game_of_life(mineral_map, 5, C::LAND_CHAR, C::GENERIC_MINERAL_CHAR); gen.save_intermediate_map(mineral_map, C::WORLD_X_DIMENSION);
		gen.fill(mineral_map, 2, C::LAND_CHAR, C::GENERIC_MINERAL_CHAR);  gen.save_intermediate_map(mineral_map, C::WORLD_X_DIMENSION);
		gen.clean(mineral_map, 3, C::LAND_CHAR, C::GENERIC_MINERAL_CHAR); gen.save_intermediate_map(mineral_map, C::WORLD_X_DIMENSION);
		gen.fill(mineral_map, 4, C::LAND_CHAR, C::GENERIC_MINERAL_CHAR);  gen.save_intermediate_map(mineral_map, C::WORLD_X_DIMENSION); // this is the same as fill(12), but each call has a seperate printout this way
		gen.fill(mineral_map, 4, C::LAND_CHAR, C::GENERIC_MINERAL_CHAR);  gen.save_intermediate_map(mineral_map, C::WORLD_X_DIMENSION);
		gen.fill(mineral_map, 4, C::LAND_CHAR, C::GENERIC_MINERAL_CHAR);  gen.save_intermediate_map(mineral_map, C::WORLD_X_DIMENSION);

		// save the mineral map to disk
		gen.to_file(mineral_map, C::WORLD_X_DIMENSION, C::iron_deposit_map_file_location);

		this->iron_deposit_map = std::make_unique<std::vector<char>>(mineral_map);
	}

	// generate a new limestone mineral map if needed
	if (load_existing_limestone_deposit_map())
	{
		std::cout << "Loaded existing limestone mineral map...\n";
	}
	else
	{
		// create the generator object
		Generator gen("limestone mineral map");

		// generate a biome map
		std::vector<char> biome_map = gen.generate_biome_map(C::LAND_CHAR, C::GENERIC_MINERAL_CHAR, 1, 39, 25); // hardcoding a bit here

		// use the biome map to generate static in a full size map
		std::vector<char> mineral_map = gen.generate_static_using_biome_map(biome_map, 25, C::LAND_CHAR, C::GENERIC_MINERAL_CHAR); // hardcoding again

		gen.game_of_life(mineral_map, 5, C::LAND_CHAR, C::GENERIC_MINERAL_CHAR); gen.save_intermediate_map(mineral_map, C::WORLD_X_DIMENSION);
		gen.fill(mineral_map, 2, C::LAND_CHAR, C::GENERIC_MINERAL_CHAR);  gen.save_intermediate_map(mineral_map, C::WORLD_X_DIMENSION);
		gen.clean(mineral_map, 3, C::LAND_CHAR, C::GENERIC_MINERAL_CHAR); gen.save_intermediate_map(mineral_map, C::WORLD_X_DIMENSION);
		gen.fill(mineral_map, 4, C::LAND_CHAR, C::GENERIC_MINERAL_CHAR);  gen.save_intermediate_map(mineral_map, C::WORLD_X_DIMENSION); // this is the same as fill(12), but each call has a seperate printout this way
		gen.fill(mineral_map, 4, C::LAND_CHAR, C::GENERIC_MINERAL_CHAR);  gen.save_intermediate_map(mineral_map, C::WORLD_X_DIMENSION);
		gen.fill(mineral_map, 4, C::LAND_CHAR, C::GENERIC_MINERAL_CHAR);  gen.save_intermediate_map(mineral_map, C::WORLD_X_DIMENSION);

		// save the mineral map to disk
		gen.to_file(mineral_map, C::WORLD_X_DIMENSION, C::limestone_deposit_map_file_location);

		this->limestone_deposit_map = std::make_unique<std::vector<char>>(mineral_map);
	}
}

// three functions for loading and verifying the world map and the two mineral maps

bool World::load_existing_world_terrain()
{
	// load the contents of the terrain file, if it exists

	terrain = std::make_unique<std::vector<char>>();
	terrain->reserve(C::WORLD_X_DIMENSION * C::WORLD_Y_DIMENSION);

	if (U::file_exists(C::world_terrain_file_location))
	{
		std::fstream terrain_file;
		terrain_file.open(C::world_terrain_file_location);
		std::string row;
		while (getline(terrain_file, row)) // for each row
		{
			if (row.length() == C::WORLD_X_DIMENSION) // if the row is not empty
			{
				terrain->insert(terrain->end(), row.begin(), row.end()); // copy the contents of the row into an anonymous vector
			}
		}
	}

	// test if the loaded terrain is the correct dimensions
	return terrain->size() == C::WORLD_X_DIMENSION * C::WORLD_Y_DIMENSION;
}
bool World::load_existing_iron_deposit_map()
{
	iron_deposit_map = std::make_unique<std::vector<char>>();
	iron_deposit_map->reserve(C::WORLD_X_DIMENSION * C::WORLD_Y_DIMENSION);

	if (U::file_exists(C::iron_deposit_map_file_location))
	{
		std::fstream terrain_file;
		terrain_file.open(C::iron_deposit_map_file_location);
		std::string row;
		while (getline(terrain_file, row)) // for each row
		{
			if (row.length() == C::WORLD_X_DIMENSION) // if the row is not empty
			{
				iron_deposit_map->insert(iron_deposit_map->end(), row.begin(), row.end()); // copy the contents of the row into an anonymous vector
			}
		}
	}

	// test if the loaded terrain is the correct dimensions
	return iron_deposit_map->size() == C::WORLD_X_DIMENSION * C::WORLD_Y_DIMENSION;
}
bool World::load_existing_limestone_deposit_map()
{
	limestone_deposit_map = std::make_unique<std::vector<char>>();
	limestone_deposit_map->reserve(C::WORLD_X_DIMENSION * C::WORLD_Y_DIMENSION);

	if (U::file_exists(C::limestone_deposit_map_file_location))
	{
		std::fstream terrain_file;
		terrain_file.open(C::limestone_deposit_map_file_location);
		std::string row;
		while (getline(terrain_file, row)) // for each row
		{
			if (row.length() == C::WORLD_X_DIMENSION) // if the row is not empty
			{
				limestone_deposit_map->insert(limestone_deposit_map->end(), row.begin(), row.end()); // copy the contents of the row into an anonymous vector
			}
		}
	}

	// test if the loaded terrain is the correct dimensions
	return limestone_deposit_map->size() == C::WORLD_X_DIMENSION * C::WORLD_Y_DIMENSION;
}

// a room at x,y does not exist on the disk; create it and add it to the world
void World::generate_room_at(const Coordinate & coordinate)
{
	// ensure the folder exists
	std::string path = C::room_directory + "/" + U::to_string(coordinate.get_x());
	U::create_path_if_not_exists(path);

	// extend the path to include the file
	path += "/" + U::to_string(coordinate.get_x()) + "-" + U::to_string(coordinate.get_y()) + ".xml";

	// create an XML document to store the room
	pugi::xml_document room_document;

	// if the file exists
	if (U::file_exists(path))
	{
		// load the saved room to XML
		load_room_to_XML(coordinate, room_document);

		// load the XML to the game world
		add_room_to_world(room_document, coordinate);
	}
	else // the room does not exist on the disk
	{
		// create the room and add it to the world
		reference_room_at(coordinate) = create_room(coordinate); // this should invoke the funtionality of move() automatically

		// this is empty, but save it to the disk
		room_document.save_file(path.c_str());
	}
}

// load the room x,y to an xml_document
void World::load_room_to_XML(const Coordinate & coordinate, pugi::xml_document & room)
{
	// convert integers to strings, since they'll be used multiple times
	const std::string x = U::to_string(coordinate.get_x());
	const std::string y = U::to_string(coordinate.get_y());

	// populate the document using the file for the room at (x, y)
	room.load_file((C::room_directory + "/" + x + "/" + x + "-" + y + ".xml").c_str());
}

// build a room given an XML node, add to world at x,y
void World::add_room_to_world(pugi::xml_node & room_document, const Coordinate & coordinate)
{
	// create an empty room
	std::unique_ptr<Room> room(std::make_unique<Room>(coordinate));

	// get the root node that represents the room
	const pugi::xml_node room_node = room_document.child(C::XML_ROOM.c_str());

	// set whether or not the room is water (off-island or river/lake)
	room->set_water_status(room_node.attribute(C::XML_IS_WATER.c_str()).as_bool());

	// extract the items node
	const pugi::xml_node items_node = room_node.child(C::XML_ITEMS.c_str());

	// for each item under the item node
	for (const pugi::xml_node & item_node : items_node)
	{
		// use the item ID to make a new item and add it to the room

		// create the item using the value of the item ID attribute
		std::shared_ptr<Item> item = Craft::make(item_node.attribute(C::XML_ITEM_ID.c_str()).as_string());

		// set the item's health
		item->set_health(item_node.attribute(C::XML_ITEM_HEALTH.c_str()).as_int());

		// if the item is a stackable type
		if (std::shared_ptr<Stackable> stackable = U::convert_to<Stackable>(item))
		{
			// set the amount as the value of the "count" attribute
			stackable->amount = std::max(1u, item_node.attribute(C::XML_ITEM_COUNT.c_str()).as_uint());
		}

		// add the item to the room
		room->insert(item);
	}

	// for each surface in the room
	for (const pugi::xml_node & surface : room_node.children(C::XML_SURFACE.c_str()))
	{
		// extract the attribute containing the health/integrity of the surface
		const pugi::xml_attribute health_attribute = surface.attribute(C::XML_SURFACE_HEALTH.c_str());

		// construct a new surface to add to the room
		room->add_surface(
			U::to_surface(std::string(surface.child(C::XML_SURFACE_DIRECTION.c_str()).child_value())),
			surface.child(C::XML_SURFACE_MATERIAL.c_str()).child_value(),

			(!health_attribute.empty()) // if the health attribute exists
			? health_attribute.as_int() // set the surface health to the attribute's value
			: C::MAX_SURFACE_HEALTH // else set the room to full health
			);

		// select the door node
		const pugi::xml_node door_node = surface.child(C::XML_DOOR.c_str());

		// if the door node exists
		if (!door_node.empty())
		{
			// extract values
			const int health = door_node.attribute(C::XML_DOOR_HEALTH.c_str()).as_int();
			const std::string material_ID = door_node.attribute(C::XML_DOOR_MATERIAL.c_str()).value();
			const std::string faction_ID = door_node.attribute(C::XML_DOOR_FACTION.c_str()).value();

			// add a door to the surface
			room->add_door(
				U::to_surface(std::string(surface.child(C::XML_SURFACE_DIRECTION.c_str()).child_value())),
				health, material_ID, faction_ID);
		}
	}

	// extract the chest node
	const pugi::xml_node chest_node = room_node.child(C::XML_CHEST.c_str());

	// if the extracted chest node exists
	if (!chest_node.empty())
	{
		// create the chest's internal structure for equipment
		std::multimap<std::string, std::shared_ptr<Item>> chest_contents = {};

		// for each item node (children of the chest node)
		for (const pugi::xml_node & item_node : chest_node.child(C::XML_ITEM.c_str()).children())
		{
			// create the item
			std::shared_ptr<Item> item = U::convert_to<Item>(Craft::make(item_node.name()));

			// set the item's health
			item->set_health(item_node.attribute(C::XML_ITEM_HEALTH.c_str()).as_int());

			// if the item is a stackable type
			if (std::shared_ptr<Stackable> stackable = U::convert_to<Stackable>(item))
			{
				// set the amount as the value of the "count" attribute
				stackable->amount = std::max(1u, item_node.attribute(C::XML_ITEM_COUNT.c_str()).as_uint());
			}

			// add the item to the temporary multimap
			chest_contents.insert(make_pair(item->get_name(), item));
		}

		// create an anonymous chest object and add it to the room
		room->set_chest(std::make_shared<Chest>(Chest(
			chest_node.attribute(C::XML_CHEST_FACTION_ID.c_str()).as_string(), // the chest's faction ID
			chest_node.attribute(C::XML_CHEST_HEALTH.c_str()).as_int(), // the chest's health
			chest_contents) // the chest's contents
			));
	}

	// extract the table node
	const pugi::xml_node table_node = room_node.child(C::XML_TABLE.c_str());

	// if the extracted table node exists
	if (!table_node.empty())
	{
		// create the table's internal structure for equipment
		std::multimap<std::string, std::shared_ptr<Item>> table_contents = {};

		// for each item node (children of the table node)
		for (const pugi::xml_node & item_node : table_node.child(C::XML_ITEM.c_str()).children())
		{
			// create the item
			std::shared_ptr<Item> item = U::convert_to<Item>(Craft::make(item_node.name()));

			// set the item's health
			item->set_health(item_node.attribute(C::XML_ITEM_HEALTH.c_str()).as_int());

			// if the item is a stackable type
			if (std::shared_ptr<Stackable> stackable = U::convert_to<Stackable>(item))
			{
				// set the amount as the value of the "count" attribute
				stackable->amount = std::max(1u, item_node.attribute(C::XML_ITEM_COUNT.c_str()).as_uint());
			}

			// add the item to the temporary multimap
			table_contents.insert(make_pair(item->get_name(), item));
		}

		// create an anonymous table object and add it to the room
		room->set_table(std::make_shared<Table>(Table(
			table_node.attribute(C::XML_TABLE_HEALTH.c_str()).as_int(), // the table's health
			table_contents) // the table's contents
			));
	}

	// add room to world
	reference_room_at(coordinate) = std::move(room);
}

// move specific room into memory
void World::load_room_to_world(const Coordinate & coordinate)
{
	// If the room is already loaded, return. This should never happen.
	if (room_at(coordinate) != nullptr) return;

	// check if the room is in the unload queue
	{
		std::unique_lock<std::recursive_mutex> lock(unload_queue_mutex);
		// for each room in the queue
		for (auto it = unload_queue.begin(); it != unload_queue.end(); ++it)
		{
			// if the room we want to load is about to be unloaded
			if ((**it).get_coordinates() == coordinate) // dereference twice, because we have an iterator to a unique pointer
			{
				// move the room back to the world
				reference_room_at(coordinate) = std::move(*it);
				// erase the entry in the unload queue
				unload_queue.erase(it);
				// our work here is done; the room is loaded
				return;
			}
		}
	} // release mutex

	// check if the room is currently being written to the disk
	{
		std::unique_lock<std::mutex> lock(writing_to_disk_mutex);
		// if the room is being written to the disk
		while (writing_to_disk != nullptr && *writing_to_disk == coordinate) // important to verify that the pointer is non-null
			// sleep until the 
			writing_to_disk_cv.wait(lock);
	} // release the disk mutex

	// the room is not in the unload queue, either load it from the disk or create it fresh

	// get the path to the room's save file'
	const std::string str_x = U::to_string(coordinate.get_x());
	const std::string str_y = U::to_string(coordinate.get_y());
	const std::string path = C::room_directory + "/" + str_x + "/" + str_x + "-" + str_y + ".xml";

	// if the room's save file does not exist'
	if (!U::file_exists(path))
	{
		// generate the room and create the file
		generate_room_at(coordinate);
		return; // we're done
	}

	// the file exists, load it
	pugi::xml_document room_document;
	room_document.load_file(path.c_str());

	// load the room's data to the world
	add_room_to_world(room_document, coordinate);
}

// save the contents of a room to an XML file in memory
void World::save_room_to_XML(const std::unique_ptr<Room> & room, pugi::xml_document & room_document) const
{
	// clear any existing data
	room_document.reset();

	// create a root node to represent the room
	pugi::xml_node room_node = room_document.append_child(C::XML_ROOM.c_str());

	// add a boolean representing if the room is water (off-island or a lake/river)
	room_node.append_attribute(C::XML_IS_WATER.c_str()).set_value(room->is_water());

	// add a node to contain all item nodes
	pugi::xml_node items_node = room_node.append_child(C::XML_ITEMS.c_str());

	// for each item in the room
	const std::multimap<std::string, std::shared_ptr<Item>> room_item_contents = room->get_contents();
	for (std::multimap<std::string, std::shared_ptr<Item>>::const_iterator item_it = room_item_contents.cbegin();
	item_it != room_item_contents.cend(); ++item_it)
	{
		// create a node for the item, append it to the items node
		pugi::xml_node item_node = items_node.append_child(C::XML_ITEM.c_str());

		// append the name of the item: item_ID="limestone deposit"
		item_node.append_attribute(C::XML_ITEM_ID.c_str()).set_value(item_it->second->get_name().c_str());

		// append the item's health as an attribute
		item_node.append_attribute(C::XML_ITEM_HEALTH.c_str()).set_value(item_it->second->get_health());

		// if the item is a stackable type
		if (std::shared_ptr<Stackable> stackable = U::convert_to<Stackable>(item_it->second))
		{
			// append count="[amount]" to the item's item node
			item_node.append_attribute(C::XML_ITEM_COUNT.c_str()).set_value(stackable->amount);
		}
	}

	// for each side of the room
	C::surface surface = (C::surface)0;
	for (auto surface_it = room->get_room_sides().cbegin();
	surface_it != room->get_room_sides().cend(); ++surface_it, surface = C::surface((size_t)surface + 1))
	{
		if (!surface_it->has_value()) continue;

		// create a node for a surface
		pugi::xml_node surface_node = room_node.append_child(C::XML_SURFACE.c_str());

		// add an attribute to the surface containing the health of the surface (example: health="90")
		surface_node.append_attribute(C::XML_SURFACE_HEALTH.c_str()).set_value((*surface_it)->get_health());

		// create nodes for surface direction and surface material
		pugi::xml_node direction_node = surface_node.append_child(C::XML_SURFACE_DIRECTION.c_str());
		pugi::xml_node material_node = surface_node.append_child(C::XML_SURFACE_MATERIAL.c_str());

		// append the surface direction and material
		direction_node.append_child(pugi::node_pcdata).set_value(U::surface_to_string(surface).c_str()); // direction ID
		material_node.append_child(pugi::node_pcdata).set_value((*surface_it)->get_material_id().c_str()); // material ID

		// conditionally create a node for a door
		if ((*surface_it)->has_door())
		{
			// create a node to hold the door
			pugi::xml_node door_node = surface_node.append_child(C::XML_DOOR.c_str());

			// retrive the door
			std::shared_ptr<Door> door = (*surface_it)->get_door();

			// add three attributes for health, material, and faction

			// health="55"
			door_node.append_attribute(C::XML_DOOR_HEALTH.c_str()).set_value(door->get_health());
			// material="wood"
			door_node.append_attribute(C::XML_DOOR_MATERIAL.c_str()).set_value(door->get_material_ID().c_str());
			// faction="hostile_NPC"
			door_node.append_attribute(C::XML_DOOR_FACTION.c_str()).set_value(door->get_faction_ID().c_str());
		}
	}

	// if there is a chest in this room
	if (room->has_chest())
	{
		// create a chest node on the room node and name the chest node
		pugi::xml_node chest_node = room_node.append_child(C::XML_CHEST.c_str());
		chest_node.set_name(C::XML_CHEST.c_str());

		// extract the chest from the room
		const std::shared_ptr<Chest> chest = room->get_chest();

		// add an attribute for the chest's faction ID
		chest_node.append_attribute(C::XML_CHEST_FACTION_ID.c_str()).set_value(chest->get_faction_id().c_str());

		// add a health attribute to the chest node
		chest_node.append_attribute(C::XML_CHEST_HEALTH.c_str()).set_value(chest->get_health());

		// add equipment and material nodes to the chest node
		pugi::xml_node items_node = chest_node.append_child(C::XML_ITEM.c_str());

		// for each equipment item
		const std::multimap<std::string, std::shared_ptr<Item>> chest_contents = chest->get_contents(); // extract contents
		for (std::multimap<std::string, std::shared_ptr<Item>>::const_iterator item_it = chest_contents.cbegin();
		item_it != chest_contents.cend(); ++item_it)
		{
			pugi::xml_node item_node = items_node.append_child(item_it->second->get_name().c_str());

			// if the item is stackable
			if (std::shared_ptr<Stackable> stackable = U::convert_to<Stackable>(item_it->second))
			{
				// add count="[amount]" to the node's attributes
				item_node.append_attribute(C::XML_ITEM_COUNT.c_str()).set_value(stackable->amount);
			}

			// append a health attribute to the item node and set its value to the health of the item
			item_node.append_attribute(C::XML_ITEM_HEALTH.c_str()).set_value(item_it->second->get_health());
		}
	}

	// if there is a table in this room
	if (room->has_table())
	{
		// create a table node on the room node and name the table node
		pugi::xml_node table_node = room_node.append_child(C::XML_TABLE.c_str());
		table_node.set_name(C::XML_TABLE.c_str());

		// extract the table from the room
		const std::shared_ptr<Table> table = room->get_table();

		// add a health attribute to the table node
		table_node.append_attribute(C::XML_TABLE_HEALTH.c_str()).set_value(table->get_health());

		// add equipment and material nodes to the table node
		pugi::xml_node items_node = table_node.append_child(C::XML_ITEM.c_str());

		// for each equipment item
		const std::multimap<std::string, std::shared_ptr<Item>> table_contents = table->get_contents(); // extract contents
		for (std::multimap<std::string, std::shared_ptr<Item>>::const_iterator item_it = table_contents.cbegin();
		item_it != table_contents.cend(); ++item_it)
		{
			pugi::xml_node item_node = items_node.append_child(item_it->second->get_name().c_str());

			// if the item is stackable
			if (std::shared_ptr<Stackable> stackable = U::convert_to<Stackable>(item_it->second))
			{
				// add count="[amount]" to the node's attributes
				item_node.append_attribute(C::XML_ITEM_COUNT.c_str()).set_value(stackable->amount);
			}

			// append a health attribute to the item node and set its value to the health of the item
			item_node.append_attribute(C::XML_ITEM_HEALTH.c_str()).set_value(item_it->second->get_health());
		}
	}

}

// create a new empty room given its coordinates and the world terrain
std::unique_ptr<Room> World::create_room(const Coordinate & coordinate) const
{
	std::unique_ptr<Room> room = std::make_unique<Room>(coordinate);

	// if the terrain map indicates the room is forest
	if ((*terrain)[coordinate.get_hash()] == C::FOREST_CHAR)
	{
		room->insert(Craft::make(C::TREE_ID)); // add a tree
	}

	// if the terrain map indicates the room is water
	if ((*terrain)[coordinate.get_hash()] == C::WATER_CHAR)
	{
		room->set_water_status(true);
	}

	// if the iron ore map indicates the room contains an iron deposit
	if ((*iron_deposit_map)[coordinate.get_hash()] == C::GENERIC_MINERAL_CHAR)
	{
		room->insert(Craft::make(C::IRON_DEPOSIT_ID)); // add an iron deposit item
	}

	// if the limestone map indicates the room contains limestone
	if ((*limestone_deposit_map)[coordinate.get_hash()] == C::GENERIC_MINERAL_CHAR)
	{
		room->insert(Craft::make(C::LIMESTONE_DEPOSIT_ID)); // add a limestone item
	}

	return room;
}

// remove a room from memory
void World::erase_room_from_memory(const Coordinate & coordinate)
{
	(world.begin() + coordinate.get_hash())->reset();
}



// runs in a thread to unload rooms in the background, off of the main thread
void World::unload_rooms_to_disk()
{
	// This only unloads one room at a time in order to make sure that the main thread
	// can access the queue as quickly as possible. The main thread will always try to
	// "load" a room from the unload queue (if it exists) before resorting to loading
	// from the disk.

	// It is important to note that this thread deliberately unlocks the queue only after a room
	// has been saved to the disk. Otherwise, the main thread could try to load a room, correctly
	// notice that the room is not in the unload queue, and then errantly load it from the disk
	// while this thread is saving that same room to the disk. The fix is to keep the queue locked
	// while a room is being written to the disk. A better fix would be to block the main thread
	// from accessing the disk, instead of blocking the main thread from reading the queue.

	for (;;)
	{
		std::unique_ptr<Room> room;

		{
			std::unique_lock<std::recursive_mutex> lock(unload_queue_mutex);

			// wait if there are no rooms in the queue unload
			while (unload_queue.empty() && Server::is_running())
				unload_queue_cv.wait(lock);

			// this only clears the queue when the server is running
			// Upon server shutdown, the main thread adds all remaining rooms in the World to the queue,
			// and then unloads the queue.
			if (!Server::is_running()) return; // this thread now releases the unload queue mutex and dies

			// debug code
			//std::stringstream ss;
			//ss << "Unloading a room from the unload queue. Current size: " << unload_queue.size() << std::endl;
			//std::cout << ss.str();

			// get the room
			room = std::move(unload_queue.front());

			// clear the nullptr at the beginning of the queue
			unload_queue.erase(unload_queue.begin());

			// before releasing the mutex, note which room is currently being unloaded
			std::unique_lock<std::mutex> lock_b(writing_to_disk_mutex);
			writing_to_disk = std::make_unique<Coordinate>(room->get_coordinates());
		} // release both mutexes (no need to call a .notify())

		// unload one room
		unload_room(room);

		{ // grab the mutex again, to update the currently_unloading_room indicator
			std::unique_lock<std::mutex> lock(writing_to_disk_mutex);
			writing_to_disk = nullptr;
			writing_to_disk_cv.notify_one(); // if the main thread was waiting for a room to be finished writing, wake it
		} // release the mutex

		// the room object is finally destroyed

	} // repeat until server shutdown
}

// move a passed room to disk
void World::unload_room(const std::unique_ptr<Room> & room)
{
	// Unloads passed room. Can be called even if the room doesn't exist in the world structure

	// ensure the path exists up to /x
	std::string path = (C::room_directory + "/" + U::to_string(room->get_coordinates().get_x()) + "/");
	U::create_path_if_not_exists(path);

	// ensure the path exists up to x/x-y.xml
	path += (U::to_string(room->get_coordinates().get_x()) + "-" + U::to_string(room->get_coordinates().get_y()) + ".xml");

	// load the room to XML
	pugi::xml_document room_document;
	load_room_to_XML(room->get_coordinates(), room_document); // Why is this being done? Why not overwrite? Try remove this and benchmark.

	// process the room into the XML
	save_room_to_XML(room, room_document);

	// save the XML document to disk
	room_document.save_file(path.c_str()); // returns an unused boolean indicated success or failure


}
