
/* Jim Viebke
Dec 22 2015

Definitions for Chest object. */

#ifndef CHEST_H
#define CHEST_H

#include "item.h"

class Chest : public Item
{
private:
	std::string faction_id;
	std::multimap<std::string, std::shared_ptr<Equipment>> equipment_contents = {};
	std::map<std::string, std::shared_ptr<Material>> material_contents = {};

public:
	Chest(const std::string & set_faction_id) : Item(C::CHEST_ID, false), faction_id(set_faction_id) {}
	Chest(const std::string & set_faction_id, const int & set_health, const std::multimap<std::string, std::shared_ptr<Equipment>> & set_equipment_contents,
		const std::map<std::string, std::shared_ptr<Material>> & set_material_contents) : Item(C::CHEST_ID, false, set_health),
		faction_id(set_faction_id),
		equipment_contents(set_equipment_contents),
		material_contents(set_material_contents) {}

	// contents manipulation and retrieval
	void add(const std::shared_ptr<Item> & item);
	void remove(const std::string & item_id, const unsigned & count = 1);
	bool has(const std::string & item_id, const unsigned & count = 1) const;
	std::shared_ptr<Item> take(const std::string & item_id);
	std::string contents() const;
	std::multimap<std::string, std::shared_ptr<Equipment>> get_equipment_contents() const;
	std::map<std::string, std::shared_ptr<Material>> get_material_contents() const;

	// health
	void damage(const int & amount);
	void set_health(const int & amount);
	int get_health() const;

	// faction ID retrieval
	std::string get_faction_id() const;
};

#endif