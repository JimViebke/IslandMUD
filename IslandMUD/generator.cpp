/* Jim Viebke
May 15 2015 */

#include "generator.h"

Generator::Generator(const string & map_type)
{
	cout << "\nGenerating new " << map_type << "...";

	// create the timestamped directory
	generated_terrain_dir = C::game_directory + "/gen_" + U::to_string(U::current_time_in_ms());
	U::create_path_if_not_exists(generated_terrain_dir);
}

vector<vector<char_type>> Generator::generate_biome_map(const char_type & default_char, const char_type & fill_char, const int & fill_ratio, const int & default_ratio, const int & biome_size)
{
	vector<vector<char_type>> biome_map(C::WORLD_X_DIMENSION / biome_size, vector<char_type>(C::WORLD_Y_DIMENSION / biome_size,
#ifdef _WIN32
		' '
#else
		" "
#endif
		));

	// generate the biome map
	for (int i = 0; i < (C::WORLD_X_DIMENSION / biome_size); ++i)
	{
		for (int j = 0; j < (C::WORLD_Y_DIMENSION / biome_size); ++j)
		{
			biome_map[i][j] = (
				(U::random_int_from(1, max(1, fill_ratio + default_ratio)) <= fill_ratio) ? fill_char : default_char);
		}
	}

	// update the pattern
	generator_pattern << "B.66"; // Biomes are ~66% forest

	// save the biome map to file
	to_file(biome_map, generated_terrain_dir + "/biome_map.txt");

	return biome_map; // this will be used for the next generation step
}
vector<vector<char_type>> Generator::generate_static_using_biome_map(const vector<vector<char_type>> & biome_map, const int & biome_size,
	const char_type & empty_char, const char_type & fill_char)
{
	vector<vector<char_type>> v(C::WORLD_X_DIMENSION, vector<char_type>(C::WORLD_Y_DIMENSION,
#ifdef _WIN32
		' '
#else
		" "
#endif
		));

	for (unsigned x = 0; x < v.size(); ++x)
	{
		for (unsigned y = 0; y < v[0].size(); ++y)
		{
			// if the coordinates are not within the islands radius
			if (U::euclidean_distance(x, y, x_center, y_center) > island_radius)
			{
				v[x][y] = C::WATER_CHAR;
			}
			// if the biome map contains a land char
			else if (biome_map[x / biome_size][y / biome_size] == empty_char)
			{
				v[x][y] = empty_char;
			}
			// the biome map contains a forest char, generate forest/land
			else
			{
				// push back forest or land (again, hardcoding static here)
				v[x][y] = (rand() % 4) ? empty_char : fill_char;
			}
		}
	}

	return v; // this will be used for the next generation step
}

