/* Jim Viebke
Feb 14, 2015 */

#ifndef ROOM_H
#define ROOM_H

#include <map> // for room contents multimap
#include <set> // playerlist

#include "resources.h"
#include "constants.h"
#include "item.h"
#include "room_side.h" // walls, floor, or ceiling

class Room
{
private:
	bool updated = false; // has the room been updated since it was loaded?
	bool water = false; // is this dry land or water
	map<string, Room_Side> room_sides = {}; // the floor, walls, and ceiling in the room (no key for absent surfaces)
	multimap<string, shared_ptr<Item>> contents = {}; // the items in a room
	vector<string> viewing_actor_ids = {}; // the PCs and NPCs who can see this room
	vector<string> actor_ids = {}; // the PCs and NPCs in a room

public:

	Room() {}

	// room contents
	inline const multimap<string, shared_ptr<Item>> Room::get_contents() const
	{
		return contents;
	}
	inline const map<string, Room_Side> Room::get_room_sides() const
	{
		return room_sides;
	}

	// room information
	bool has_wall() const;
	inline bool has_surface(const string & direction_id) const
	{
		return room_sides.find(direction_id) != room_sides.cend();
	}
	inline bool contains_no_items() const
	{
		return contents.size() == 0;
	}
	inline bool is_unloadable() const
	{
		return actor_ids.size() == 0 && viewing_actor_ids.size() == 0;
	}
	inline bool contains_item(const string & item_id) const
	{
		return contents.find(item_id) != contents.cend();
	}
	bool contains_item(const string & item_id, const unsigned & count) const;

	inline void set_water_status(const bool & is_water)
	{
		water = is_water;
	}
	inline bool is_water() const
	{
		return water;
	}

	// add and remove items
	void add_item(const shared_ptr<Item> item);
	void remove_item(const string & item_id, const int & count = 1);

	// add surfaces
	void add_surface(const string & surface_ID, const string & material_ID);
	void add_surface(const string & surface_ID, const string & material_ID, const int & surface_health);

	// damage surface
	string damage_surface(const string & surface_ID, const shared_ptr<Item> & equipped_item);

	// add and remove actors
	void add_actor(const string & actor_id);
	void remove_actor(const string & actor_id);
	void add_viewing_actor(const string & actor_id);
	void remove_viewing_actor(const string & actor_id);

	// printing
	string summary() const;

};

#endif
