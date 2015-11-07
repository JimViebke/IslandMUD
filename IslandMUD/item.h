/* Jim Viebke
Feb 14, 2015 */

#ifndef ITEM_H
#define ITEM_H

#include <vector>
#include <string>
#include <map>

#include "parse.h"

using namespace std;

class Item
{
private:
	bool takable;

public:
	string name;
	// int weight = 0;

protected:

	int health = 100;

	Item(const string & item_name, const bool & is_takable, const int & set_health = C::DEFAULT_ITEM_MAX_HEALTH) :
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
	Material(const string & cust_name) : Item(cust_name, true) {} // all materials are takable
};

class Stone : public Material
{
public:
	Stone() : Material(C::STONE_ID) {}
};

class Stick : public Material
{
public:
	Stick() : Material(C::STICK_ID) {}
};

class Branch : public Material // like a stick, but thicker
{
public:
	Branch() : Material(C::BRANCH_ID) {}
};

class Vine : public Material
{
public:
	Vine() : Material(C::VINE_ID) {}
};

class Wood : public Material
{
public:
	Wood() : Material(C::WOOD_ID) {}
};

class Arrow : public Material
{
public:
	Arrow() : Material(C::ARROW_ID) {}
};

class Arrowhead : public Material
{
public:
	Arrowhead() : Material(C::ARROWHEAD_ID) {}
};

class Board : public Material
{
public:
	Board() : Material(C::BOARD_ID) {}
};

class Tree : public Item
{
public:
	Tree() : Item(C::TREE_ID, false) {}
};

class Smelter : public Item
{
public:
	Smelter() : Item(C::SMELTER_ID, false) {}
};

class Anvil : public Item
{
public:
	Anvil() : Item(C::ANVIL_ID, false) {}
};

class Equipment : public Item
{
	// string equipment_material; // stone, iron, wood, etc
	// string handle_material; // wood, etc
	// other members represent quality, health

protected:
	Equipment(const string & set_name) : Item(set_name, true) {} // all equipment is takable
};

class Forge : public Item
{
private:
	unique_ptr<Equipment> workpiece;
	time_t workpiece_insert_time; // the time that the current workpiece placed in the forge

	// These three fields store the amount of fuel in the forge for a given point in time,
	// and a boolean indicating if the forge was lit at that time.
	// These fields update every time the forge is observed.
	unsigned fuel_units_remaining = 0;
	time_t fuel_time;
	bool lit = false;

	// add something to indicate fuel type, which will be used to determine the temperature of the fire 

public:
	Forge() : Item(C::FORGE_ID, false) {}
};

class Staff : public Equipment
{
public:
	Staff() : Equipment(C::STAFF_ID) {}
};

class Torch : public Equipment
{
public:
	Torch() : Equipment(C::TORCH_ID) {}
};

class Axe : public Equipment
{
public:
	Axe() : Equipment(C::AXE_ID) {}
};

class Bow : public Equipment
{
public:
	Bow() : Equipment(C::BOW_ID) {}
};

class Sword : public Equipment
{
public:
	Sword() : Equipment(C::SWORD_ID) {}
};

class Hammer : public Equipment
{
public:
	Hammer() : Equipment(C::HAMMER_ID) {}
};

class Debris : public Item
{
public:
	Debris() : Item(C::DEBRIS_ID, false) {}
};

class Chest : public Item
{
private:
	string faction_id;
	multimap<string, shared_ptr<Equipment>> equipment_contents = {};
	map<string, shared_ptr<Material>> material_contents = {};

public:
	Chest(const string & set_faction_id) : Item(C::CHEST_ID, false), faction_id(set_faction_id) {}
	Chest(const string & set_faction_id, const int & set_health, const multimap<string, shared_ptr<Equipment>> & set_equipment_contents,
		const map<string, shared_ptr<Material>> & set_material_contents) : Item(C::CHEST_ID, false, set_health),
		faction_id(set_faction_id),
		equipment_contents(set_equipment_contents),
		material_contents(set_material_contents) {}

	// contents manipulation and retrieval
	void add(const shared_ptr<Item> & item);
	void remove(const string & item_id, const unsigned & count = 1);
	bool has(const string & item_id, const unsigned & count = 1) const;
	shared_ptr<Item> take(const string & item_id);
	string contents() const;
	multimap<string, shared_ptr<Equipment>> get_equipment_contents() const;
	map<string, shared_ptr<Material>> get_material_contents() const;

	// health
	void damage(const int & amount);
	void set_health(const int & amount);
	int get_health() const;

	// faction ID retrieval
	string get_faction_id() const;
};

class Log : public Item
{
public:
	Log() : Item(C::LOG_ID, false) {}
};

class Mineral_Deposit : public Item
{
protected:
	Mineral_Deposit(const string & mineral_ID) : Item(mineral_ID, false) {}
};

class Mineral : public Material
{
protected:
	Mineral(const string & mineral_ID) : Material(mineral_ID) {}
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
