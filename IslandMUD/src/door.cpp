/* Jim Viebke
May 19 2015 */

#include "door.h"

void Door::set_health_to(const int & set_health)
{
	health = set_health;
	
	update_health_by(0); // reuse the validation within
}
void Door::update_health_by(const int & health_update)
{
	// update the door's health
	health += health_update;

	// if the health is larger than allowed
	if (health > C::MAX_DOOR_HEALTH)
	{
		// bring it back down to max
		health = C::MAX_DOOR_HEALTH;
	}

	// if health is negative
	if (health < 0)
	{
		// move health back up to 0
		health = 0;
	}
}

// getters for serialization
int Door::get_health() const
{
	return health;
}
std::string Door::get_faction_ID() const
{
	return faction_ID;
}
std::string Door::get_material_ID() const
{
	return material_ID;
}

bool Door::is_rubble() const
{
	return (health == 0);
}
