/* Jim Viebke
Mar 19 2015

Floors, walls, and ceilings. */

#ifndef ROOM_SIDE_H
#define ROOM_SIDE_H

#include "item.h"
#include "craft.h"
#include "door.h"

class Room_Side
{
private:
	int integrity = C::MAX_SURFACE_HEALTH;
	std::string material_id;
	std::shared_ptr<Door> door = nullptr;

public:

	Room_Side(const std::string & material_id) : material_id(material_id) {}

	// health retrieval and modification
	int get_health() const;
	void set_health(const int & health);
	void change_health(const int & change);

	// the surface's material
	std::string get_material_id() const;

	// surface information
	bool is_intact() const;
	bool is_rubble() const;
	bool has_door() const;
	bool has_intact_door() const;

	std::string can_move_through_wall(const std::string & player_faction_ID) const;

	std::shared_ptr<Door> get_door() const;

	void add_door(const int & health, const std::string & material_ID, const std::string & faction_ID);
	void remove_door();

};

#endif
