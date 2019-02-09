/* Jim Viebke
Jun 23 2015 */

#include "room_side.h"

// health retrieval and modification
int Room_Side::get_health() const
{
	return health;
}
void Room_Side::set_health(const int & set_health)
{
	if (set_health > C::MAX_SURFACE_HEALTH ||
		set_health < C::MIN_SURFACE_HEALTH) return;

	health = set_health;
}
void Room_Side::change_health(const int & change)
{
	// modify this surface's integrity
	health += change;

	// if the current integrity is below bounds
	if (health < C::MIN_SURFACE_HEALTH)
	{
		// set it to min
		health = C::MIN_SURFACE_HEALTH;
	}
	// if the current integrity is above bounds
	else if (health > C::MAX_SURFACE_HEALTH)
	{
		// set it to max
		health = C::MAX_SURFACE_HEALTH;
	}
}

// the surface's material
std::string Room_Side::get_material_id() const
{
	return material_id;
}

// surface information
bool Room_Side::is_intact() const
{
	return health > 0;
}
bool Room_Side::is_rubble() const
{
	return health == 0;
}
bool Room_Side::has_door() const
{
	return door != nullptr;
}
bool Room_Side::has_intact_door() const
{
	return this->has_door() && !door->is_rubble();
}

C::move_attempt Room_Side::is_traversable(const std::string & player_faction_ID) const
{
	if (this->is_rubble())
	{
		return C::move_attempt::traversable;
	}
	// if the surface has a door
	else if (this->has_door())
	{
		// the character can move through the door if it is rubble, or owned by the player's faction
		if (door->is_rubble())
		{
			return C::move_attempt::traversable;
		}

		if (door->get_faction_ID() == player_faction_ID)
		{
			return C::move_attempt::traversable;
		}
		else
		{
			return C::move_attempt::unfamiliar_lock;
		}
	}
	else // this is a wall without a door
	{
		// the player cannot move through
		return C::move_attempt::blocked_by_wall;
	}
}

std::shared_ptr<Door> Room_Side::get_door() const
{
	return door;
}

void Room_Side::add_door(const int & health, const std::string & material_ID, const std::string & faction_ID)
{
	this->door = std::make_shared<Door>(faction_ID, material_ID, health);
}
void Room_Side::remove_door()
{
	// removes reference to a door if one exists
	this->door = nullptr;
}
