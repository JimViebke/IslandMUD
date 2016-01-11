/* Jim Viebke
April 14, 2015 */

#ifndef GENERATOR_H
#define GENERATOR_H

#include <array>
#include <fstream>
#include <ctime>

#include "constants.h"
#include "utilities.h"

class Generator
{
public:

	Generator(const std::string & map_type); // defined in class file

	// generate a miniature map of the biomes
	std::vector<std::vector<char>> generate_biome_map(const char & default_char, const char & fill_char,
		const int & fill_ratio, const int & default_ratio, const int & biome_size);

	// generate a full size map using static inside of each biome
	std::vector<std::vector<char>> generate_static_using_biome_map(const std::vector<std::vector<char>> & biome_map, const int & biome_size,
		const char & empty_char, const char & fill_char);

	// different pass types, all return by reference
	void game_of_life(std::vector<std::vector<char>> & original, const int & iterations, const char & empty_char, const char & fill_char);
	void clean(std::vector<std::vector<char>> & original, const int & iterations, const char & empty_char, const char & fill_char);
	void fill(std::vector<std::vector<char>> & original, const int & iterations, const char & empty_char, const char & fill_char);

	// save intermediate generated maps to /gen_[timestamp]/[pattern].txt
	void save_intermediate_map(const std::vector<std::vector<char>> & v) const;

	// save to custom location
	void to_file(const std::vector<std::vector<char>> & v, const std::string & path) const;

	std::string get_generator_pattern() const;
	std::string get_generated_terrain_dir() const;

private:

	const int island_radius = (int)((double)C::WORLD_X_DIMENSION * .45); // assumes full map is square
	const unsigned x_center = C::WORLD_X_DIMENSION / 2;
	const unsigned y_center = C::WORLD_Y_DIMENSION / 2;

	std::stringstream generator_pattern; // reflects the steps used to reach a generated terrain

	std::string generated_terrain_dir; // the path to place the terrain

};

#endif
