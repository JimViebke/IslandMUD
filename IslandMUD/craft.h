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
	static shared_ptr<Item> make(const string & item_ID);

private:
	Craft() = delete;
};

#endif
