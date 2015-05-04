/*
Jim Viebke
Feb 14, 2015 */

#ifndef ITEM_H
#define ITEM_H

#include <vector>
#include <string>
#include <map>

#include "resources.h"
#include "constants.h"
#include "parse.h"

using namespace std;

class Item
{
public:
	string name;
	int weight = 0;
	bool is_takable = false; // items are untakable by default
	
	Item(string item_name = "BAD_ITEM_NAME", bool takable = false) : name(item_name), is_takable(takable) {}
	virtual ~Item() {}

};

class Craftable : public Item // this is useless
{
public:
	Craftable(string cust_name) : Item(cust_name, true) {}
};

class Material : public Item
{
public:
	unsigned amount = 1;
	Material(string cust_name) : Item(cust_name, true) {} // all materials are takable
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

class Tree : public Item
{
public:
	Tree() : Item(C::TREE_ID, false) {}
};

class Equipment : public Craftable
{
public:
	string equipment_material; // stone, iron, wood, etc
	string handle_material; // wood, etc
	// other member represent quality, health

	Equipment(string cust_name) : Craftable(cust_name) {}
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

class Weapon : public Equipment
{
public:
	Weapon(string cust_name) : Equipment(cust_name) {}
};

class Axe : public Weapon
{
public:
	Axe() : Weapon(C::AXE_ID) {}
};

class Bow : public Weapon
{
public:
	Bow() : Weapon(C::BOW_ID) {}
};

#endif
