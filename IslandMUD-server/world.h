/* Jim Viebke
April 1, 2014 */

#ifndef WORLD_H
#define WORLD_H

#include <vector>
#include <memory>
#include <map>

#include "pugixml.hpp"

#include "room.h"
#include "generator.h"

using namespace std;
using namespace pugi;

class World
{
private:
	// x, y, and z dimensions repectively
	vector<vector<vector<shared_ptr<Room>>>> world;

public:

	World()
	{

	}


	void load()
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

	void create(const vector<vector<char>> & world_terrain)
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
		const double start_time = (double)R::current_time_in_ms();

		for (int x = 0; x < C::WORLD_X_DIMENSION; ++x) // for each row
		{
			// print completion percent and timing statistics
			int percent_processed = (x * 100 / C::WORLD_X_DIMENSION);
			if (percent_processed != previous_percent)
			{
				previous_percent = percent_processed;
				cout << "\nProcessing rooms: " << percent_processed << "% (" << ((double)R::current_time_in_ms() - start_time) << " sec elapsed, ~"
					<<
					((((R::current_time_in_ms() - start_time)) / (double)previous_percent) * (100.0 - (double)previous_percent)) / 60.0
					<< " minutes remaining...";
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
	shared_ptr<Room> room_at(const int & x, const int & y, const int & z) const
	{
		return world.at(x).at(y).at(z);
	}
	shared_ptr<Room> & room_at(const int & x, const int & y, const int & z)
	{
		return world.at(x).at(y).at(z);
	}

	// debugging
	unsigned count_loaded_rooms() const
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
	void load_view_radius_around(const int & x, const int & y, const string & character_ID)
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
	void remove_viewer_and_attempt_unload(const int & x, const int & y, const int & z, const string & viewer_ID)
	{
		if (!R::bounds_check(x, y, z) || room_at(x, y, z) == nullptr) // the reference room is out of bounds
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
	bool is_unloadable(const int & x, const int & y, const int & z) const
	{
		return room_at(x, y, z)->is_unloadable();
	}

	// move a room from world to disk
	void unload_room(const int & x, const int & y, const int & z)
	{
		// pass the coordinates and a shared_ptr to the room
		unload_room(x, y, z, room_at(x, y, z));

		// set the shared_ptr at x,y,z to null
		room_at(x, y, z) = nullptr;
	}

	// Build and return a top-down area map around a given coordinate
	string generate_area_map_for(const int & x, const int & y, const int & z) const
	{
		// this algorithm unfortunately does not take doors into account
		// We could just take out the wall piece to indicate a door. Whatever is in the corners will still indicate a wall.
		// What this doesn't do is show whether the door is open or closed.

		bool
			// is a wall present in a given direction?
			n, e, s, w,

			// is a forest area in a given direction from a coordinate? (f_n == forest_north)
			f_n, f_ne, f_e, f_se, f_s, f_sw, f_w, f_nw;


		stringstream user_map, a, b, c; // three stringstreams feed into one master stringstream

		// create a 2D vector to represent whether or not a tree is at a location
		vector<vector<bool>> forest_grid;
		for (int i = 0; i < (C::VIEW_DISTANCE * 2) + 3; ++i) // (view+1+view) plus a padding of 1 on each side
		{
			vector<bool> row;
			for (int j = 0; j < (C::VIEW_DISTANCE * 2) + 3; ++j)
			{
				row.push_back(false);
			}
			forest_grid.push_back(row);
		}

		/*
		We're looking for presence/absence info on trees in a 11*11 radius, and fitting it into a 13x13 grid, with a ring around for bounds padding.

		Top row is an example of indexes, botttom row is the forest_grid indexes these have to line up with

		player ---------v
		45 46 47 48 49 (50) 51 52 53 54 55
		1   2  3  4  5   6   7  8  9 10 11

		The following formula is applied to the indexes we're iterating over:
		tree_grid index = current index - (player index - (view distance + 1)) */
		for (int cx = x - (int)C::VIEW_DISTANCE; cx <= x + (int)C::VIEW_DISTANCE; ++cx)
		{
			int i = cx - (x - (C::VIEW_DISTANCE + 1));
			for (int cy = y - (int)C::VIEW_DISTANCE; cy <= y + (int)C::VIEW_DISTANCE; ++cy)
			{
				if (R::bounds_check(cx, cy))
				{
					forest_grid[i][cy - (y - (C::VIEW_DISTANCE + 1))] = room_at(cx, cy, C::GROUND_INDEX)->contains_item(C::TREE_ID);
				}
			}
		}

		/* for each row in sight range
		for each room in sight range in the row
		set n, e, s, w to(room contains surface ? ); */
		for (int cx = x - (int)C::VIEW_DISTANCE; cx <= x + (int)C::VIEW_DISTANCE; ++cx)
		{
			for (int cy = y - (int)C::VIEW_DISTANCE; cy <= y + (int)C::VIEW_DISTANCE; ++cy)
			{
				// if the room is out of bounds
				if (!R::bounds_check(cx, cy, C::GROUND_INDEX))
				{
					// draw the "room"
					a << "***";
					b << "***";
					c << "***";

					// nothing left to do with this room, skip to next room
					continue;
				}

				// if the coordinates are valid but the room is not loaded (this would be an error)
				if (room_at(cx, cy, C::GROUND_INDEX) == nullptr)
				{
					// draw the room as an exclaimation mark
					a << "   ";
					b << " ! ";
					c << "   ";

					// skip to next room
					continue;
				}

				// if there is a forest char at the given location
				// Somehow a tree is currently chosen to represent a forest room.

				// 23 24 25 26 27 (28) 29 30 31 32 33

				//  1  2  3  4  5   6   7  8  9 10 11

				// access_x = current index - (player index - (view distance + 1)) */
				//      1   =     23        - (      28     - (    5        + 1 ))

				// FGA = forest grid access
				int fga_x = cx - (x - (C::VIEW_DISTANCE + 1));
				int fga_y = cy - (y - (C::VIEW_DISTANCE + 1));
				if (forest_grid[fga_x][fga_y]) // if there is a tree here, determine how the tile should be drawn
				{
					// is a forest area at the neighbouring coordinates? f_n == forest_north
					f_n = forest_grid[fga_x - 1][fga_y];
					f_ne = forest_grid[fga_x - 1][fga_y + 1];
					f_e = forest_grid[fga_x][fga_y + 1];
					f_se = forest_grid[fga_x + 1][fga_y + 1];
					f_s = forest_grid[fga_x + 1][fga_y];
					f_sw = forest_grid[fga_x + 1][fga_y - 1];
					f_w = forest_grid[fga_x][fga_y - 1];
					f_nw = forest_grid[fga_x - 1][fga_y - 1];

					// conditionally draw a tree or an empty space in the corners, other five are always draw as trees
					a << ((f_n || f_nw || f_w) ? C::FOREST_CHAR : C::LAND_CHAR) << C::FOREST_CHAR << ((f_n || f_ne || f_e) ? C::FOREST_CHAR : C::LAND_CHAR);
					b << C::FOREST_CHAR << ((cx == x && cy == y) ? C::PLAYER_CHAR : C::FOREST_CHAR) << C::FOREST_CHAR;
					c << ((f_s || f_sw || f_w) ? C::FOREST_CHAR : C::LAND_CHAR) << C::FOREST_CHAR << ((f_s || f_se || f_e) ? C::FOREST_CHAR : C::LAND_CHAR);
				}
				// if the room is water
				else if (room_at(cx, cy, C::GROUND_INDEX)->is_water())
				{
					// Either draw a 3x3 grid with a "wave", or a 3x3 grid with the player's icon.

					a << "   ";
					b << " " << ((cx == x && cy == y) ? C::PLAYER_CHAR : C::WATER_CHAR) << " ";
					c << "   ";
				}
				// there is no tree, so there may be a structure
				else
				{
					// use a boolean value to indicate the presence or absence of any given wall
					n = room_at(cx, cy, C::GROUND_INDEX)->has_surface(C::NORTH);
					e = room_at(cx, cy, C::GROUND_INDEX)->has_surface(C::EAST);
					s = room_at(cx, cy, C::GROUND_INDEX)->has_surface(C::SOUTH);
					w = room_at(cx, cy, C::GROUND_INDEX)->has_surface(C::WEST);

					// time for glorious nested ternary statements to do this cheap
					a
						<< ((n && w) ? C::NW_CORNER : (n) ? C::WE_WALL : (w) ? C::NS_WALL : C::LAND_CHAR)
						<< ((n) ? C::WE_WALL : C::LAND_CHAR)
						<< ((n && e) ? C::NE_CORNER : (n) ? C::WE_WALL : (e) ? C::NS_WALL : C::LAND_CHAR);
					b
						<< ((w) ? C::NS_WALL : C::LAND_CHAR)
						// if the current coordinates are the player's, draw an @ icon, else if there is an item, draw an item char, else empty
						<< ((cx == x && cy == y) ? C::PLAYER_CHAR : ((room_at(cx, cy, C::GROUND_INDEX)->contains_no_items()) ? C::LAND_CHAR : C::ITEM_CHAR))
						<< ((e) ? C::NS_WALL : C::LAND_CHAR);
					c
						<< ((s && w) ? C::SW_CORNER : (s) ? C::WE_WALL : (w) ? C::NS_WALL : C::LAND_CHAR)
						<< ((s) ? C::WE_WALL : C::LAND_CHAR)
						<< ((s && e) ? C::SE_CORNER : (s) ? C::WE_WALL : (e) ? C::NS_WALL : C::LAND_CHAR);
				}
			} // end for each room in row

			// each iteration, push the three stringstreams representing the row into the user's map
			user_map << a.str() << endl << b.str() << endl << c.str() << endl;

			// reset the stringstreams for the next row of rooms
			a.str(std::string()); b.str(std::string()); c.str(std::string());
		} // end for each row

		return user_map.str(); // this now contains the client's entire visible area. No formatting is required, just dump this to console.
	}



private:

	// load in all rooms at x,y to an xml_document
	void load_vertical_rooms_to_XML(const int & ix, const int & iy, xml_document & vertical_rooms)
	{
		// convert integers to strings, since they'll be used multiple times
		string x = R::to_string(ix);
		string y = R::to_string(iy);

		// populate the document using the file for the vertical stack of rooms at x,y
		vertical_rooms.load_file((C::room_directory + "\\" + x + "\\" + x + "-" + y + ".xml").c_str());
	}

	// build a room from XML at location z, add to world at x,y,z
	void add_room_to_world(const xml_document & z_stack, const int & x, const int & y, const int & z)
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
				? health_attribute.as_int() // set the surface health to its value
				: C::MAX_SURFACE_HEALTH // else set the room to full health

				);
		}

		// add room to world
		room_at(x, y, z) = room;
	}

