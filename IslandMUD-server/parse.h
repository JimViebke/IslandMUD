/* Jim Viebke
Feb 14, 2015

user input parsing */

#ifndef PARSE_H
#define PARSE_H

#include <iterator>
#include <sstream>
#include <map>
#include <algorithm>

#include "resources.h"
#include "constants.h"

class Parse; // forward declaring

typedef Parse P;

class Parse
{
private:
	Parse() = delete; // deliberately not implemented
	
public:
	static map<string, string> dict; // first string maps to second string
	
	static void initialize();

	static vector<string> tokenize(const string & s);
};

#endif
