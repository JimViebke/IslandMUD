/* Jim Viebke
May 15 2015 */

#include <iomanip>

#include "world.h"
#include "character.h"
#include "npc_enemy.h"

void World::load()
{
	cout << "\nLoading world terrain...";

	vector<vector<char>> terrain;

	// load the contents of the terrain file, it it exists
	if (R::file_exists(C::world_terrain_file_location))
	{
		fstream terrain_file;
		terrain_file.open(C::world_terrain_file_location);
		string row;
		while (getline(terrain_file, row)) // for each row
		{
			if (row.length() > 1) // if the row is not empty
			{
				terrain.push_back(vector<char>(row.begin(), row.end())); // copy the contents of the row into an anonymous vector
			}
		}
	}

	// test if the loaded terrain is the correct dimensions
	bool terrain_loaded_from_file_good = false;
	if (terrain.size() == C::WORLD_X_DIMENSION)
	{
		terrain_loaded_from_file_good = true;
		for (unsigned i = 0; i < terrain.size(); ++i)
		{
			if (terrain[i].size() != C::WORLD_Y_DIMENSION)
			{
				terrain_loaded_from_file_good = false;
				break;
			}
		}
	}

	if (!terrain_loaded_from_file_good) // if the terrain needs to regenerated
	{
		// create the world generator object
		Generator gen;

		gen.generate_biome_map();
		gen.generate_static_using_biome_map();

		gen.game_of_life(5);
		gen.fill(2);
		gen.clean(3);
		gen.fill(4);
		gen.fill(4);
		gen.fill(4);

		// saves the final terrain to disk
		gen.save_terrain();

		terrain = gen.get_terrain();
	}

	// create/load the world's rooms using terrain
	create(terrain);
}

void World::create(const vector<vector<char>> & world_terrain)
{
	// world terrain is only used to generate rooms that do not already exist on disk

	cout << "\nCreating world object...";

	{ // temporary scope to delete temp structure later
		vector<vector<vector<shared_ptr<Room>>>> temp(C::WORLD_X_DIMENSION,
			vector<vector<shared_ptr<Room>>>(C::WORLD_Y_DIMENSION,
			vector<shared_ptr<Room>>(C::WORLD_Z_DIMENSION)));

		cout << "\nSaving world object...";
		world = temp;
	}

	cout << "\nCreating world...";

	int previous_percent = 0;
	const double start_time = (double)R::current_time_in_ms() / 1000.0;

	for (int x = 0; x < C::WORLD_X_DIMENSION; ++x) // for each row
	{
		// print completion percent and timing statistics
		int percent_processed = (x * 100 / C::WORLD_X_DIMENSION);
		if (percent_processed != previous_percent)
		{
			previous_percent = percent_processed;

			cout << fixed << setprecision(1)
				<< "\nProcessing rooms: " << percent_processed << "% (" << (((double)R::current_time_in_ms()/1000.0) - start_time) << " sec elapsed, ~"
				<<
				(((((R::current_time_in_ms() / 1000.0) - start_time)) / (double)previous_percent) * (100.0 - (double)previous_percent)) / 60.0
				<< " minutes remaining)";
		}

		// vector<vector<shared_ptr<Room>>> row; // create the empty row

		for (int y = 0; y < C::WORLD_Y_DIMENSION; ++y) // for each vertical "stack" in the row
		{
			// vector<shared_ptr<Room>> vertical_stack; // create an empty vertical "stack"
			// vertical_stack.reserve(C::WORLD_Z_DIMENSION); // size the stack
			// for (int z = 0; z < C::WORLD_Z_DIMENSION; ++z)
			// {
			// 	vertical_stack.push_back(shared_ptr<Room>(nullptr)); // add the correct number of "null" rooms to the vertical stack
			// }
			// row.push_back(vertical_stack); // add the empty vertical stack to the row

			// ensure the folder exists
			string z_stack_path = C::room_directory + "\\" + R::to_string(x);
			R::create_path_if_not_exists(z_stack_path);

			// extend the path to include the file
			z_stack_path += "\\" + R::to_string(x) + "-" + R::to_string(y) + ".xml";

			// if the file exists, make sure each room is in it
			if (R::file_exists(z_stack_path))
			{
				// create a flag to indicate if the z_stack was modified and has to be written back to disk
				bool z_stack_modified = false;

				// create an XML document to store the Z stack
				xml_document z_stack;

				// load the z-stack to the document
				load_vertical_rooms_to_XML(x, y, z_stack);

				// create and add rooms that don't exist
				// for (int z = 0; z < C::WORLD_Z_DIMENSION; ++z) // for each vertically-stacked room
				// {
				// select the specific room from the stack
				xml_node room_node = z_stack.child(("room-" + R::to_string(C::GROUND_INDEX)).c_str());

				// if the room is not in the stack
				if (!room_node)
				{
					// indicate that the stack will be changed and needs to be written to the disk
					z_stack_modified = true;

					// add the room to the z-stack
					this->add_room_to_z_stack(C::GROUND_INDEX, create_room(x, y, C::GROUND_INDEX, world_terrain), z_stack);
				}
				// }

				// if the stack was modified (rooms were not on disk, so were recently created)
				if (z_stack_modified)
				{
					// save the stack to disk
					z_stack.save_file(z_stack_path.c_str());
				}
			}
			else
			{
				// the file/z-stack does not exist, create ground level room to add to it

				// create an XML document to store the Z stack
				xml_document z_stack;

				// create and add ground level room
				this->add_room_to_z_stack(C::GROUND_INDEX, create_room(x, y, C::GROUND_INDEX, world_terrain), z_stack);

				// save the stack to disk
				z_stack.save_file(z_stack_path.c_str());
			}

		}
		// world.push_back(row); // add the row to the world
	}

} // end load world

