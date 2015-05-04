/* Jim Viebke
Feb 14, 2015 */

#ifndef CRAFT_H
#define CRAFT_H

#include <map>
#include <string>
#include <iostream>
#include <ostream>

#include "item.h"

using namespace std;

class Recipe
{
public:
	map<string, int> inventory_need; // needs to be in inventory
	map<string, int> inventory_remove; // needs to be in inventory, is removed
	map<string, int> local_need; // needs to be in the node
	map<string, int> local_remove; // needs to be in the node, is removed

	map<string, int> yields; // the item(s) the character recieves

	Recipe() {}
	Recipe(
		const map<string, int> & inv_n, const map<string, int> & inv_r,
		const map<string, int> & loc_n, const map<string, int> & loc_r,
		const map<string, int> & yeilds) :
		inventory_need(inv_n), inventory_remove(inv_r), local_need(loc_n), local_remove(loc_r), yields(yeilds)
	{

	}
};

class Recipes
{
public:
	map<string, Recipe> recipes; // name, recipe object

	Recipes()
	{
		populate();
	}

	// generation
	void populate()
	{
		// for documentation, refer to add()

		cout << "\nLoading crafting recipes...";

		// add("recipe name", {}, {}, {}, {}, {});


		// raw materials
		add("stick", {}, {}, { { C::TREE_ID, 1 } }, {}, { { C::STICK_ID, 1 } });
		add("branch", {}, {}, { { C::TREE_ID, 1 } }, {}, { { C::BRANCH_ID, 1 } });
		add("vine", {}, {}, { { C::TREE_ID, 1 } }, {}, { { C::VINE_ID, 1 } });
		add("stone", {}, {}, {}, {}, { { C::STONE_ID, 1 } }); // you can always get a stone

		// equipment
		add("staff", { { C::AXE_ID, 1 } }, { { C::BRANCH_ID, 1 } }, {}, {}, { { C::STAFF_ID, 1 } });
		add("axe", {}, { { C::BRANCH_ID, 1 }, { C::STONE_ID, 1 }, { C::VINE_ID, 1 } }, {}, {}, { { C::AXE_ID, 1 } }); // maybe need a sharp stone?
		add("bow", {}, { { C::BRANCH_ID, 1 }, { C::VINE_ID, 1 } }, {}, {}, { { C::BOW_ID, 1 } }); // maybe need a sharp stone?
	}
	void add(const string & name,
		const map<string, int> & inv_n, const map<string, int> & inv_r,
		const map<string, int> & loc_n, const map<string, int> & loc_r, const map<string, int> & yields)
	{
		/* arguments in order:
		recipe name
		map of item IDs that need to be in inventory
		map of item IDs that need to be in inventory, are removed
		map of item IDs that need to be in the node
		map of item IDs that need to be in the node, are removed
		map of item IDs to be yielded */

		this->recipes.insert(pair<string, Recipe>(name, Recipe(inv_n, inv_r, loc_n, loc_r, yields)));
	}

	// information
	bool has_recipe_for(const string & name) const
	{
		return this->recipes.find(name) != this->recipes.cend();
	}

	// retrieval
	Recipe get_recipe(const string & recipe_name) const
	{
		return this->recipes.find(recipe_name)->second;
	}

	// debugging
	string get_recipes() const
	{
		stringstream output;
		for (auto & recipe : recipes)
		{
			output << "Recipe name: " << recipe.first << endl;
			if (recipe.second.inventory_need.size() > 0)
			{
				output << "Requirements:\n";
				for (map<string, int>::const_iterator it = recipe.second.inventory_need.cbegin(); it != recipe.second.inventory_need.cend(); ++it)
				{
					output << "\t" << it->second << " " << it->first << endl; // 5 stick
				}
			}
			if (recipe.second.inventory_remove.size() > 0)
			{
				output << "Requirements that will be removed from your inventory:\n";
				for (map<string, int>::const_iterator it = recipe.second.inventory_remove.cbegin(); it != recipe.second.inventory_remove.cend(); ++it)
				{
					output << "\t" << it->second << " " << it->first << endl; // 5 stick
				}
			}
			if (recipe.second.local_need.size() > 0)
			{
				output << "Local requirements:\n";
				for (map<string, int>::const_iterator it = recipe.second.local_need.cbegin(); it != recipe.second.local_need.cend(); ++it)
				{
					output << "\t" << it->second << " " << it->first << endl; // 5 stick
				}
			}
			if (recipe.second.local_remove.size() > 0)
			{
				output << "Local requirements that will be removed from the area:\n";
				for (map<string, int>::const_iterator it = recipe.second.local_remove.cbegin(); it != recipe.second.local_remove.cend(); ++it)
				{
					output << "\t" << it->second << " " << it->first << endl; // 5 stick
				}
			}
			output << endl;
		}
		return output.str();
	}

};



class Craft
{
public:

	Craft()	{ }

	static shared_ptr<Item> make(const string & item_id);
};

#endif
