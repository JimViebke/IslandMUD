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
private:
	const int island_radius = (int)((double)C::WORLD_X_DIMENSION * .45); // assumes full map is square
	const unsigned x_center = C::WORLD_X_DIMENSION / 2;
	const unsigned y_center = C::WORLD_Y_DIMENSION / 2;
	const int biome_size = 25;

	const char water = C::WATER_CHAR;
	const char land = C::LAND_CHAR;
	const char forest = C::FOREST_CHAR;

	stringstream generator_pattern; // reflects the steps used to reach a generated terrain

	string generated_terrain_dir; // the path to place the terrain

	vector<vector<char>> v1, v2, biome_map;

public:

	Generator()
	{
		cout << "\nGenerating new world terrain map...";

		srand((unsigned)time(NULL)); // seed rand
		
		// create the timestamped directory
		generated_terrain_dir = C::game_directory + "/gen " + R::to_string(R::current_time_in_ms());
		R::create_path_if_not_exists(generated_terrain_dir);

		// fill both working vectors with empty space
		size_vector(v1, C::WORLD_X_DIMENSION, C::WORLD_Y_DIMENSION);
		size_vector(v2, C::WORLD_X_DIMENSION, C::WORLD_Y_DIMENSION);
	}

	void generate_biome_map();
	void generate_static_using_biome_map();

	// load or generate terrain, depending on if it exists
	/* void load_terrain_from_file()
	{
	vector<vector<char>> v;
	if (R::file_exists(C::world_terrain_file_location))
	{
	ifstream f;
	f.open(C::world_terrain_file_location);
	for (string line; getline(f, line);)
	{
	vector<char> row;
	for (char c : line)
	{
	row.push_back(c);
	}
	v.push_back(row);
	}
	v1 = v;
	v2 = v;
	}

	// now test if that just worked
	if (v.size() == C::WORLD_X_DIMENSION &&
	v[0].size() == C::WORLD_Y_DIMENSION)
	{
	// what we loaded is good

	}
	else
	{
	// a new terrain file must be generated

	}
	} */
	/* void load_biome_map_from_file()
	{
	ifstream f;
	f.open(C::world_biome_file_location);

	vector<vector<char>> v;
	for (string line; getline(f, line);)
	{
	vector<char> row;
	for (char c : line)
	{
	row.push_back(c);
	}
	v.push_back(row);
	}

	biome_map = v;
	} */

	// different pass types to call manually
	void game_of_life(const int & iterations);
	void clean(const int & iterations);
	void fill(const int & iterations);

	// two optional ways of getting the final result, either by saving the terrain to the disk, or by retriving it manually
	void save_terrain() const;
	vector<vector<char>> get_terrain();

private:

	void size_vector(vector<vector<char>> & v, const int & x, const int & y);

	void save_current_terrain() const;
	void to_file(const vector<vector<char>> & v, const string & dir) const;
};

#endif
