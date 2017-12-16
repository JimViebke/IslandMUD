/* Jim Viebke
May 15 2015 */

#include "generator.h"

Generator::Generator(const std::string & map_type)
{
	std::stringstream ss;
	ss << "Generating new " << map_type << "...\n";
	std::cout << ss.str();

	// create the timestamped directory
	generated_terrain_dir = C::game_directory + "/gen_" + U::to_string(U::current_time_in_ms()) + " (" + map_type + ")";
	U::create_path_if_not_exists(generated_terrain_dir);
}

std::vector<char> Generator::generate_biome_map(const char & default_char, const char & fill_char, const int & fill_ratio, const int & default_ratio, const int & biome_size)
{
	std::vector<char> biome_map((C::WORLD_X_DIMENSION / biome_size) * (C::WORLD_Y_DIMENSION / biome_size), ' ');

	// generate the biome map
	for (char & biome : biome_map)
	{
		biome = ((U::random_int_from(1, std::max(1, fill_ratio + default_ratio)) <= fill_ratio) ? fill_char : default_char);
	}

	// update the pattern
	generator_pattern << "B.66"; // Biomes are ~66% forest

	// save the biome map to file
	to_file(biome_map, (C::WORLD_X_DIMENSION / biome_size), generated_terrain_dir + "/biome_map.txt");

	return biome_map; // this will be used for the next generation step
}
std::vector<char> Generator::generate_static_using_biome_map(const std::vector<char> & biome_map, const int & biome_size,
	const char & empty_char, const char & fill_char)
{
	std::vector<char> output(C::WORLD_X_DIMENSION * C::WORLD_Y_DIMENSION, ' ');

	const char * biome_map_ptr = biome_map.data();
	char * output_ptr = output.data();

	for (unsigned x = 0; x < (unsigned)C::WORLD_X_DIMENSION; ++x)
	{
		for (unsigned y = 0; y < (unsigned)C::WORLD_Y_DIMENSION; ++y)
		{
			// if the coordinates are not within the islands radius
			if (U::euclidean_distance(x, y, x_center, y_center) > island_radius)
			{
				output_ptr[hash(x, y)] = C::WATER_CHAR;
			}
			// if the biome map contains a land char      Here we're scaling all three values (x, y_max, and y) by biome_size
			else if (biome_map_ptr[((x / biome_size) * (C::WORLD_Y_DIMENSION / biome_size)) + y / biome_size] == empty_char)
			{
				output_ptr[hash(x, y)] = empty_char;
			}
			// the biome map contains a forest char, generate forest/land
			else
			{
				// push back forest or land (again, hardcoding static here)
				output_ptr[hash(x, y)] = (rand() % 4) ? empty_char : fill_char;
			}
		}
	}

	return output; // this will be used for the next generation step
}

