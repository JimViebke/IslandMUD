/* Jim Viebke
Jul 31 2015 */

#include <memory>

#include "chest.h"
#include "../craft.h"


// health
void Chest::damage(const int & amount)
{
	// PASS A NEGATIVE VALUE FOR DAMAGE

	health += amount;

	// if the health is larger than allowed
	if (health > C::MAX_CHEST_HEALTH)
	{
		// bring it back down to max
		health = C::MAX_CHEST_HEALTH;
	}

	// if health is negative
	if (health < 0)
	{
		// move health back up to 0
		health = 0;
	}
}
void Chest::set_health(const int & amount)
{
	health = amount;

	damage(0); // reuse the validation within
}
int Chest::get_health() const
{
	return health;
}

// faction ID retrieval
std::string Chest::get_faction_id() const
{
	return this->faction_id;
}
