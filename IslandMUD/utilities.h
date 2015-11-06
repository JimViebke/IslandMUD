
/* Jim Viebke
Feb 14, 2015 */

#ifndef UTILITIES_H
#define UTILITIES_H

#include <vector>
#include <map>
#include <iostream>
#include <algorithm> // std::transform
#include <memory> // for shared pointers
#include <sstream>
#include <fstream> // file IO
#include <sys/stat.h> // check for file presence
#include <chrono> // time

#ifdef _WIN32
#include <direct.h>
#else
#include <stdlib.h>
#include <stdio.h>
#endif

#include "constants.h"

using namespace std;

namespace Utilities {} // "forward delcaring"...

namespace U = Utilities; // ...to do this here

namespace Utilities
{
	// debugging output
	template <typename T> inline void print(const vector<T> & v)
	{
		for (const T & element : v)
		{
			cout << element << " ";
		}
	}

	// string construction
	template <typename T> inline string to_string(const T & val)
	{
		stringstream output;
		output << val;
		return output.str();
	}
	inline char_type to_char_type(const unsigned & val)
	{
		// crashes for numbers not in the range of 0-9 inclusive
		return C::numbers[val];
	}

	// verify a set of coordinates against the dimensions of the world
	inline bool bounds_check(const int & x, const int & y)
	{
		// x, and y must be from 0 to dimension inclusive
		return (x >= 0 && y >= 0 &&
			x < C::WORLD_X_DIMENSION &&
			y < C::WORLD_Y_DIMENSION);
	}
	inline bool bounds_check(const int & x, const int & y, const int & z)
	{
		// ensure x, y, and z are from 0 to dimension inclusive
		return (x >= 0 && y >= 0 && z >= 0 &&
			x < C::WORLD_X_DIMENSION &&
			y < C::WORLD_Y_DIMENSION &&
			z < C::WORLD_Z_DIMENSION);
	}

	// box drawing
	char_type corner_char(const bool & north, const bool & east, const bool & south, const bool & west);

	// movement
	inline void assign_movement_deltas(const string & direction_ID, int & dx, int & dy)
	{
		// express movement through two dimensions in dx, dy
		// If direction_ID is invalid, output arguments will not be modified.

		if (direction_ID == C::NORTH) { --dx; }
		else if (direction_ID == C::SOUTH) { ++dx; }
		else if (direction_ID == C::WEST) { --dy; }
		else if (direction_ID == C::EAST) { ++dy; }
		else if (direction_ID == C::NORTH_WEST) { --dx; --dy; }
		else if (direction_ID == C::NORTH_EAST) { --dx; ++dy; }
		else if (direction_ID == C::SOUTH_EAST) { ++dx; ++dy; }
		else if (direction_ID == C::SOUTH_WEST) { ++dx; --dy; }

		// nothing to return, values passed by reference
	}
	inline void assign_movement_deltas(const string & direction_ID, int & dx, int & dy, int & dz)
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
	inline string get_movement_direction(const int & x, const int & y, const int & dest_x, const int & dest_y)
	{
		if (x == dest_x) // east or west
		{
			if (y + 1 == dest_y) { return C::EAST; }
			if (y - 1 == dest_y) { return C::WEST; }
		}
		else if (x + 1 == dest_x) // south
		{
			if (y + 1 == dest_y) { return C::SOUTH_EAST; }
			if (y == dest_y) { return C::SOUTH; }
			if (y - 1 == dest_y) { return C::SOUTH_WEST; }
		}
		else if (x - 1 == dest_x) // north
		{
			if (y + 1 == dest_y) { return C::NORTH_EAST; }
			if (y == dest_y) { return C::NORTH; }
			if (y - 1 == dest_y) { return C::NORTH_WEST; }
		}

		return ""; // empty string object
	}

	// vector utilities
	template <typename T> inline bool contains(const vector<T> & v, const T & find_element)
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
	template <typename T> inline void erase_element_from_vector(vector<T> & vec, const T & erase_element)
	{
		vec.erase(find(vec.begin(), vec.end(), erase_element));
	}

	// file utilities
	inline bool file_exists(const string & path)
	{
		struct stat buf;
		return (stat(path.c_str(), &buf) != -1);
	}
	inline void create_path_if_not_exists(const string & path)
	{
		// if a path does not exist, create it
#ifdef _WIN32
		_mkdir(path.c_str());
#else
		system(string("mkdir -p " + path).c_str());
#endif
	}
	inline void to_file(const string & path, const string & contents)
	{
		ofstream file;
		file.open(path);
		file << contents;
		file.close();
	}

	// text formatting
	void to_lower_case(string & word);
	string capitalize(string & word);

	// grammar
	string get_article_for(const string & noun);

	// math
	inline int difference(const int & a, const int & b);
	int euclidean_distance(const int & x1, const int & y1, const int & x2, const int & y2);
	int diagonal_distance(const int & x1, const int & y1, const int & x2, const int & y2);

	// pathfinding
	int diagonal_movement_cost(const int & x1, const int & y1, const int & x2, const int & y2);

	// random utils
	int random_int_from(const int & min, const int & max);
	unsigned random_int_from(const unsigned & min, const unsigned & max);
	template <typename T> inline T random_element_from(const vector<T> & v)
	{
		// this will crash if v is empty
		return v[U::random_int_from(0u, unsigned(v.size() - 1))];
	}

	// time
	inline long long current_time_in_ms()
	{
		// return chrono::system_clock::to_time_t(chrono::system_clock::now());
		return chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();
	}

	// polymorphic testing and conversion
	template <typename Derived_Type, typename Parent_Type> inline bool is(const Parent_Type * object)
	{
		return dynamic_cast<Derived_Type const *>(object) != 0;
	}
	template <typename Derived_Type, typename Parent_Type> inline bool is(const Parent_Type & object)
	{
		return dynamic_cast<Derived_Type const *>(object.get()) != 0;
	}
	template <typename Derived_Type, typename Parent_Type> inline bool is_not(const Parent_Type * object)
	{
		return !U::is<Derived_Type>(object);
	}
	template <typename Derived_Type, typename Parent_Type> inline bool is_not(const Parent_Type & object)
	{
		return !U::is<Derived_Type>(object);
	}
	template <typename Derived_Type, typename Parent_Type> inline shared_ptr<Derived_Type> convert_to(shared_ptr<Parent_Type> const & object)
	{
		return dynamic_pointer_cast<Derived_Type>(object);
	}

	// managed pointer utilities
	template<typename T, typename... Args> inline unique_ptr<T> make_unique(Args&&... args)
	{
		return unique_ptr<T>(new T(forward<Args>(args)...));
	}
}

#endif
