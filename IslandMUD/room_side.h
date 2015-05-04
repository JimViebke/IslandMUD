/* Jim Viebke
Mar 19 2015

Floors, walls, and ceilings. No doors yet. */

#ifndef ROOM_SIDE_H
#define ROOM_SIDE_H

#include "item.h"
#include "craft.h"

class Room_Side
{
public:
	string material_id;
	int completion = 0; // unused
	int integrity = 0; // unused

	Room_Side(const string & material_id = C::BAD_ITEM_ID) : material_id(material_id) {}
};

#endif