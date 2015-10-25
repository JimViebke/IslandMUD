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

using namespace std;

class Recipes
{
public:
	map<string, Recipe> recipes; // name, recipe object

	Recipes() {}

	void load();

	// recipe population
	void add_recipe(const string & name,
		const map<string, int> & inv_n, const map<string, int> & inv_r,
		const map<string, int> & loc_n, const map<string, int> & loc_r, const map<string, int> & yields);

	// information
	bool has_recipe_for(const string & name) const;

	// retrieval
	Recipe get_recipe(const string & recipe_name) const;

	// debugging
	string get_recipes() const;

};

#endif
