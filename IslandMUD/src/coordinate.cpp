
#include "coordinate.h"

Coordinate::Coordinate(const int x, const int y) : x(x), y(y) {}

Coordinate Coordinate::get_after_move(const C::direction & direction) const
{
	// copy the current coordinate to modify
	Coordinate destination = *this;

	// Update a coordinate based on a direction.
	// If the direction is invalid, nothing changes.

	switch (direction)
	{
	case C::direction::north: --destination.x; break;
	case C::direction::south: ++destination.x; break;
	case C::direction::west: --destination.y; break;
	case C::direction::east: ++destination.y; break;
	case C::direction::north_west: --destination.x; --destination.y; break;
	case C::direction::north_east: --destination.x; ++destination.y; break;
	case C::direction::south_west: ++destination.x; --destination.y; break;
	case C::direction::south_east: ++destination.x; ++destination.y; break;
	}

	return destination;
}

std::string Coordinate::to_string() const
{
	// build the result
	std::stringstream ss;
	ss << x << ", " << y;

	return ss.str();
}

C::direction Coordinate::get_movement_direction_to(const Coordinate & adjacent) const
{
	if (x == adjacent.x) // east or west
	{
		if (y + 1 == adjacent.y) return C::direction::east;
		if (y - 1 == adjacent.y) return C::direction::west;
	}
	else if (x + 1 == adjacent.x) // south
	{
		if (y + 1 == adjacent.y) return C::direction::south_east;
		if (y == adjacent.y) return C::direction::south;
		if (y - 1 == adjacent.y) return C::direction::south_west;
	}
	else if (x - 1 == adjacent.x) // north
	{
		if (y + 1 == adjacent.y) return C::direction::north_east;
		if (y == adjacent.y) return C::direction::north;
		if (y - 1 == adjacent.y) return C::direction::north_west;
	}

	// This is a bad thing to return. The coordinates are simple not adjacent.
	return C::direction::not_a_direction;
}
