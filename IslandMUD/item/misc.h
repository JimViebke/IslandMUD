
/*
Jim Viebke
Dec 19 2015

Miscellaneous Item classes. */

#ifndef MISC_ITEMS_H
#define MISC_ITEMS_H

#include "item.h"

class Anvil : public Item
{
public:
	Anvil() : Item(C::ANVIL_ID, false) {}
};

class Debris : public Item
{
public:
	Debris() : Item(C::DEBRIS_ID, false) {}
};

class Log : public Item
{
public:
	Log() : Item(C::LOG_ID, false) {}
};

class Tree : public Item
{
public:
	Tree() : Item(C::TREE_ID, false) {}
};

#endif
