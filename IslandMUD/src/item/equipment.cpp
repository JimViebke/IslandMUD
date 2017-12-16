
/* Jim Viebke
May 11 2016 */

#include "equipment.h"

Equipment::Equipment(const std::string & name) : Item(name, true) {}

unsigned Equipment::get_combat_damage() const
{
	// get the damage table for this item
	const std::map<std::string, std::map<std::string, int>>::const_iterator implement_table_it = C::damage_tables.find(this->get_name());

	// if no damage table exists for this item, return 1
	if (implement_table_it == C::damage_tables.cend()) return 1;

	// a damage table exists for this item, extract it from the iterator
	const std::map<std::string, int> implement_table = implement_table_it->second;

	// get the damage amount against a character
	const std::map<std::string, int>::const_iterator implement_damage = implement_table.find("");

	// if a damage table exists for this item but does not have an entry for attacking a character, return 1
	if (implement_damage == implement_table.cend()) return 1;

	// return the damage
	return std::max(implement_damage->second, 1);
}
