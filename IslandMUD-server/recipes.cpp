/* Jim Viebke
May 15 2015 */

#include "recipes.h"

void Recipes::load()
{
	// for documentation, refer to add_recipe() below

	cout << "\nLoading crafting recipes...";

	// add_recipe("recipe name", {}, {}, {}, {}, {});

	// materials
	add_recipe(C::STICK_ID, {}, {}, { { C::TREE_ID, 1 } }, {}, { { C::STICK_ID, 1 } });
	add_recipe(C::BRANCH_ID, {}, {}, { { C::TREE_ID, 1 } }, {}, { { C::BRANCH_ID, 1 } });
	add_recipe(C::VINE_ID, {}, {}, { { C::TREE_ID, 1 } }, {}, { { C::VINE_ID, 1 } });
	add_recipe(C::STONE_ID, {}, {}, {}, {}, { { C::STONE_ID, 1 } }); // you can always get a stone
	add_recipe(C::ARROW_ID, {}, { { C::ARROWHEAD_ID, 1 }, { C::STICK_ID, 1 } }, {}, {}, { { C::ARROW_ID, 1 } }); // a stick and an arrowhead
	add_recipe(C::ARROWHEAD_ID, { { C::STONE_ID, 2 } }, { { C::STONE_ID, 1 } }, {}, {}, { { C::ARROWHEAD_ID, 3 } }); // requires two stones but only one is made into arrowheads

	// equipment
	add_recipe(C::STAFF_ID, { { C::AXE_ID, 1 } }, { { C::BRANCH_ID, 1 } }, {}, {}, { { C::STAFF_ID, 1 } });
	add_recipe(C::AXE_ID, {}, { { C::BRANCH_ID, 1 }, { C::STONE_ID, 1 }, { C::VINE_ID, 1 } }, {}, {}, { { C::AXE_ID, 1 } }); // maybe need a sharp stone?
	add_recipe(C::BOW_ID, {}, { { C::BRANCH_ID, 1 }, { C::VINE_ID, 1 } }, {}, {}, { { C::BOW_ID, 1 } });
	add_recipe(C::SWORD_ID, {}, { { C::STONE_ID, 2 }, { C::STICK_ID, 1 } }, {}, {}, { { C::SWORD_ID, 1 } });

	// other
	add_recipe(C::FORGE_ID, {}, { { C::STONE_ID, 20 } }, {}, {}, { { C::FORGE_ID, 1 } }); // 20 stones to build an immovable forge 

}

// recipe population
void Recipes::add_recipe(const string & name,
	const map<string, int> & inv_n, const map<string, int> & inv_r,
	const map<string, int> & loc_n, const map<string, int> & loc_r, const map<string, int> & yields)
{
	/* arguments in order:
	recipe name
	map of item IDs that need to be in inventory
	map of item IDs that need to be in inventory, are removed
	map of item IDs that need to be in the room
	map of item IDs that need to be in the room, are removed
	map of item IDs to be yielded */

	this->recipes.insert(pair<string, Recipe>(name, Recipe(inv_n, inv_r, loc_n, loc_r, yields)));
}

// information
bool Recipes::has_recipe_for(const string & name) const
{
	return this->recipes.find(name) != this->recipes.cend();
}

// retrieval
Recipe Recipes::get_recipe(const string & recipe_name) const
{
	return this->recipes.find(recipe_name)->second;
}

// debugging
string Recipes::get_recipes() const
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
