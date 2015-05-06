/* Jim Viebke
Feb 16 2015 */

#include "parse.h"

map<string, string> Parse::dict; // first string maps to second string

void Parse::initialize()
{
	if (dict.size() == 0)
	{
		cout << "\nLoading parse dictionary...";

		// test commands
		P::dict["recipe"] = P::dict["recipes"] = C::PRINT_RECIPES_COMMAND;

		// verbs
		P::dict["help"] = P::dict["h"] = P::dict["q"] = P::dict["?"] = P::dict["assist"] = C::GET_HELP_COMMAND;
		P::dict["move"] = P::dict["m"] = P::dict["walk"] = P::dict["run"] = P::dict["head"] = P::dict["continue"] = C::MOVE_COMMAND;
		P::dict["take"] = P::dict["get"] = P::dict["pick"] = P::dict["choose"] = P::dict["grab"] = P::dict["acquire"] = C::TAKE_COMMAND;
		P::dict["craft"] = P::dict["forge"] = P::dict["make"] = P::dict["create"] = P::dict["fashion"] = C::CRAFT_COMMAND;
		P::dict["drop"] = P::dict["release"] = P::dict["unhand"] = C::DROP_COMMAND;
		P::dict["wait"] = P::dict["pause"] = P::dict["consider"] = P::dict["delay"] = C::WAIT_COMMAND;
		P::dict["construct"] = P::dict["build"] = P::dict["erect"] = C::CONSTRUCT_COMMAND;

		// items (materials)
		P::dict["stone"] = P::dict["rock"] = C::STONE_ID;
		P::dict["tree"] = C::TREE_ID;
		P::dict["branch"] = P::dict["branches"] = C::BRANCH_ID;
		P::dict["stick"] = C::STICK_ID;
		P::dict["vine"] = C::VINE_ID;

		// items (tools)
		P::dict["bow"] = C::BOW_ID;
		P::dict["staff"] = C::STAFF_ID;
		P::dict["axe"] = C::AXE_ID;

		// directions
		P::dict["n"] = P::dict["north"] = C::NORTH;
		P::dict["ne"] = P::dict["northeast"] = C::NORTH_EAST;
		P::dict["e"] = P::dict["east"] = C::EAST;
		P::dict["se"] = P::dict["southeast"] = C::SOUTH_EAST;
		P::dict["s"] = P::dict["south"] = C::SOUTH;
		P::dict["sw"] = P::dict["southwest"] = C::SOUTH_WEST;
		P::dict["w"] = P::dict["west"] = C::WEST;
		P::dict["nw"] = P::dict["northwest"] = C::NORTH_WEST;
		P::dict["up"] = P::dict["upward"] = P::dict["upwards"] = C::UP;
		P::dict["down"] = P::dict["downward"] = P::dict["downwards"] = C::UP;

		P::dict["floor"] = P::dict["ground"] = C::FLOOR;
		P::dict["ceiling"] = C::CEILING;
	}
}

vector<string> Parse::tokenize(const string & s)
{
	// converts an input string to vector of "engine official" keywords/commands

	cout << "\nDEBUG inside parse.tokenize: "; // debugging

	// convert the user input to a vector of strings (one word per element)
	stringstream ss(s);
	istream_iterator<string> begin(ss);
	istream_iterator<string> end;
	vector<string> strings(begin, end);

	R::print(strings); // debugging

	R::to_lower_case(strings); // convert all input to lower case

	for (string & word : strings) // for each word
	{
		// replace each word with the engine keyword, or C::BAD_COMMAND
		word = (dict.find(word) != dict.end()) ? dict.find(word)->second : C::BAD_COMMAND;
	}

	return strings;
}