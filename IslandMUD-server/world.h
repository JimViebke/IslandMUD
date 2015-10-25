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
public:

private:
	// 2d terrain (biome) map
	unique_ptr<vector<vector<char_type>>> terrain;
	unique_ptr<vector<vector<char_type>>> iron_deposit_map;
	unique_ptr<vector<vector<char_type>>> limestone_deposit_map;

	vector<unique_ptr<Room>> world;

public:

	World();

	void load();

	// access a room given coordinates
	unique_ptr<Room>::pointer room_at(const int & x, const int & y, const int & z);
	const unique_ptr<Room>::pointer room_at(const int & x, const int & y, const int & z) const;
	unique_ptr<Room> & room_pointer_at(const int & x, const int & y, const int & z);

	// debugging
	unsigned count_loaded_rooms() const;

	// load rooms around a player spawning in
	void load_view_radius_around(const int & x, const int & y, const string & character_ID);

	// loading and unloading rooms at the edge of vision
	void remove_viewer_and_attempt_unload(const int & x, const int & y, const int & z, const string & viewer_ID);

	// test if a room can be removed from memory
	bool is_unloadable(const int & x, const int & y, const int & z) const;

	// move a room from world to disk
	void unload_room(const int & x, const int & y, const int & z);

	// room information
	bool room_has_surface(const int & x, const int & y, const int & z, const string & direction_ID) const;



private:

	void create_world_container();
	void load_or_generate_terrain_and_mineral_maps();

	// three functions for loading and verifying the world map and the two mineral maps

	bool load_existing_world_terrain();
	bool load_existing_iron_deposit_map();
	bool load_existing_limestone_deposit_map();
	
	// a room at x,y,z does not exist on the disk; create it
	void generate_room_at(const int & x, const int & y, const int & z);

	// load in all rooms at x,y to an xml_document
	void load_vertical_rooms_to_XML(const int & ix, const int & iy, xml_document & vertical_rooms);

	// build a room given an XML node, add to world at x,y,z
	void add_room_to_world(xml_node & room_node, const int & x, const int & y, const int & z);

	// move specific room into memory
	void load_room_to_world(const int & x, const int & y, const int & z);

	// move a passed room to disk
	void unload_room(const int & x, const int & y, const int & z, const unique_ptr<Room>::pointer room);

	// add a room to a z_stack at a given index
	void add_room_to_z_stack(const int & z, const unique_ptr<Room>::pointer room, xml_document & z_stack) const;

	// create a new empty room given its coordinates and the world terrain
	unique_ptr<Room> create_room(const int & x, const int & y, const int & z) const;

	// remove a room from memory
	void erase_room_from_memory(const int & x, const int & y, const int & z);
};

#endif
