/* Jim Viebke
April 14, 2015 */

#ifndef GENERATOR_H
#define GENERATOR_H

#include <array>
#include <fstream>
#include <ctime>

#include "constants.h"
#include "resources.h"

using namespace std;

class Generator
{
public:

	Generator(const string & map_type); // defined in class file

	// generate a miniature map of the biomes
	vector<vector<char_type>> generate_biome_map(const char_type & default_char, const char_type & fill_char,
		const int & fill_ratio, const int & default_ratio, const int & biome_size);

	// generate a full size map using static inside of each biome
	vector<vector<char_type>> generate_static_using_biome_map(const vector<vector<char_type>> & biome_map, const int & biome_size);

	// different pass types, all return by reference
	void game_of_life(vector<vector<char_type>> & original, const int & iterations);
	void clean(vector<vector<char_type>> & original, const int & iterations);
	void fill(vector<vector<char_type>> & original, const int & iterations);

	// save intermediate generated maps to /gen_[timestamp]/[pattern].txt
	void save_intermediate_map(const vector<vector<char_type>> & v) const;

	// save to custom location
	void to_file(const vector<vector<char_type>> & v, const string & path) const;

	string get_generator_pattern() const;
	string get_generated_terrain_dir() const;

private:

	const int island_radius = (int)((double)C::WORLD_X_DIMENSION * .45); // assumes full map is square
	const unsigned x_center = C::WORLD_X_DIMENSION / 2;
	const unsigned y_center = C::WORLD_Y_DIMENSION / 2;

	stringstream generator_pattern; // reflects the steps used to reach a generated terrain

	string generated_terrain_dir; // the path to place the terrain

};

#endif
