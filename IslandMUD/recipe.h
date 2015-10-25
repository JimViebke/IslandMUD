/* Jim Viebke
May 15 2015 */

#ifndef RECIPE_H
#define RECIPE_H

#include <map>
#include <string>

using namespace std;

class Recipe
{
private:
	Recipe() = delete; // this is deliberately not implemented

public:
	map<string, int> inventory_need; // needs to be in inventory
	map<string, int> inventory_remove; // needs to be in inventory, is removed
	map<string, int> local_need; // needs to be in the room
	map<string, int> local_remove; // needs to be in the room, is removed

	map<string, int> yields; // the item(s) the character recieves

	Recipe(
		const map<string, int> & inv_n, const map<string, int> & inv_r,
		const map<string, int> & loc_n, const map<string, int> & loc_r,
		const map<string, int> & yeilds) :
		inventory_need(inv_n), inventory_remove(inv_r), local_need(loc_n), local_remove(loc_r), yields(yeilds) {}
};

#endif
