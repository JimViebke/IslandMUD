/* Jim Viebke
May 15 2015 */

#ifndef RECIPES_H
#define RECIPES_H

#include <map>
#include <string>
#include <iostream>
#include <sstream>

#include "recipe.h"
#include "constants.h"

class Recipes
{
public:
	Recipes();

	// information
	bool has_recipe_for(const std::string & name) const;

	// retrieval
	Recipe get_recipe(const std::string & recipe_name) const;

	// debugging
	std::string get_recipes() const;
	std::string get_recipes_matching(const std::string & item_ID) const;

private:
	std::map<std::string, Recipe> recipes; // name, recipe object

	void load();

	// recipe population
	void add_recipe(const std::string & name,
		const std::map<std::string, int> & inv_n, const std::map<std::string, int> & inv_r,
		const std::map<std::string, int> & loc_n, const std::map<std::string, int> & loc_r, const std::map<std::string, int> & yields);
};

#endif
