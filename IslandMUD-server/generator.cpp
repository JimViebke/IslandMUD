/* Jim Viebke
May 15 2015 */

#include "generator.h"

void Generator::generate_biome_map()
{
	// reset/reserve vector to be full of spaces
	size_vector(biome_map, C::WORLD_X_DIMENSION / biome_size, C::WORLD_Y_DIMENSION / biome_size);

	for (int i = 0; i < (C::WORLD_X_DIMENSION / biome_size); ++i)
	{
		for (int j = 0; j < (C::WORLD_Y_DIMENSION / biome_size); ++j)
		{
			biome_map[i][j] = ((rand() % 3 == 1) ? land : forest); // 1/3 land biome, 2/3 forest biome
		}
	}

	// update the pattern
	generator_pattern << "B.66"; // Biomes are ~66% forest

	// save the biome map to file
	to_file(biome_map, generated_terrain_dir + "/biome_map.txt");
}
void Generator::generate_static_using_biome_map()
{
	for (unsigned x = 0; x < v2.size(); ++x)
	{
		for (unsigned y = 0; y < v2[0].size(); ++y)
		{
			// if the coordinates are not within the islands radius
			if (R::euclidean_distance(x, y, x_center, y_center) > island_radius)
			{
				v2[x][y] = water;
			}
			// if the biome map contains a land char
			else if (biome_map[x / biome_size][y / biome_size] == land)
			{
				v2[x][y] = land;
			}
			// the biome map contains a forest char, generate forest/land
			else
			{
				// push back forest or land
				v2[x][y] = (rand() % 4) ? land : forest;
			}
		}
	}

	// copy over
	v1 = v2;

	// save static
	to_file(v2, generated_terrain_dir + "/static.txt");
}