// return shared_ptr to a room at a location
shared_ptr<Room> World::room_at(const int & x, const int & y, const int & z) const
{
	return world.at(x).at(y).at(z);
}
shared_ptr<Room> & World::room_at(const int & x, const int & y, const int & z)
{
	return world.at(x).at(y).at(z);
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
			for (int z = 0; z < C::WORLD_Z_DIMENSION; ++z)
			{
				if (room_at(x, y, z) != nullptr)
				{
					++loaded_rooms;
				}
			}
		}
	}
	return loaded_rooms;
}

// load rooms around a player spawning in
void World::load_view_radius_around(const int & x, const int & y, const string & character_ID)
{
	// current x ranges from x-view to x+view
	for (int cx = x - (int)C::VIEW_DISTANCE; cx <= x + (int)C::VIEW_DISTANCE; ++cx)
	{
		// current y ranges from y-view to y+view
		for (int cy = y - (int)C::VIEW_DISTANCE; cy <= y + (int)C::VIEW_DISTANCE; ++cy)
		{
			// if the coordinates are not within world bounds
			if (!R::bounds_check(cx, cy))
			{
				continue; // go to next coordinate
			}

			// if the room is not already in memory
			if (room_at(cx, cy, C::GROUND_INDEX) == nullptr)
			{
				// move the room from disk to world
				load_room_to_world(cx, cy, C::GROUND_INDEX);
			}

			// whoever loaded this can see it
			room_at(cx, cy, C::GROUND_INDEX)->add_viewing_actor(character_ID);
		}
	}
}

// loading and unloading rooms at the edge of vision
void World::remove_viewer_and_attempt_unload(const int & x, const int & y, const int & z, const string & viewer_ID)
{
	// if the referenced room is out of bounds
	if (!R::bounds_check(x, y, z))
	{
		return; // nothing to remove or unload
	}

	// if the referenced room is not loaded
	if (room_at(x, y, z) == nullptr)
	{
		return; // nothing to remove or unload
	}

	// remove the viewer from the room's viewing list
	room_at(x, y, z)->remove_viewing_actor(viewer_ID);

	// if there is no one in the room or able to see it, remove the room from memory
	if (room_at(x, y, z)->is_unloadable())
	{
		unload_room(x, y, z);
	}
}

