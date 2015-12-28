
/* Jim Viebke
Dec 22 2015

Definitions for Chest object. */

#ifndef CHEST_H
#define CHEST_H

#include "item.h"
#include "container.h"

class Chest : public Item, public Container
{
private:
	std::string faction_id;

public:
	Chest(const std::string & set_faction_id) : Item(C::CHEST_ID, false), faction_id(set_faction_id) {}
	Chest(const std::string & set_faction_id, const int & set_health, const std::multimap<std::string, std::shared_ptr<Item>> & set_contents)
		: Item(C::CHEST_ID, false, set_health), Container(), faction_id(set_faction_id)
	{
		this->set_contents(set_contents);
	}

	// health
	void damage(const int & amount);
	void set_health(const int & amount);
	int get_health() const;

	// faction ID retrieval
	std::string get_faction_id() const;
};

#endif
