/*
Jim Viebke
Feb 14, 2015 */

#ifndef CHARACTER_H
#define CHARACTER_H

#include <map> // for inventory multimap

#include "item.h"
#include "craft.h"
#include "room.h"
#include "world.h"

using namespace std;

class Character
{
public:
	string name;
	__int64 last_action_timestamp;
	vector<string> follower_ids;

	multimap<string, shared_ptr<Equipment>> equipment_inventory; // equipment doesn't stack
	map<string, shared_ptr<Material>> material_inventory; // materials stack

	// Item dragging_item; // a character can drag an item if they don't want to carry it.
	static Recipes recipes; // exists in memory once for all PCs and NPCs

	int x = C::DEFAULT_SPAWN_X; // location coordinates
	int y = C::DEFAULT_SPAWN_Y;
	int z = C::DEFAULT_SPAWN_Z;

	Character() { }
	virtual ~Character() {} // to make a polymorphic type

	// inventory manipulation
	void add(const shared_ptr<Item> & item);
	void remove(const string & item_id, const unsigned & count = 1);

	// inventory information
	bool has(const string & item_name, const unsigned & item_count = 1) const;
	bool does_not_have(const string & item_name, const unsigned & item_count = 1) const;

	// actions
	string craft(const string & craft_item_id, World & world);
	string move(const string & direction, World & world);
	string take(const string & item_id, World & world);
	string drop(const string & drop_item_id, World & world);
	string construct_surface(const string & material_id, const string & surface_id, World & world);

	// movement info
	string validate_movement(const string & direction_ID, const int & dx, const int & dy, const int & dz, const World & world) const
	{
		// determine if a character can move in a given direction (8 compass points, up, or down)

		// validate direction
		if (!R::contains(R::direction_ids, direction_ID)) { return direction_ID + " is not a direction."; }

		// if the player wants to move in a primary direction (n/e/s/w), condition is
		// no wall OR wall with opening (currently the opening is not taken into account)
		if (direction_ID == C::NORTH || direction_ID == C::EAST ||
			direction_ID == C::SOUTH || direction_ID == C::WEST)
		{
			// if the current room has a wall in the way
			// OR
			// the destination room has an opposite wall in the way
			if (world.room_at(x, y, z)->has_surface(direction_ID)
				||
				world.room_at(x + dx, y + dy, z)->has_surface(R::opposite_surface_id.find(direction_ID)->second))
			{
				return "There is a wall in your way to the " + direction_ID + ".";
			}
		}
		// if the player wants to move in a secondary direction (nw/ne/se/sw), condition is
		// one of two direction walls absent AND one of two result room opposite walls absent
		else if (
			direction_ID == C::NORTH_WEST || direction_ID == C::NORTH_EAST ||
			direction_ID == C::SOUTH_EAST || direction_ID == C::SOUTH_WEST)
		{
			const shared_ptr<Room> current_room = world.room_at(x, y, z);
			const shared_ptr<Room> destination_room = world.room_at(x + dx, y + dy, z);

			if (direction_ID == C::NORTH_WEST)
			{
				if (current_room->has_surface(C::NORTH) || current_room->has_surface(C::WEST) ||
					destination_room->has_surface(C::SOUTH) || destination_room->has_surface(C::EAST))
				{
					return "There are walls in your way to the " + direction_ID + ".";
				}
			}
			else if (direction_ID == C::NORTH_EAST)
			{
				if (current_room->has_surface(C::NORTH) || current_room->has_surface(C::EAST) ||
					destination_room->has_surface(C::SOUTH) || destination_room->has_surface(C::WEST))
				{
					return "There are walls in your way to the " + direction_ID + ".";
				}
			}
			else if (direction_ID == C::SOUTH_EAST)
			{
				if (current_room->has_surface(C::SOUTH) || current_room->has_surface(C::EAST) ||
					destination_room->has_surface(C::NORTH) || destination_room->has_surface(C::WEST))
				{
					return "There are walls in your way to the " + direction_ID + ".";
				}
			}
			else if (direction_ID == C::SOUTH_WEST)
			{
				if (current_room->has_surface(C::SOUTH) || current_room->has_surface(C::WEST) ||
					destination_room->has_surface(C::NORTH) || destination_room->has_surface(C::EAST))
				{
					return "There are walls in your way to the " + direction_ID + ".";
				}
			}
		}
		// condition for up is (opening AND ladder/stair/ramp)
		/*else if (direction_ID == C::UP || )
		{
		// HERE
		if ()
		{
		return "You [walk]/[climb] up to the [...]." //... ground level, second level, ...
		}
		}
		// condition for down is (ceiling) AND (ceiling has opening)
		else if (direction_ID == C::DOWN)
		{
		// HERE
		if ()
		{
		return "You drop down."; // ... to [ground level]/[the second level]
		}
		}*/

		// no issues were detected
		return C::GOOD_SIGNAL;
	}

};

#endif