// different pass types to call manually
void Generator::game_of_life(vector<vector<char_type>> & original, const int & iterations, const char_type & empty_char, const char_type & fill_char)
{
	vector<vector<char_type>> working = original; // copy to start

	for (int pass = 0; pass < iterations; ++pass)
	{
		// for each row (excluding a 1-thick border)
		for (int i = 1; i < C::WORLD_X_DIMENSION - 1; ++i)
		{
			// for each room in the room (excluding a 1-thick border
			for (int j = 0; j < C::WORLD_Y_DIMENSION - 1; ++j)
			{
				// if the current cell/room is water, skip it
				if (original[i][j] == C::WATER_CHAR)
				{
					continue;
				}

				// for every land cell, count its forest neighours (sp?)
				int live_neighbors = 0;
				// count the row above
				if (original[i - 1][j - 1] == fill_char) { ++live_neighbors; }
				if (original[i - 1][j] == fill_char) { ++live_neighbors; }
				if (original[i - 1][j + 1] == fill_char) { ++live_neighbors; }
				// count the left and right neighers
				if (original[i][j - 1] == fill_char) { ++live_neighbors; }
				if (original[i][j + 1] == fill_char) { ++live_neighbors; }
				// count the row below
				if (original[i + 1][j - 1] == fill_char) { ++live_neighbors; }
				if (original[i + 1][j] == fill_char) { ++live_neighbors; }
				if (original[i + 1][j + 1] == fill_char) { ++live_neighbors; }

				/* Wikipedia:
				Any live cell with fewer than two live neighbours dies, as if caused by under-population.
				Any live cell with two or three live neighbours lives on to the next generation.
				Any live cell with more than three live neighbours dies, as if by overcrowding.
				Any dead cell with exactly three live neighbours becomes a live cell, as if by reproduction.
				*/

				if (working[i][j] == fill_char) // any living cell...
				{
					if (live_neighbors < 2 || live_neighbors > 3) // ...with <2 or >3 neighbors...
					{
						working[i][j] = empty_char; // ...dies
					}
				}
				else if (live_neighbors == 3) // a dead cell with 3 neighbors...
				{
					working[i][j] = fill_char; // ...is born
				}
			}
		}

		// copy the working vector back into the original vector for the next iteration
		original = working;
	} // end pass forloop

	// save
	generator_pattern << iterations << "C+";
}
void Generator::clean(vector<vector<char_type>> & original, const int & iterations, const char_type & empty_char, const char_type & fill_char)
{
	vector<vector<char_type>> working = original; // copy to start

	for (int pass = 0; pass < iterations; ++pass)
	{
		// for each row (excluding a 1-thick border)
		for (int i = 1; i < C::WORLD_X_DIMENSION - 1; ++i)
		{
			// for each room in the room (excluding a 1-thick border
			for (int j = 0; j < C::WORLD_Y_DIMENSION - 1; ++j)
			{
				// if the current cell/room is water, skip it
				if (working[i][j] == C::WATER_CHAR)
				{
					continue;
				}

				// for every land cell, count its forest neighours (sp?)
				int live_neighbors = 0;
				// count the row above
				if (original[i - 1][j - 1] == fill_char) { ++live_neighbors; }
				if (original[i - 1][j] == fill_char) { ++live_neighbors; }
				if (original[i - 1][j + 1] == fill_char) { ++live_neighbors; }
				// count the left and right neighers
				if (original[i][j - 1] == fill_char) { ++live_neighbors; }
				if (original[i][j + 1] == fill_char) { ++live_neighbors; }
				// count the row below
				if (original[i + 1][j - 1] == fill_char) { ++live_neighbors; }
				if (original[i + 1][j] == fill_char) { ++live_neighbors; }
				if (original[i + 1][j + 1] == fill_char) { ++live_neighbors; }

				if (live_neighbors < 2)
				{
					working[i][j] = empty_char; // dies
				}
			}
		}

		// copy the working vector back into the original vector for the next iteration
		original = working;

		// update generator pattern for each generation
		generator_pattern << "C";
	}
}
void Generator::fill(vector<vector<char_type>> & original, const int & iterations, const char_type & empty_char, const char_type & fill_char)
{
	vector<vector<char_type>> working = original; // copy to start

	for (int pass = 0; pass < iterations; ++pass)
	{
		// for each row (excluding a 1-thick border)
		for (int i = 1; i < C::WORLD_X_DIMENSION - 1; ++i)
		{
			// for each room in the room (excluding a 1-thick border
			for (int j = 0; j < C::WORLD_Y_DIMENSION - 1; ++j)
			{
				// if the current cell/room is water, skip it
				if (working[i][j] == C::WATER_CHAR)
				{
					continue;
				}

				// for every land cell, count its forest neighours (sp?)
				int live_neighbors = 0;
				// count the row above
				if (original[i - 1][j - 1] == fill_char) { ++live_neighbors; }
				if (original[i - 1][j] == fill_char) { ++live_neighbors; }
				if (original[i - 1][j + 1] == fill_char) { ++live_neighbors; }
				// count the left and right neighers
				if (original[i][j - 1] == fill_char) { ++live_neighbors; }
				if (original[i][j + 1] == fill_char) { ++live_neighbors; }
				// count the row below
				if (original[i + 1][j - 1] == fill_char) { ++live_neighbors; }
				if (original[i + 1][j] == fill_char) { ++live_neighbors; }
				if (original[i + 1][j + 1] == fill_char) { ++live_neighbors; }

				if (live_neighbors > 3)
				{
					working[i][j] = fill_char; // birth or survival
				}
			}
		}

		// copy the working vector back into the original vector for the next iteration
		original = working;

		// upate generator pattern
		generator_pattern << "F";
	}
}

// save intermediate generated maps to /gen_[timestamp]/[pattern].txt
void Generator::save_intermediate_map(const vector<vector<char_type>> & v) const
{
	to_file(v, generated_terrain_dir + "/" + generator_pattern.str() + ".txt");
}

// save to custom location
void Generator::to_file(const vector<vector<char_type>> & v, const string & path) const
{
	// copy the entire vector into a stringstream
	ostringstream oss;
	for (unsigned i = 0; i < v.size(); ++i)
	{
		for (unsigned j = 0; j < v[0].size(); ++j)
		{
			oss << v[i][j];
		}
		oss << endl;
	}

	// save the stringstream to the specified path
	ofstream myfile;
	myfile.open(path);
	myfile << oss.str();
	myfile.close(); // with RAII this shouldn't be necessary
}

string Generator::get_generator_pattern() const
{
	return generator_pattern.str();
}
string Generator::get_generated_terrain_dir() const
{
	return generated_terrain_dir;
}
