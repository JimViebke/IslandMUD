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
	static std::shared_ptr<Item> make(const std::string & item_ID);

private:
	Craft() = delete;
};

#endif
