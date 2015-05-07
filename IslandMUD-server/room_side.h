/* Jim Viebke
Mar 19 2015

Floors, walls, and ceilings. No doors yet. */

#ifndef ROOM_SIDE_H
#define ROOM_SIDE_H

#include "item.h"
#include "craft.h"

class Room_Side
{
private:
	int integrity = 100;

public:
	string material_id;

	Room_Side(const string & material_id) : material_id(material_id) {}

	// health retrieval and modification
	inline int get_health() const
	{
		return integrity;
	}
	inline void set_health(const int & health)
	{
		// if the passed value is within acceptable bounds
		if (health >= C::MIN_SURFACE_HEALTH &&
			health <= C::MIN_SURFACE_HEALTH)
		{
			// set the surface's integry to what was passed
			integrity = health;
		}
	}
	inline void change_health(const int & change)
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
};

#endif