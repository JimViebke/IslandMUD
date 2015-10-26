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

class Parse; // forward declaring

using P = Parse;

class Parse
{
private:
	Parse() = delete; // deliberately not implemented
	
public:
	static map<string, string> dict; // first string maps to second string
	static map<string, string> articles; // item_ID maps to its article
	
	static void initialize();

	static vector<string> tokenize(const string & s);
};

#endif
