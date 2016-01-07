/* Jim Viebke
Jan 6 2016

Tables for IslandMUD */

#ifndef TABLE_H
#define TABLE_H

#include "item.h"
#include "container.h"

class Table : public Item, public Container
{
public:
	Table();
};

#endif