// different pass types to call manually
void Generator::game_of_life(std::vector<char> & original, const int & iterations, const char & empty_char, const char & fill_char)
{
	std::vector<char> working = original; // copy to start

	for (int pass = 0; pass < iterations; ++pass)
	{
		const char * original_ptr = original.data();
		char * working_ptr = working.data();

		// for each row (excluding a 1-thick border)
		for (int i = 1; i < C::WORLD_X_DIMENSION - 1; ++i)
		{
			// for each room in the room (excluding a 1-thick border
			for (int j = 1; j < C::WORLD_Y_DIMENSION - 1; ++j)
			{
				// if the current cell/room is water, skip it
				if (original_ptr[hash(i, j)] == C::WATER_CHAR) continue;

				// for every land cell, count its forest neighours (sp?)
				unsigned live_neighbors = 0;
				// count the row above
				live_neighbors += (original_ptr[hash(i - 1, j - 1)] == fill_char);
				live_neighbors += (original_ptr[hash(i - 1, j)] == fill_char);
				live_neighbors += (original_ptr[hash(i - 1, j + 1)] == fill_char);
				// count the left and right neighbors
				live_neighbors += (original_ptr[hash(i, j - 1)] == fill_char);
				live_neighbors += (original_ptr[hash(i, j + 1)] == fill_char);
				// count the row below
				live_neighbors += (original_ptr[hash(i + 1, j - 1)] == fill_char);
				live_neighbors += (original_ptr[hash(i + 1, j)] == fill_char);
				live_neighbors += (original_ptr[hash(i + 1, j + 1)] == fill_char);

				/* Wikipedia:
				Any live cell with fewer than two live neighbours dies, as if caused by under-population.
				Any live cell with two or three live neighbours lives on to the next generation.
				Any live cell with more than three live neighbours dies, as if by overcrowding.
				Any dead cell with exactly three live neighbours becomes a live cell, as if by reproduction.
				*/

				char & current_cell = working_ptr[hash(i, j)];

				if (current_cell == fill_char) // any living cell...
				{
					if (live_neighbors < 2 || live_neighbors > 3) // ...with <2 or >3 neighbors...
					{
						current_cell = empty_char; // ...dies
					}
				}
				else if (live_neighbors == 3) // a dead cell with 3 neighbors...
				{
					current_cell = fill_char; // ...is born
				}
			}
		}

		// copy the working vector back into the original vector for the next iteration
		original = working;
	} // end pass forloop

	// save
	generator_pattern << iterations << "C+";
}
void Generator::clean(std::vector<char> & original, const int & iterations, const char & empty_char, const char & fill_char)
{
	std::vector<char> working = original; // copy to start

	for (int pass = 0; pass < iterations; ++pass)
	{
		const char * original_ptr = original.data();
		char * working_ptr = working.data();

		// for each row (excluding a 1-thick border)
		for (int i = 1; i < C::WORLD_X_DIMENSION - 1; ++i)
		{
			// for each room in the room (excluding a 1-thick border
			for (int j = 0; j < C::WORLD_Y_DIMENSION - 1; ++j)
			{
				// if the current cell/room is water, skip it
				if (working_ptr[hash(i, j)] == C::WATER_CHAR) continue;

				// for every land cell, count its forest neighours (sp?)
				int live_neighbors = 0;
				// count the row above
				live_neighbors += (original_ptr[hash(i - 1, j - 1)] == fill_char);
				live_neighbors += (original_ptr[hash(i - 1, j)] == fill_char);
				live_neighbors += (original_ptr[hash(i - 1, j + 1)] == fill_char);
				// count the left and right neighbors
				live_neighbors += (original_ptr[hash(i, j - 1)] == fill_char);
				live_neighbors += (original_ptr[hash(i, j + 1)] == fill_char);
				// count the row below
				live_neighbors += (original_ptr[hash(i + 1, j - 1)] == fill_char);
				live_neighbors += (original_ptr[hash(i + 1, j)] == fill_char);
				live_neighbors += (original_ptr[hash(i + 1, j + 1)] == fill_char);

				if (live_neighbors < 2)
				{
					working_ptr[hash(i, j)] = empty_char; // dies
				}
			}
		}

		// copy the working vector back into the original vector for the next iteration
		original = working;

		// update generator pattern for each generation
		generator_pattern << "C";
	}
}
void Generator::fill(std::vector<char> & original, const int & iterations, const char & empty_char, const char & fill_char)
{
	std::vector<char> working = original; // copy to start

	for (int pass = 0; pass < iterations; ++pass)
	{
		const char * original_ptr = original.data();
		char * working_ptr = working.data();

		// for each row (excluding a 1-thick border)
		for (int i = 1; i < C::WORLD_X_DIMENSION - 1; ++i)
		{
			// for each room in the room (excluding a 1-thick border)
			for (int j = 1; j < C::WORLD_Y_DIMENSION - 1; ++j)
			{
				// if the current cell/room is water, skip it
				if (working_ptr[hash(i, j)] == C::WATER_CHAR) continue;

				// for every land cell, count its forest neighours (sp?)
				unsigned live_neighbors = 0;
				// count the row above
				live_neighbors += (original_ptr[hash(i - 1, j - 1)] == fill_char);
				live_neighbors += (original_ptr[hash(i - 1, j)] == fill_char);
				live_neighbors += (original_ptr[hash(i - 1, j + 1)] == fill_char);
				// count the left and right neighbors
				live_neighbors += (original_ptr[hash(i, j - 1)] == fill_char);
				live_neighbors += (original_ptr[hash(i, j + 1)] == fill_char);
				// count the row below
				live_neighbors += (original_ptr[hash(i + 1, j - 1)] == fill_char);
				live_neighbors += (original_ptr[hash(i + 1, j)] == fill_char);
				live_neighbors += (original_ptr[hash(i + 1, j + 1)] == fill_char);

				// check if the flag has shifted more than three times
				if (live_neighbors > 3)
				{
					working_ptr[hash(i, j)] = fill_char; // birth or survival
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
void Generator::save_intermediate_map(const std::vector<char> & v, const unsigned line_width) const
{
	to_file(v, line_width, generated_terrain_dir + "/" + generator_pattern.str() + ".txt");
}

// save to custom location
void Generator::to_file(const std::vector<char> & v, const unsigned line_width, const std::string & path) const
{
	// allocate the needed buffer size, given:
	//     data_size = v.size();
	//     number_of_newlines = v.size() / line_width;
	std::string buffer(v.size() + (v.size() / line_width), ' ');
	
	// create a pointer to walk the output and input
	char * output_ptr = &(buffer.front());
	const char * input_ptr = v.data();

	// I can't believe I have to cache this to optimize debug mode.
	// In the loop, *this call* takes 14.37% of startup execution time.
	const unsigned v_size = v.size();

	for (unsigned i = 0; i < v_size; )
	{
		*output_ptr++ = *input_ptr++;

		if (++i % line_width == 0) *output_ptr++ = '\n';
	}

	// save the stringstream to the specified path
	std::ofstream myfile;
	myfile.open(path);
	myfile << buffer;
}

std::string Generator::get_generator_pattern() const
{
	return generator_pattern.str();
}
std::string Generator::get_generated_terrain_dir() const
{
	return generated_terrain_dir;
}

inline unsigned Generator::hash(const unsigned x, const unsigned y) const
{
	return (x * C::WORLD_Y_DIMENSION) + y;
}
