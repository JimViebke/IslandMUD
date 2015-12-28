
/*
Jim Viebke
Dec 18 2015

Basic Equipment types for IslandMUD.
*/

#ifndef EQUIPMENT_H
#define EQUIPMENT_H

#include "item.h"
#include "../constants.h"

class Equipment : public Item
{
protected:
	Equipment(const std::string & name) : Item(name, true) {}
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

#endif