// test if a room can be removed from memory
bool World::is_unloadable(const int & x, const int & y, const int & z) const
{
	return room_at(x, y, z)->is_unloadable();
}

// move a room from world to disk
void World::unload_room(const int & x, const int & y, const int & z)
{
	// pass the coordinates and a shared_ptr to the room
	unload_room(x, y, z, room_at(x, y, z));

	// set the shared_ptr at x,y,z to null
	room_at(x, y, z) = nullptr;
}



/* Private member functions */



// load in all rooms at x,y to an xml_document
void World::load_vertical_rooms_to_XML(const int & ix, const int & iy, xml_document & vertical_rooms)
{
	// convert integers to strings, since they'll be used multiple times
	string x = R::to_string(ix);
	string y = R::to_string(iy);

	// populate the document using the file for the vertical stack of rooms at x,y
	vertical_rooms.load_file((C::room_directory + "\\" + x + "\\" + x + "-" + y + ".xml").c_str());
}

// build a room from XML at location z, add to world at x,y,z
void World::add_room_to_world(const xml_document & z_stack, const int & x, const int & y, const int & z)
{
	// create an empty room
	shared_ptr<Room> room = make_shared<Room>();

	// select the specific room
	xml_node room_node = z_stack.child(("room-" + R::to_string(z)).c_str());

	// set whether or not the room is water (off-island or river/lake)
	room->set_water_status(room_node.attribute(C::XML_IS_WATER.c_str()).as_bool());

	// add a boolean representing if the room is water (off-island or a lake/river)
	room_node.append_attribute(C::XML_IS_WATER.c_str()).as_bool(room->is_water());

	// for each item in the room
	for (const xml_node & item : room_node.children(C::XML_ITEM.c_str()))
	{
		// use the item ID to make a new item and add it to the room
		room->add_item(Craft::make(item.child_value()));
	}

	// for each surface in the room
	for (const xml_node & surface : room_node.children(C::XML_SURFACE.c_str()))
	{
		// extract the attribute containing the health/integrity of the surface
		xml_attribute health_attribute = surface.attribute(C::XML_SURFACE_HEALTH.c_str());

		// construct a new surface to add to the room
		room->add_surface(
			surface.child(C::XML_SURFACE_DIRECTION.c_str()).child_value(),
			surface.child(C::XML_SURFACE_MATERIAL.c_str()).child_value(),

			(!health_attribute.empty()) // if the health attribute exists
			? health_attribute.as_int() // set the surface health to the attribute's value
			: C::MAX_SURFACE_HEALTH // else set the room to full health

			);

		// select the door node
		xml_node door_node = surface.child(C::XML_DOOR.c_str());

		// if the door node exists
		if (!door_node.empty())
		{
			// extract values
			int health = door_node.attribute(C::XML_DOOR_HEALTH.c_str()).as_int();
			string material_ID = door_node.attribute(C::XML_DOOR_MATERIAL.c_str()).value();
			string faction_ID = door_node.attribute(C::XML_DOOR_FACTION.c_str()).value();

			// add a door to the surface
			room->add_door(surface.child(C::XML_SURFACE_DIRECTION.c_str()).child_value(),
				health, material_ID, faction_ID);
		}
	}

	// add room to world
	room_at(x, y, z) = room;
}

// move specific room into memory
void World::load_room_to_world(const int & x, const int & y, const int & z)
{
	// if the room is already loaded
	if (room_at(x, y, z) != nullptr)
	{
		// nothing left to do here
		return;
	}

	// create an XML document to hold the vertical stack of rooms
	xml_document vertical_rooms;

	// load all rooms at x,y
	load_vertical_rooms_to_XML(x, y, vertical_rooms);

	// extract the room at z, add the room at x,y,z
	add_room_to_world(vertical_rooms, x, y, z);
}

