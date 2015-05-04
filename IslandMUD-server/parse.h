/* Jim Viebke
Feb 14, 2015

input utilities */

#ifndef PARSE_H
#define PARSE_H

#include <iterator>
#include <sstream>
#include <map>
#include <algorithm>

#include "resources.h"
#include "constants.h"

class Parse
{
public:
	static map<string, string> dict; // first string maps to second string

	Parse()	{ }

	static void initialize();

	static vector<string> tokenize(const string & s);

};

typedef Parse P;

#endif