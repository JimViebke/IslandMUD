
/*
Jim Viebke
Dec 18 2015

Basic Material types for IslandMUD.
*/

#ifndef MATERIALS_H
#define MATERIALS_H

#include "item.h"
#include "../constants.h"

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

#endif
