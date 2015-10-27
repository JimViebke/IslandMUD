
/* Jim Viebke
Oct 21, 2015*/

#include "utilities.h"

char_type U::corner_char(const bool & north, const bool & east, const bool & south, const bool & west)
{
	int index = 0;
	if (north) { index += 1; }
	if (east) { index += 2; }
	if (south) { index += 4; }
	if (west) { index += 8; }

	return C::CORNERS[index];
}

// text formatting
void U::to_lower_case(string & word)
{
	// convert a vector of strings passed by reference to a vector of lowercase strings
	if (word.length() > 0)
	{
		std::transform(word.begin(), word.end(), word.begin(), ::tolower);
	}
}

string U::get_article_for(const string & noun)
{
	// get an iterator to the <key, value> pair for <noun, article>
	const map<string, string>::const_iterator it = C::articles.find(noun);

	// return the article if the key exists, else return generic "a(n)".
	return ((it != C::articles.cend()) ? it->second : "a(n)");
}

// math
int U::difference(const int & a, const int & b)
{
	return (a - b > 0) ? (a - b) : (b - a);
}
int U::euclidean_distance(const int & x1, const int & y1, const int & x2, const int & y2)
{
	int x_diff = difference(x1, x2);
	int y_diff = difference(y1, y2);
	return static_cast<int>(sqrt( // use Pythagoras' theorem
		(x_diff * x_diff) +
		(y_diff * y_diff)
		));
}
int U::diagonal_distance(const int & x1, const int & y1, const int & x2, const int & y2)
{
	return max(difference(x1, x2), difference(x2, y2));
}

// pathfinding
int U::diagonal_movement_cost(const int & x1, const int & y1, const int & x2, const int & y2)
{
	// Because this uses different movement costs, this works for AI pathfinding, but
	// not so much for determining if a coordinate is visible from another coordinate.

	// a diagonal move = (sqrt(2) * straight move)
	int dx = abs(x1 - x2);
	int dy = abs(y1 - y2);
	return C::AI_MOVEMENT_COST * (dx + dy) + (C::AI_MOVEMENT_COST_DIAGONAL - 2 * C::AI_MOVEMENT_COST) * min(dx, dy);
}

// random utils
int U::random_int_from(const int & min, const int & max)
{
	return min + (rand() % (max - min + 1));
}
unsigned U::random_int_from(const unsigned & min, const unsigned & max)
{
	return min + (rand() % (max - min + 1));
}
