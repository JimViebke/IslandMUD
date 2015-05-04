/*
Jim Viebke
Feb 14, 2015 */

#ifndef PLAYER_CHARACTER_H
#define PLAYER_CHARACTER_H

#include "character.h"

class Player_Character : public Character
{
public:
	Player_Character() : Character()
	{

	}

	string print() const
	{
		stringstream output;

		// if the player is carrying any materials, list them above equipment
		if (material_inventory.size() > 0)
		{
			for (multimap<string, shared_ptr<Material>>::const_iterator item_it = material_inventory.cbegin(); // for each item
				item_it != material_inventory.cend(); ++item_it)
			{
				output << item_it->second->name << " (x" << item_it->second->amount << ") "; // add its name to the output: stick (x5)
			}
		}

		// if the player is carrying any equipment, list them after materials
		if (equipment_inventory.size() > 0) // if the player is carrying anything
		{
			for (multimap<string, shared_ptr<Equipment>>::const_iterator item_it = equipment_inventory.cbegin(); // for each item
				item_it != equipment_inventory.cend(); ++item_it)
			{
				output << item_it->second->name << " "; // add its name to the output
			}
		}

		return (output.str().size() > 0) ? // if there is anything to print
			"You have " + output.str() : // return the output
			"You aren't carrying anything."; // return generic "no items" message
	}
};

typedef Player_Character PC;

#endif
