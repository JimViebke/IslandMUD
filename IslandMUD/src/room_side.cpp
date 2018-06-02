/* Jim Viebke
Jun 23 2015 */

#include "room_side.h"

// health retrieval and modification
int Room_Side::get_health() const
{
	return integrity;
}
void Room_Side::set_health(const int & health)
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
void Room_Side::change_health(const int & change)
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

// the surface's material
std::string Room_Side::get_material_id() const
{
	return material_id;
}

// surface information
bool Room_Side::is_intact() const
{
	return integrity > 0;
}
bool Room_Side::is_rubble() const
{
	return (integrity == 0);
}
bool Room_Side::has_door() const
{
	return (door != nullptr);
}
bool Room_Side::has_intact_door() const
{
	return this->has_door() && !door->is_rubble();
}

std::string Room_Side::can_move_through_wall(const std::string & player_faction_ID) const
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