
/* Jim Viebke
Feb 14, 2015 */

#ifndef UTILITIES_H
#define UTILITIES_H

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

namespace Utilities {} // forward declaring...

namespace U = Utilities; // ...to do this here

namespace Utilities
{
	// debugging output
	template <typename T> inline void print(const std::vector<T> & v)
	{
		std::stringstream ss;
		for (const T & element : v) ss << element << " ";
		std::cout << ss.str();
	}

	// string construction
	template <typename T> inline std::string to_string(const T & val)
	{
		std::stringstream output;
		output << val;
		return output.str();
	}
	inline char to_char(const unsigned & val)
	{
		// undefined behavior for numbers not in the range of 0-9 inclusive
		return val + '0';
	}

	// vector utilities
	template <typename T> inline bool contains(const std::vector<T> & v, const T & element)
	{
		return std::find(v.cbegin(), v.cend(), element) != v.cend();
	}
	template <typename T> inline void erase_element_from_vector(std::vector<T> & vec, const T & erase_element)
	{
		vec.erase(find(vec.begin(), vec.end(), erase_element));
	}
	template <typename T> inline void append_b_to_a(std::vector<T> & dest, const std::vector<T> & source)
	{
		dest.insert(dest.end(), source.begin(), source.end());
	}

	// file utilities
	inline bool file_exists(const std::string & path)
	{
		struct stat buf;
		return (stat(path.c_str(), &buf) != -1);
	}
	inline void create_path_if_not_exists(const std::string & path)
	{
		// if a path does not exist, create it
#ifdef _WIN32
		_mkdir(path.c_str());
#else
		system(std::string("mkdir -p " + path).c_str());
#endif
	}
	inline void to_file(const std::string & path, const std::string & contents)
	{
		std::ofstream file;
		file.open(path);
		file << contents;
		file.close();
	}

	// string utilities
	unsigned to_unsigned(const std::string & word);
	void to_lower_case(std::string & word);
	std::string capitalize(std::string & word);
	std::string capitalize(const std::string & word);

	// grammar
	std::string get_article_for(const std::string & noun);
	std::string get_plural_for(const std::string & noun);

	// math
	template<typename T> inline T difference(const T & a, const T & b) { return (a - b > 0) ? (a - b) : (b - a); }
	int euclidean_distance(const int & x1, const int & y1, const int & x2, const int & y2);

	// random utils
	int random_int_from(const int & min, const int & max);
	unsigned random_int_from(const unsigned & min, const unsigned & max);
	template <typename T> inline T random_element_from(const std::vector<T> & v)
	{
		// this will crash if v is empty
		return v[U::random_int_from(0u, unsigned(v.size() - 1))];
	}

	// time
	inline long long current_time_in_ms()
	{
		// return chrono::system_clock::to_time_t(chrono::system_clock::now());
		return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
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
	template <typename Derived_Type, typename Parent_Type> inline std::shared_ptr<Derived_Type> convert_to(std::shared_ptr<Parent_Type> const & object)
	{
		return std::dynamic_pointer_cast<Derived_Type>(object);
	}
}

#endif
