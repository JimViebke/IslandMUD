/* Jim Viebke
May 15 2015 */

#ifndef RECIPE_H
#define RECIPE_H

#include <map>
#include <string>

class Recipe
{
private:
	Recipe() = delete; // this is deliberately not implemented

public:
	std::map<std::string, int> inventory_need; // needs to be in inventory
	std::map<std::string, int> inventory_remove; // needs to be in inventory, is removed
	std::map<std::string, int> local_need; // needs to be in the room
	std::map<std::string, int> local_remove; // needs to be in the room, is removed

	std::map<std::string, int> yields; // the item(s) the character recieves

	Recipe(
		const std::map<std::string, int> & inv_n, const std::map<std::string, int> & inv_r,
		const std::map<std::string, int> & loc_n, const std::map<std::string, int> & loc_r,
		const std::map<std::string, int> & yeilds) :
		inventory_need(inv_n), inventory_remove(inv_r), local_need(loc_n), local_remove(loc_r), yields(yeilds) {}
};

#endif
