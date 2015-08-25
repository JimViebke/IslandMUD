/* Jim Viebke
Feb 14, 2015 */

#ifndef RESOURCES_H
#define RESOURCES_H

#include <vector>
#include <map>
#include <iostream>
#include <algorithm> // std::transform
#include <memory> // for shared pointers
#include <sstream>
#include <fstream> // file IO
#include <sys/stat.h> // check for file presence
#include <chrono> // time

#ifdef WINDOWS
#include <direct.h>
#else
#include <sys/stat.h>
#endif

#include <stdlib.h>
#include <stdio.h>

#include "constants.h"

using namespace std;

class Resources; // forward declaring

typedef Resources R;

class Resources
{
public:

	// debugging output
	template <typename T> static inline void print(const vector<T> & v)
	{
		for (T element : v)
		{
			cout << element << " ";
		}
	}

	// string construction
	template <typename T> static inline string to_string(const T & val)
	{
		stringstream output;
		output << val;
		return output.str();
	}
	static inline char to_char(const unsigned & val)
	{
		vector<char> numbers = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };
		return numbers[val];
	}

	// verify a set of coordinates against the dimensions of the world
	static inline bool bounds_check(const int & x, const int & y)
	{
		// x, and y must be from 0 to dimension inclusive
		return (x >= 0 && y >= 0 &&
			x < C::WORLD_X_DIMENSION &&
			y < C::WORLD_Y_DIMENSION);
	}
	static inline bool bounds_check(const int & x, const int & y, const int & z)
	{
		// ensure x, y, and z are from 0 to dimension inclusive
		return (x >= 0 && y >= 0 && z >= 0 &&
			x < C::WORLD_X_DIMENSION &&
			y < C::WORLD_Y_DIMENSION &&
			z < C::WORLD_Z_DIMENSION);
	}

	// box drawing
	static char corner_char(const bool & north, const bool & east, const bool & south, const bool & west)
	{
		int index = 0;
		if (north) { index += 1; }
		if (east) { index += 2; }
		if (south) { index += 4; }
		if (west) { index += 8; }

		return C::CORNERS[index];
	}

	// movement
	void static inline assign_movement_deltas(const string & direction_ID, int & dx, int & dy, int & dz)
	{
		// express movement through three dimensions in dx, dy, dz
		// If direction_ID is invalid, output arguments will not be modified.

		if (direction_ID == C::NORTH) { --dx; }
		else if (direction_ID == C::SOUTH) { ++dx; }
		else if (direction_ID == C::WEST) { --dy; }
		else if (direction_ID == C::EAST) { ++dy; }
		else if (direction_ID == C::NORTH_WEST) { --dx; --dy; }
		else if (direction_ID == C::NORTH_EAST) { --dx; ++dy; }
		else if (direction_ID == C::SOUTH_EAST) { ++dx; ++dy; }
		else if (direction_ID == C::SOUTH_WEST) { ++dx; --dy; }
		else if (direction_ID == C::UP) { ++dz; }
		else if (direction_ID == C::DOWN) { --dz; }

		// nothing to return, values passed by reference
	}

	// vector utilities
	template <typename T> static inline bool contains(const vector<T> & v, const T & find_element)
	{
		for (const T & element : v)
		{
			if (element == find_element)
			{
				return true;
			}
		}
		return false;
	}
	template <typename T> static inline void erase_element_from_vector(vector<T> & vec, T erase_element)
	{
		vec.erase(find(vec.begin(), vec.end(), erase_element));
	}

	// file utilities
	static inline bool file_exists(const string & path)
	{
		struct stat buf;
		return (stat(path.c_str(), &buf) != -1);
	}
	static inline void create_path_if_not_exists(const string & path)
	{
		// if a path does not exist, create it
		#ifdef WINDOWS
		_mkdir(path.c_str());
		#else
		// TODO: cleanup "Dirty hack attack!"
		std::stringstream command;
		command << "mkdir -p " << path;
		system(command.str().c_str());
		#endif
	}
	static inline void to_file(const string & path, const string & contents)
	{
		ofstream file;
		file.open(path);
		file << contents;
		file.close();
	}

	// text formatting
	static void to_lower_case(string & word)
	{
		// convert a vector of strings passed by reference to a vector of lowercase strings
		if (word.length() > 0)
		{
			std::transform(word.begin(), word.end(), word.begin(), ::tolower);
		}
	}

	// math
	static int euclidean_distance(const int & x1, const int & y1, const int & x2, const int & y2)
	{
		int x_diff = R::difference(x1, x2);
		int y_diff = R::difference(y1, y2);
		return static_cast<int>(sqrt( // use Pythagoras's theorem
			(x_diff * x_diff) +
			(y_diff * y_diff)
			));
	}
	static int diagonal_distance(const int & x1, const int & y1, const int & x2, const int & y2)
	{
		// a diagonal move = (sqrt(2) * straight move)
		int dx = abs(x1 - x2);
		int dy = abs(y1 - y2);
		return C::AI_MOVEMENT_COST * (dx + dy) + (C::AI_MOVEMENT_COST_DIAGONAL - 2 * C::AI_MOVEMENT_COST) * min(dx, dy);
	}
	static int difference(const int & a, const int & b)
	{
		return (a - b > 0) ? (a - b) : (b - a);
	}

	// random utils
	static int random_int_from(const int & min, const int & max)
	{
		return min + static_cast<int>((rand() / static_cast<double>(RAND_MAX + 1)) * ((max + 1) - min));
	}

	// time
	static inline long long current_time_in_ms()
	{
		// return chrono::system_clock::to_time_t(chrono::system_clock::now());
		return chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();
	}

	// polymorphic testing and conversion
	template <typename Derived_Type, typename Parent_Type> static inline bool is(const Parent_Type * object)
	{
		return dynamic_cast<Derived_Type const *>(object) != 0;
	}
	template <typename Derived_Type, typename Parent_Type> static inline bool is(const Parent_Type & object)
	{
		return dynamic_cast<Derived_Type const *>(object.get()) != 0;
	}
	template <typename Derived_Type, typename Parent_Type> static inline bool is_not(const Parent_Type * object)
	{
		return !R::is<Derived_Type>(object);
	}
	template <typename Derived_Type, typename Parent_Type> static inline bool is_not(const Parent_Type & object)
	{
		return !R::is<Derived_Type>(object);
	}
	template <typename Derived_Type, typename Parent_Type> static inline shared_ptr<Derived_Type> convert_to(shared_ptr<Parent_Type> const & object)
	{
		return dynamic_pointer_cast<Derived_Type>(object);
	}
};

#endif