	// move specific room into memory
	void load_room_to_world(const int & x, const int & y, const int & z)
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
	void unload_room(const int & x, const int & y, const int & z, shared_ptr<Room> & room)
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
	void add_room_to_z_stack(const int & z, const shared_ptr<Room> & room, xml_document & z_stack) const
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
			xml_node surface_node = room_node.append_child(C::XML_SURFACE.c_str()); // duplicate or replace?

			// add an attribute to the surface containing the health of the surface (example: health="90")
			surface_node.append_attribute(C::XML_SURFACE_HEALTH.c_str()).set_value(surface_it->second.get_health());

			// create nodes for surface direction and surface material
			xml_node direction_node = surface_node.append_child(C::XML_SURFACE_DIRECTION.c_str()); // duplicate or replace?
			xml_node material_node = surface_node.append_child(C::XML_SURFACE_MATERIAL.c_str()); // duplicate or replace?

			// append the surface direction and material
			direction_node.append_child(node_pcdata).set_value(surface_it->first.c_str()); // direction ID
			material_node.append_child(node_pcdata).set_value(surface_it->second.material_id.c_str()); // material ID
		}
	}

	// create a new empty room given its coordinates and the world terrain
	shared_ptr<Room> create_room(const int & x, const int & y, const int & z, const vector<vector<char>> world_terrain) const
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
};

#endif
