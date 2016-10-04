
/*
Jim Viebke
Dec 18 2015

Basic Stackable types for IslandMUD.
*/

#ifndef STACKABLE_H
#define STACKABLE_H

#include "item.h"
#include "../constants.h"

class Stone : public Stackable
{
public:
	Stone() : Stackable(C::STONE_ID) {}
};

class Stick : public Stackable
{
public:
	Stick() : Stackable(C::STICK_ID) {}
};

class Branch : public Stackable // like a stick, but thicker
{
public:
	Branch() : Stackable(C::BRANCH_ID) {}
};

class Vine : public Stackable
{
public:
	Vine() : Stackable(C::VINE_ID) {}
};

class Wood : public Stackable
{
public:
	Wood() : Stackable(C::WOOD_ID) {}
};

class Arrow : public Stackable
{
public:
	Arrow() : Stackable(C::ARROW_ID) {}
};

class Arrowhead : public Stackable
{
public:
	Arrowhead() : Stackable(C::ARROWHEAD_ID) {}
};

class Board : public Stackable
{
public:
	Board() : Stackable(C::BOARD_ID) {}
};

#endif
