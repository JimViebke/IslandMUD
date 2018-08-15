
/* Jim Viebke
Oct 21, 2015 */

#include "utilities.h"

#include <array>

// string utilities
unsigned U::to_unsigned(const std::string & word)
{
	// "a1b2c3" will return 123. "abc" will return 0.

	unsigned count = 0;
	for (const char & digit : word) // for each character in the second command
	{
		if (digit >= '0' && digit <= '9') // if the character is a digit
		{
			count *= 10; // shift digits to the left
			count += digit - '0'; // add newest digit
		}
	}
	return count;
}
void U::to_lower_case(std::string & word)
{
	// convert a vector of strings passed by reference to a vector of lowercase strings
	if (word.length() > 0)
	{
		std::transform(word.begin(), word.end(), word.begin(), ::tolower);
	}
}
std::string U::capitalize(std::string & word)
{
	// immediately return the string if it is empty
	if (word.size() == 0) return word;

	// if the first letter is in the range a-z, convert the letter to capital by subtracting 32
	if (word[0] >= 'a' && word[0] <= 'z')
		word[0] -= 32;

	return word;
}
std::string U::capitalize(const std::string & word)
{
	// immediately return the string if it is empty
	if (word.size() == 0) return word;

	// copy 'word' to a string that can be modified
	std::string result = word;

	// if the first letter is in the range a-z, convert the letter to capital by subtracting 32
	if (result[0] >= 'a' && result[0] <= 'z')
		result[0] -= 32;

	return result;
}

C::surface U::to_surface(const std::string & surface)
{
	if (surface == C::NORTH) return C::surface::north;
	else if (surface == C::EAST) return C::surface::east;
	else if (surface == C::SOUTH) return C::surface::south;
	else if (surface == C::WEST) return C::surface::west;

	return C::surface::not_a_surface;
}
std::string U::surface_to_string(const C::surface surface)
{
	const static std::array<std::string, 7> surfaces = {
		C::NORTH, C::EAST, C::SOUTH, C::WEST,
		C::CEILING, C::FLOOR, "not a surface" };

	return surfaces[(size_t)surface];
}
C::direction U::to_direction(const std::string & direction)
{
	if (direction == C::NORTH) return C::direction::north;
	else if (direction == C::EAST) return C::direction::east;
	else if (direction == C::SOUTH) return C::direction::south;
	else if (direction == C::WEST) return C::direction::west;
	else if (direction == C::NORTH_EAST) return C::direction::north_east;
	else if (direction == C::SOUTH_EAST) return C::direction::south_east;
	else if (direction == C::SOUTH_WEST) return C::direction::south_west;
	else if (direction == C::NORTH_WEST) return C::direction::north_west;

	return C::direction::not_a_direction;
}
std::string U::direction_to_string(const C::direction direction)
{
	const static std::array<std::string, 9> directions = {
		C::NORTH, C::EAST, C::SOUTH, C::WEST,
		C::NORTH_EAST, C::SOUTH_EAST, C::SOUTH_WEST, C::NORTH_WEST,
		"not_a_direction" };

	return directions[(size_t)direction];
}

C::surface U::opposite_surface(const C::surface surface)
{
	/* flipping:
	
	enum class surface
	{
		north,
		east,
		south,
		west,

		ceiling,
		floor,

		not_a_surface,
	}; */

	const static std::array<C::surface, 9> opposite_surfaces = {
		C::surface::south, C::surface::west, C::surface::north, C::surface::east,
		C::surface::floor, C::surface::ceiling, C::surface::not_a_surface };

	return opposite_surfaces[(size_t)surface];
}
C::direction U::opposite_direction(const C::direction direction)
{
	/* flipping:

	enum class direction
	{
		north,
		east,
		south,
		west,

		north_east,
		south_east,
		south_west,
		north_west,

		not_a_direction
	}; */

	const static std::array<C::direction, 9> opposite_surfaces = {
		C::direction::south, C::direction::west, C::direction::north, C::direction::east,
		C::direction::south_west, C::direction::north_west, C::direction::north_east, C::direction::south_east,
		C::direction::not_a_direction };

	return opposite_surfaces[(size_t)direction];
}

// grammar
std::string U::get_article_for(const std::string & noun)
{
	// get an iterator to the <key, value> pair for <noun, article>
	const std::map<std::string, std::string>::const_iterator it = C::articles.find(noun);

	// return the article if the key exists, else return generic "a(n)".
	return ((it != C::articles.cend()) ? it->second : "a(n)");
}
std::string U::get_plural_for(const std::string & noun)
{
	// get an iterator to the <key, value> pair for <noun, article>
	const std::map<std::string, std::string>::const_iterator it = C::plurals.find(noun);

	// return the article if the key exists, else return the original noun
	return ((it != C::plurals.cend()) ? it->second : noun);
}

// math
int U::euclidean_distance(const int & x1, const int & y1, const int & x2, const int & y2)
{
	int x_diff = difference(x1, x2);
	int y_diff = difference(y1, y2);
	return static_cast<int>(sqrt( // use Pythagoras' theorem
		(x_diff * x_diff) +
		(y_diff * y_diff)
		));
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
