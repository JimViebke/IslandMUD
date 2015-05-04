/* Jim Viebke
March 14 2015 */

#include "resources.h"

// store a vector of all valid surface ids
const vector<string> R::surface_ids = { C::NORTH, C::EAST, C::SOUTH, C::WEST, C::FLOOR, C::CEILING };

// store a vector of all valid movement directions
const vector<string> R::direction_ids = {
	C::NORTH, C::EAST, C::SOUTH, C::WEST,
	C::NORTH_EAST, C::NORTH_WEST,
	C::SOUTH_EAST, C::SOUTH_WEST,
	C::UP, C::DOWN };

// map surface ids to their opposite surface
const map<string, string> R::opposite_surface_id = {
	{ C::NORTH, C::SOUTH },
	{ C::SOUTH, C::NORTH },
	{ C::EAST, C::WEST },
	{ C::WEST, C::EAST },

	/* { C::NORTH_WEST, C::SOUTH_EAST }, // these aren't surface ids.
	{ C::SOUTH_EAST, C::NORTH_WEST },
	{ C::NORTH_EAST, C::SOUTH_WEST },
	{ C::SOUTH_WEST, C::NORTH_EAST }, */

	{ C::CEILING, C::FLOOR },
	{ C::FLOOR, C::CEILING },
};
