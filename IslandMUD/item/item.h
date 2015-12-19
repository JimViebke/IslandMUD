/* Jim Viebke
Feb 14, 2015 */

#ifndef ITEM_H
#define ITEM_H

#include <vector>
#include <string>
#include <map>
#include <memory>

#include "constants.h"
#include "../utilities.h"

class Item
{
private:
	bool takable;

public:
	std::string name;
	// int weight = 0;

protected:

	int health = 100;

	Item(const std::string & item_name, const bool & is_takable, const int & set_health = C::DEFAULT_ITEM_MAX_HEALTH) :
		name(item_name), takable(is_takable), health(set_health) {}

	virtual ~Item() {}

public:

	bool is_takable() const { return takable; }
	int get_health() const { return health; }
	void set_health(const int & set_health)
	{
		// if the passed value is out of bounds, set health to max_health, otherwise set to the passed value
		health = ((set_health > C::DEFAULT_ITEM_MAX_HEALTH || set_health < C::DEFAULT_ITEM_MIN_HEALTH)
			? C::DEFAULT_ITEM_MAX_HEALTH : set_health);
	}
	void update_health(const int & update_health)
	{
		// subtract the passed amount from the current health
		health -= update_health;

		// use set_health() to validate
		set_health(health);
	}

};

class Material : public Item
{
public:
	unsigned amount = 1;

protected:
	Material(const std::string & cust_name) : Item(cust_name, true) {} // all materials are takable
};

class Fire_Container : public Item
{
protected:
	// These three fields store the amount of fuel in the item for a given point in time,
	// and a boolean indicating if the item was lit at that time.
	// These fields update every time the item is observed.
	unsigned fuel_units_remaining = 0;
	time_t fuel_time;
	bool lit = false;

	// add something to indicate fuel type, which will be used to determine the temperature of the fire 
	// Fuel_Type

	Fire_Container(const std::string & item_name) : Item(item_name, false) {}
};

class Forgeable : public Item
{
protected:
	// contents are 
	unsigned iron_units; // units are arbitrary, but consistent
	unsigned carbon_units;
	unsigned impurity_units;

	unsigned hardness;
	unsigned toughness;
	unsigned grain_size;

	unsigned self_temperature;
	long long last_update_timestamp;

	Forgeable(const std::string & item_ID) : Item(item_ID, true) {}

	void update_specs(const unsigned & ambient_temperature)
	{
		// Use the ambient temperature to update the temperature of the forgeable item.
		// The ambient temperature is the temperature of the fire, forge, or just the air temperature.

		// What factors affect the speed at which the temperature of the forgeable item approaches
		// the ambient temperature?
		// Just time and size.

		// So how fast does the temperature of the forgable item approach the ambient temperature?
		// ... ?
	}
};

class Bloom : public Forgeable // this is the glowy lump that comes out of a bloomery
{
public:
	Bloom() : Forgeable(C::BLOOM_ID) {}

	// unsigned bloom_units() const { return iron_units + carbon_units + impurity_units; }
};

class Mineral : public Material
{
protected:
	Mineral(const std::string & mineral_ID) : Material(mineral_ID) {}
};

class Bloomery : public Fire_Container // raw/scrap/unwanted ores and metals goes in, a glowy blooms comes out
{
public:
	Bloomery() : Fire_Container(C::BLOOMERY_ID) {}

private:
	class Meltable
	{
	public:
		std::shared_ptr<Item> meltable_item;
		unsigned temperature;
		long long insertion_time;
		Bloomery::Meltable(const std::shared_ptr<Item> & insert_item)
		{
			this->meltable_item = insert_item;
			this->insertion_time = U::current_time_in_ms();
		}
	};
	
public:
	void add_mineral_to_bloomery(const std::shared_ptr<Mineral> & mineral)
	{
		// Accept a shared_ptr to a Mineral types.
		// The mineral item will be wrapped in a Meltable item, which records
		melt_contents.push_back(Meltable(mineral));
	}

private:	
	std::unique_ptr<Bloom> bloom; // a bloomery may contain 1 or 0 blooms. The bloom will be a nullptr if there is no bloom.

	std::vector<Meltable> melt_contents; // an item will melt and join the bloom when it reaches temperature and melts

	void update_bloomery() // update the temperature and state of all items inside the bloomery
	{

	}
};

class Equipment : public Item
{
	// string equipment_material; // stone, iron, wood, etc
	// string handle_material; // wood, etc
	// other members represent quality, health

protected:
	Equipment(const std::string & set_name) : Item(set_name, true) {} // all equipment is takable
};

class Forge : public Fire_Container
{
private:
	std::unique_ptr<Equipment> workpiece;
	time_t workpiece_insert_time; // the time that the current workpiece placed in the forge

public:
	Forge() : Fire_Container(C::FORGE_ID) {}
};

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

class Mineral_Deposit : public Item
{
protected:
	Mineral_Deposit(const std::string & mineral_ID) : Item(mineral_ID, false) {}
};

class Iron_Deposit : public Mineral_Deposit
{
public:
	Iron_Deposit() : Mineral_Deposit(C::IRON_DEPOSIT_ID) {}
};

class Limestone_Deposit : public Mineral_Deposit
{
public:
	Limestone_Deposit() : Mineral_Deposit(C::LIMESTONE_DEPOSIT_ID) {}
};

class Iron : public Mineral
{
public:
	Iron() : Mineral(C::IRON_ID) {}
};

class Limestone : public Mineral
{
public:
	Limestone() : Mineral(C::LIMESTONE_ID) {}
};

#endif
