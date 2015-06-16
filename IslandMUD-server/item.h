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
	bool takable = false; // items are untakable by default
public:
	string name;
	int weight = 0;
	
	Item(string item_name, bool takable = false) : name(item_name), takable(takable) {}
	virtual ~Item() {}

	bool is_takable() const { return takable; }

};

class Material : public Item
{
public:
	unsigned amount = 1;
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

class Tree : public Item
{
public:
	Tree() : Item(C::TREE_ID, false) {}
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
public:
	string equipment_material; // stone, iron, wood, etc
	string handle_material; // wood, etc
	// other member represent quality, health

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
	Debris() : Item(C::DEBRIS_ID) {}
};

#endif
