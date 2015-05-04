/*
Jim Viebke
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

#include <direct.h> // mkdir, only one of these should be needed
#include <stdlib.h>
#include <stdio.h>

#include "constants.h"

using namespace std;

class Resources
{
public:

	static const vector<string> surface_ids;
	static const vector<string> direction_ids;
	static const map<string, string> opposite_surface_id;

	// debugging output
	template <typename T> static inline void print(const vector<T> & v)
	{
		for (T element : v)
		{
			cout << element << " ";
		}
	}

	// string construction
	template <typename T> static inline string to_string(const vector<T> & v)
	{
		stringstream output;
		for (const T & element : v)
		{
			output << element << " ";
		}
		return output.str();
	}
	template <typename T> static inline string to_string(const T & val)
	{
		stringstream output;
		output << val;
		return output.str();
	}

	// movement
	void static inline assign_movement_deltas(const string & direction_ID, int & dx, int & dy, int & dz)
	{
		// express movement through three dimensions in dx, dy, dz
		// If direction_ID is invalid, output arguments will not be modified.

		if (direction_ID == C::NORTH) { --dx; }
		else if (direction_ID == C::NORTH_EAST) { --dx; ++dy; }
		else if (direction_ID == C::EAST) { ++dy; }
		else if (direction_ID == C::SOUTH_EAST) { ++dx; ++dy; }
		else if (direction_ID == C::SOUTH) { ++dx; }
		else if (direction_ID == C::SOUTH_WEST) { ++dx; --dy; }
		else if (direction_ID == C::WEST) { --dy; }
		else if (direction_ID == C::NORTH_WEST) { --dx; --dy; }
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
		_mkdir(path.c_str());
	}
	static inline void to_file(const string & path, const string & contents)
	{
		ofstream myfile;
		myfile.open(path);
		myfile << contents;
		myfile.close();
	}

	// text formatting
	static void to_lower_case(string & word)
	{
		// convert a vector of strings passed by reference to a vector of lowercase strings
		if (word.length() > 0)
		{
			transform(word.begin(), word.end(), word.begin(), tolower);
		}
	}
	static void to_lower_case(vector<string> & words)
	{
		// convert a vector of strings passed by referene to a vector of lowercase strings
		for (unsigned i = 0; i < words.size(); ++i)
		{
			to_lower_case(words[i]);
		}
	}

	// math
	static int distance_between(const int & x1, const int & y1, const int & x2, const int & y2)
	{
		return static_cast<int>(sqrt( // use Pythagoras's theorem
				(difference(x1, x2) * difference(x1, x2)) +
				(difference(y1, y2) * difference(y1, y2))
				));
	}
	static int difference(const int & a, const int & b)
	{
		return (a - b > 0) ? (a - b) : (b - a);
	}

	// time
	static inline __int64 current_time_in_ms()
	{
		return chrono::system_clock::to_time_t(chrono::system_clock::now());
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
		return !Resources::is<Derived_Type>(object);
	}
	template <typename Derived_Type, typename Parent_Type> static inline bool is_not(const Parent_Type & object)
	{
		return !Resources::is<Derived_Type>(object);
	}
	template <typename Derived_Type, typename Parent_Type> static inline typename shared_ptr<Derived_Type> convert_to(shared_ptr<Parent_Type> const & object)
	{
		return dynamic_pointer_cast<Derived_Type>(object);
	}
};

typedef Resources R;

#endif