// move a passed room to disk
void World::unload_room(const int & x, const int & y, const int & z, shared_ptr<Room> & room)
{
	// Unloads passed room. Can be called even if the room doesn't exist in the world structure

	// ensure the path exists up to \x
	string room_path = (C::room_directory + "\\" + R::to_string(x) + "\\");
	R::create_path_if_not_exists(room_path);

	// ensure the path exists up to \x-y.xml
	room_path += (R::to_string(x) + "-" + R::to_string(y) + ".xml");

	// create an XML document to represent the stack of rooms
	xml_document z_stack;

	// load the stack of rooms into the XML
	load_vertical_rooms_to_XML(x, y, z_stack);

	// process the room into the XML
	add_room_to_z_stack(z, room, z_stack);

	// save the document
	z_stack.save_file(room_path.c_str()); // returns an unused boolean
}

// add a room to a z_stack at a given index
void World::add_room_to_z_stack(const int & z, const shared_ptr<Room> & room, xml_document & z_stack) const
{
	// delete the room node if it already exists
	z_stack.remove_child(("room-" + R::to_string(z)).c_str());

	// create a new node for the room
	xml_node room_node = z_stack.append_child(("room-" + R::to_string(z)).c_str());

	// add a boolean representing if the room is water (off-island or a lake/river)
	room_node.append_attribute(C::XML_IS_WATER.c_str()).set_value(room->is_water());

	// for each item in the room
	multimap<string, shared_ptr<Item>> room_item_contents = room->get_contents();
	for (multimap<string, shared_ptr<Item>>::const_iterator item_it = room_item_contents.cbegin();
		item_it != room_item_contents.cend(); ++item_it)
	{
		// create a node for an item
		xml_node item_node = room_node.append_child(C::XML_ITEM.c_str());

		// append the item's ID
		item_node.append_child(node_pcdata).set_value(item_it->first.c_str());
	}

	// for each side of the room
	map<string, Room_Side> room_sides = room->get_room_sides();
	for (map<string, Room_Side>::const_iterator surface_it = room_sides.cbegin();
		surface_it != room_sides.cend(); ++surface_it)
	{
		// create a node for a surface
		xml_node surface_node = room_node.append_child(C::XML_SURFACE.c_str());

		// add an attribute to the surface containing the health of the surface (example: health="90")
		surface_node.append_attribute(C::XML_SURFACE_HEALTH.c_str()).set_value(surface_it->second.get_health());

		// create nodes for surface direction and surface material
		xml_node direction_node = surface_node.append_child(C::XML_SURFACE_DIRECTION.c_str());
		xml_node material_node = surface_node.append_child(C::XML_SURFACE_MATERIAL.c_str());

		// append the surface direction and material
		direction_node.append_child(node_pcdata).set_value(surface_it->first.c_str()); // direction ID
		material_node.append_child(node_pcdata).set_value(surface_it->second.get_material_id().c_str()); // material ID

		// conditionally create a node for a door
		if (surface_it->second.has_door())
		{
			// create a node to hold the door
			xml_node door_node = surface_node.append_child(C::XML_DOOR.c_str());

			// retrive the door
			shared_ptr<Door> door = surface_it->second.get_door();

			// add three attributes for health, material, and faction

			// health="55"
			door_node.append_attribute(C::XML_DOOR_HEALTH.c_str()).set_value(door->get_health());
			// material="wood"
			door_node.append_attribute(C::XML_DOOR_MATERIAL.c_str()).set_value(door->get_material_ID().c_str());
			// faction="hostile_NPC"
			door_node.append_attribute(C::XML_DOOR_FACTION.c_str()).set_value(door->get_faction_ID().c_str());
		}
	}
}

// create a new empty room given its coordinates and the world terrain
shared_ptr<Room> World::create_room(const int & x, const int & y, const int & z, const vector<vector<char>> world_terrain) const
{
	shared_ptr<Room> room = make_shared<Room>();

	// if the room is ground level and the terrain map indicates the room is forest
	if (z == C::GROUND_INDEX && world_terrain[x][y] == C::FOREST_CHAR)
	{
		room->add_item(Craft::make(C::TREE_ID)); // add a tree
	}
	// else, check if the room is water (off land or a lake/river)
	else if (z == C::GROUND_INDEX && world_terrain[x][y] == C::WATER_CHAR)
	{
		room->set_water_status(true);
	}

	return room;
}
