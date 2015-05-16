/* Jim Viebke
Feb 14, 2015 */

#ifndef CRAFT_H
#define CRAFT_H

#include <iostream>
#include <ostream>

#include "item.h"

class Craft
{
public:

	Craft()	{ }

	static shared_ptr<Item> make(const string & item_id);
};

#endif
