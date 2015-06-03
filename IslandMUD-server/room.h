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
	const multimap<string, shared_ptr<Item>> get_contents() const { return contents; }
	const map<string, Room_Side> get_room_sides() const { return room_sides; }
	const vector<string> get_actor_ids() const { return actor_ids; }

	// room information
	bool has_wall() const;
	bool has_standing_wall() const;
	bool is_standing_wall(const string & surface_ID) const;
	bool has_surface(const string & direction_id) const
	{
		return room_sides.find(direction_id) != room_sides.cend();
	}
	string can_move_in_direction(const string & direction_ID, const string & faction_ID)
	{
		// if a surface is present
		if (has_surface(direction_ID))
		{
			// a player can move through the surface if it is rubble

			if (room_sides.find(direction_ID)->second.is_rubble())
			{
				return C::GOOD_SIGNAL;
			}

			return room_sides.find(direction_ID)->second.can_move_through_wall(faction_ID);
		}
		else // a surface does not exist...
		{
			// ...so the player is free to move
			return C::GOOD_SIGNAL;
		}
	}
	bool contains_no_items() const
	{
		return contents.size() == 0;
	}
	bool is_unloadable() const
	{
		return actor_ids.size() == 0 && viewing_actor_ids.size() == 0;
	}
	bool contains_item(const string & item_id) const
	{
		return contents.find(item_id) != contents.cend();
	}
	bool contains_item(const string & item_id, const unsigned & count) const;

	void set_water_status(const bool & is_water)
	{
		water = is_water;
	}
	bool is_water() const
	{
		return water;
	}

	// add and remove items
	void add_item(const shared_ptr<Item> item);
	void remove_item(const string & item_id, const int & count = 1);

	// add surfaces and doors
	void add_surface(const string & surface_ID, const string & material_ID);
	void add_surface(const string & surface_ID, const string & material_ID, const int & surface_health);
	void add_door(const string & directon_ID, const int & health, const string & material_ID, const string & faction_ID);

	// damage surface
	string damage_surface(const string & surface_ID, const shared_ptr<Item> & equipped_item);
	string damage_door(const string & surface_ID, const shared_ptr<Item> & equipped_item);

	// add and remove actors
	void add_actor(const string & actor_id);
	void remove_actor(const string & actor_id);
	void add_viewing_actor(const string & actor_id);
	void remove_viewing_actor(const string & actor_id);

	// printing
	string summary() const;

};

#endif
