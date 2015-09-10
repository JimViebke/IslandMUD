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

	stringstream generator_pattern; // reflects the steps used to reach a generated terrain

	string generated_terrain_dir; // the path to place the terrain

	vector<vector<char_type>> v1, v2, biome_map;

public:

	Generator()
	{
		cout << "\nGenerating new world terrain map...";

		srand((unsigned)time(NULL)); // seed rand
		
		// create the timestamped directory
		generated_terrain_dir = C::game_directory + "/gen_" + R::to_string(R::current_time_in_ms());
		R::create_path_if_not_exists(generated_terrain_dir);

		// fill both working vectors with empty space
		size_vector(v1, C::WORLD_X_DIMENSION, C::WORLD_Y_DIMENSION);
		size_vector(v2, C::WORLD_X_DIMENSION, C::WORLD_Y_DIMENSION);
	}

	void generate_biome_map();
	void generate_static_using_biome_map();

	// different pass types to call manually
	void game_of_life(const int & iterations);
	void clean(const int & iterations);
	void fill(const int & iterations);

	// two optional ways of getting the final result, either by saving the terrain to the disk, or by retriving it manually
	void save_terrain() const;
	vector<vector<char_type>> get_terrain();

private:

	void size_vector(vector<vector<char_type>> & v, const int & x, const int & y);

	void save_current_terrain() const;
	void to_file(const vector<vector<char_type>> & v, const string & dir) const;
};

#endif
