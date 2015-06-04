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
// #include "character.h"

using namespace std;
using namespace pugi;

class World
{
private:
	// x, y, and z dimensions repectively
	vector<vector<vector<shared_ptr<Room>>>> world;

public:

	World() {}

	void load();

	void create(const vector<vector<char>> & world_terrain);

	// return shared_ptr to a room at a location
	shared_ptr<Room> room_at(const int & x, const int & y, const int & z) const;
	shared_ptr<Room> & room_at(const int & x, const int & y, const int & z);

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



private:

	// load in all rooms at x,y to an xml_document
	void load_vertical_rooms_to_XML(const int & ix, const int & iy, xml_document & vertical_rooms);

	// build a room from XML at location z, add to world at x,y,z
	void add_room_to_world(const xml_document & z_stack, const int & x, const int & y, const int & z);

	// move specific room into memory
	void load_room_to_world(const int & x, const int & y, const int & z);

	// move a passed room to disk
	void unload_room(const int & x, const int & y, const int & z, shared_ptr<Room> & room);

	// add a room to a z_stack at a given index
	void add_room_to_z_stack(const int & z, const shared_ptr<Room> & room, xml_document & z_stack) const;

	// create a new empty room given its coordinates and the world terrain
	shared_ptr<Room> create_room(const int & x, const int & y, const int & z, const vector<vector<char>> world_terrain) const;
};

#endif
