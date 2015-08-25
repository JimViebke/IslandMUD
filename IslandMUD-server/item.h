/* Jim Viebke
Feb 14, 2015 */

#ifndef ITEM_H
#define ITEM_H

#include <vector>
#include <string>
#include <map>

#include "resources.h"
#include "parse.h"

using namespace std;

class Item
{
private:
	bool takable; // items are untakable by default

public:
	string name;
	// int weight = 0;

protected:

	int health = 100;

	Item(string item_name, bool is_takable, int health = C::DEFAULT_ITEM_MAX_HEALTH) :
		name(item_name), takable(is_takable), health(health) {}

	virtual ~Item() {}

public:

	bool is_takable() const { return takable; }
	int get_health() const { return health; }
	void set_health(int set_health)
	{
		// if the passed value is out of bounds, set health to max_health, otherwise set to the passed value
		health = ((set_health > C::DEFAULT_ITEM_MAX_HEALTH || set_health < C::DEFAULT_ITEM_MIN_HEALTH)
			? C::DEFAULT_ITEM_MAX_HEALTH : set_health);
	}

};

class Material : public Item
{
public:
	unsigned amount = 1;

protected:
	Material(const string & cust_name) : Item(cust_name, true) {} // all materials are takable
};

class Rock : public Material
{
public:
	Rock() : Material(C::STONE_ID) {}
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

class Forge : public Item
{
public:
	Forge() : Item(C::FORGE_ID, false) {}
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
	Equipment(string cust_name) : Item(cust_name, true) {} // all equipment is takable
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
	multimap<string, shared_ptr<Equipment>> equipment_contents = {};
	map<string, shared_ptr<Material>> material_contents = {};

public:
	Chest() : Item(C::CHEST_ID, false) {}

	Chest(const int & set_health, const multimap<string, shared_ptr<Equipment>> & set_equipment_contents,
		const map<string, shared_ptr<Material>> & set_material_contents) : Item(C::CHEST_ID, false, set_health),
		equipment_contents(set_equipment_contents),
		material_contents(set_material_contents) {}

	// contents
	void add(const shared_ptr<Item> & item);
	void remove(const string & item_id, const unsigned & count = 1);
	bool has(const string & item_id, const unsigned & count = 1) const;
	shared_ptr<Item> take(const string & item_id);

	// health
	void damage(const int & amount);
	void set_health(const int & amount);
	int get_health() const;

	// contents
	string contents() const;
	multimap<string, shared_ptr<Equipment>> get_equipment_contents() const;
	map<string, shared_ptr<Material>> get_material_contents() const;
};

#endif
