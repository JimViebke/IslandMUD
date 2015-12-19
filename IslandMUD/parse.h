/* Jim Viebke
Feb 14, 2015

user input parsing */

#ifndef PARSE_H
#define PARSE_H

#include <iterator>
#include <sstream>
#include <map>
#include <algorithm>

#include "utilities.h"
#include "constants.h"

class Parse
{
private:
	Parse() = delete; // deliberately not implemented
	
	static std::map<std::string, std::string> dict; // first string maps to second string
	
	static void initialize();

public:

	static std::vector<std::string> tokenize(const std::string & s);
};

#endif