// different pass types to call manually
void Generator::game_of_life(const int & iterations)
{
	for (int pass = 0; pass < iterations; ++pass)
	{
		// for each row (excluding a 1-thick border)
		for (int i = 1; i < C::WORLD_X_DIMENSION - 1; ++i)
		{
			// for each room in the room (excluding a 1-thick border
			for (int j = 0; j < C::WORLD_Y_DIMENSION - 1; ++j)
			{
				// if the current cell/room is water, skip it
				if (v2[i][j] == water)
				{
					continue;
				}

				// for every land cell, count its forest neighours (sp?)
				int live_neighbors = 0;
				// count the row above
				if (v1[i - 1][j - 1] == forest) { ++live_neighbors; }
				if (v1[i - 1][j] == forest) { ++live_neighbors; }
				if (v1[i - 1][j + 1] == forest) { ++live_neighbors; }
				// count the left and right neighers
				if (v1[i][j - 1] == forest) { ++live_neighbors; }
				if (v1[i][j + 1] == forest) { ++live_neighbors; }
				// count the row below
				if (v1[i + 1][j - 1] == forest) { ++live_neighbors; }
				if (v1[i + 1][j] == forest) { ++live_neighbors; }
				if (v1[i + 1][j + 1] == forest) { ++live_neighbors; }

				/* Wikipedia:
				Any live cell with fewer than two live neighbours dies, as if caused by under-population.
				Any live cell with two or three live neighbours lives on to the next generation.
				Any live cell with more than three live neighbours dies, as if by overcrowding.
				Any dead cell with exactly three live neighbours becomes a live cell, as if by reproduction.
				*/

				if (v2[i][j] == forest) // any living cell...
				{
					if (live_neighbors < 2 || live_neighbors > 3) // ...with <2 or >3 neighbors...
					{
						v2[i][j] = land; // ...dies
					}
				}
				else if (live_neighbors == 3) // a dead cell with 3 neighbors...
				{
					v2[i][j] = forest; // ...is born
				}
			}
		}

		// copy the working vector back into the original vector for the next iteration
		v1 = v2;
	} // end pass forloop

	// save
	generator_pattern << iterations << "C+";
	save_current_terrain();

}
void Generator::clean(const int & iterations)
{
	for (int pass = 0; pass < iterations; ++pass)
	{
		// for each row (excluding a 1-thick border)
		for (int i = 1; i < C::WORLD_X_DIMENSION - 1; ++i)
		{
			// for each room in the room (excluding a 1-thick border
			for (int j = 0; j < C::WORLD_Y_DIMENSION - 1; ++j)
			{
				// if the current cell/room is water, skip it
				if (v2[i][j] == water)
				{
					continue;
				}

				// for every land cell, count its forest neighours (sp?)
				int live_neighbors = 0;
				// count the row above
				if (v1[i - 1][j - 1] == forest) { ++live_neighbors; }
				if (v1[i - 1][j] == forest) { ++live_neighbors; }
				if (v1[i - 1][j + 1] == forest) { ++live_neighbors; }
				// count the left and right neighers
				if (v1[i][j - 1] == forest) { ++live_neighbors; }
				if (v1[i][j + 1] == forest) { ++live_neighbors; }
				// count the row below
				if (v1[i + 1][j - 1] == forest) { ++live_neighbors; }
				if (v1[i + 1][j] == forest) { ++live_neighbors; }
				if (v1[i + 1][j + 1] == forest) { ++live_neighbors; }

				if (live_neighbors < 2)
				{
					v2[i][j] = land; // dies
				}
			}
		}

		// copy the working vector back into the original vector for the next iteration
		v1 = v2;

		// update generator patter for each generation
		generator_pattern << "C";
	}
	save_current_terrain();
}
void Generator::fill(const int & iterations)
{
	for (int pass = 0; pass < iterations; ++pass)
	{
		// for each row (excluding a 1-thick border)
		for (int i = 1; i < C::WORLD_X_DIMENSION - 1; ++i)
		{
			// for each room in the room (excluding a 1-thick border
			for (int j = 0; j < C::WORLD_Y_DIMENSION - 1; ++j)
			{
				// if the current cell/room is water, skip it
				if (v2[i][j] == water)
				{
					continue;
				}

				// for every land cell, count its forest neighours (sp?)
				int live_neighbors = 0;
				// count the row above
				if (v1[i - 1][j - 1] == forest) { ++live_neighbors; }
				if (v1[i - 1][j] == forest) { ++live_neighbors; }
				if (v1[i - 1][j + 1] == forest) { ++live_neighbors; }
				// count the left and right neighers
				if (v1[i][j - 1] == forest) { ++live_neighbors; }
				if (v1[i][j + 1] == forest) { ++live_neighbors; }
				// count the row below
				if (v1[i + 1][j - 1] == forest) { ++live_neighbors; }
				if (v1[i + 1][j] == forest) { ++live_neighbors; }
				if (v1[i + 1][j + 1] == forest) { ++live_neighbors; }

				if (live_neighbors > 3)
				{
					v2[i][j] = forest; // birth or survival
				}
			}
		}

		// copy the working vector back into the original vector for the next iteration
		v1 = v2;

		// upate generator pattern
		generator_pattern << "F";
	}
	save_current_terrain();
}

// two optional ways of getting the final result, either by saving the terrain to the disk, or by retriving it manually
void Generator::save_terrain() const
{
	to_file(v2, C::world_terrain_file_location);
}
vector<vector<char>> Generator::get_terrain()
{
	return v2; // return generated world
}



/* Private member functions */



void Generator::size_vector(vector<vector<char>> & v, const int & x, const int & y)
{
	// this should be the only place in this class that a vector is sized		

	vector<vector<char>> sized_vector;
	sized_vector.reserve(x);

	for (int i = 0; i < x; ++i)
	{
		vector<char> row;
		row.reserve(y);

		for (int j = 0; j < y; ++j)
		{
			row.push_back(' ');
		}

		sized_vector.push_back(row);
	}

	// overwrite passed-in vector result
	v = sized_vector;
}

void Generator::save_current_terrain() const
{
	to_file(v2, generated_terrain_dir + "/" + generator_pattern.str() + ".txt");
}
void Generator::to_file(const vector<vector<char>> & v, const string & dir) const
{
	ostringstream iss;
	for (unsigned i = 0; i < v.size(); ++i)
	{
		for (unsigned j = 0; j < v[0].size(); ++j)
		{
			iss << v[i][j];
		}
		iss << endl;
	}

	// save to disk
	ofstream myfile;
	myfile.open(dir);
	myfile << iss.str();
	myfile.close();
}
