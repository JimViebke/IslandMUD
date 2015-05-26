/* Jim Viebke
Mar 19 2015

Floors, walls, and ceilings. No doors yet. */

#ifndef ROOM_SIDE_H
#define ROOM_SIDE_H

#include "item.h"
#include "craft.h"
#include "door.h"

class Room_Side
{
private:
	int integrity = C::MAX_SURFACE_HEALTH;
	string material_id;
	shared_ptr<Door> door = nullptr;

public:

	Room_Side(const string & material_id) : material_id(material_id) {}

	// health retrieval and modification
	int get_health() const
	{
		return integrity;
	}
	void set_health(const int & health)
	{
		// if the passed value is within acceptable bounds
		if (health <= C::MAX_SURFACE_HEALTH &&
			health >= C::MIN_SURFACE_HEALTH)
		{
			// set the surface's integry to what was passed
			integrity = health;
		}
		// else, integrity remains unchanged
	}
	void change_health(const int & change)
	{
		// modify this surface's integrity
		integrity += change;

		// if the current integrity is below bounds
		if (integrity < C::MIN_SURFACE_HEALTH)
		{
			// set it to min
			integrity = C::MIN_SURFACE_HEALTH;
		}
		// if the current integrity is above bounds
		else if (integrity > C::MAX_SURFACE_HEALTH)
		{
			// set it to max
			integrity = C::MAX_SURFACE_HEALTH;
		}
	}

	string get_material_id() const
	{
		return material_id;
	}

	// surface information
	bool is_intact() const
	{
		return integrity > 0;
	}
	bool is_rubble() const
	{
		return (integrity == 0);
	}
	bool has_door() const
	{
		return (door != nullptr);
	}
	bool has_intact_door() const
	{
		return this->has_door() && !door->is_rubble();
	}

	string can_move_through_wall(const string & player_faction_ID) const
	{
		// if the surface has a door
		if (this->has_door())
		{
			// the character can move through the door if it is rubble, or owned by the player's faction
			if (door->is_rubble())
			{
				return C::GOOD_SIGNAL;
			}

			if (door->get_faction_ID() == player_faction_ID)
			{
				return C::GOOD_SIGNAL;
			}
			else
			{
				return "This door has an unfamiliar lock.";
			}
		}
		else // this is a wall without a door
		{
			// the player cannot move through
			return "There is a wall in your way.";
		}
	}

	shared_ptr<Door> get_door() const
	{
		return door;
	}

	void add_door(const int & health, const string & material_ID, const string & faction_ID)
	{
		this->door = make_shared<Door>(faction_ID, material_ID, health);
	}
	void remove_door()
	{
		// removes reference to a door if one exists
		this->door = nullptr;
	}

};

#endif
