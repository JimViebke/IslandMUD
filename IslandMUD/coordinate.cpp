
#include "coordinate.h"

Coordinate::Coordinate(const int x, const int y) : x(x), y(y) {}

Coordinate Coordinate::get_after_move(const std::string & direction) const
{
	// copy the current coordinate to modify
	Coordinate destination = *this;

	// Update a coordinate based on a direction.
	// If the direction is invalid, nothing changes.

	if (direction == C::NORTH) { --destination.x; }
	else if (direction == C::SOUTH) { ++destination.x; }
	else if (direction == C::WEST) { --destination.y; }
	else if (direction == C::EAST) { ++destination.y; }
	else if (direction == C::NORTH_WEST) { --destination.x; --destination.y; }
	else if (direction == C::NORTH_EAST) { --destination.x; ++destination.y; }
	else if (direction == C::SOUTH_WEST) { ++destination.x; --destination.y; }
	else if (direction == C::SOUTH_EAST) { ++destination.x; ++destination.y; }

	return destination;
}

std::string Coordinate::to_string() const
{
	// build the result
	std::stringstream ss;
	ss << x << ", " << y;

	return ss.str();
}

std::string Coordinate::get_movement_direction_to(const Coordinate & adjacent) const
{
	const int dest_x = adjacent.get_x(), dest_y = adjacent.get_y();

	if (x == dest_x) // east or west
	{
		if (y + 1 == dest_y) return C::EAST;
		if (y - 1 == dest_y) return C::WEST;
	}
	else if (x + 1 == dest_x) // south
	{
		if (y + 1 == dest_y) return C::SOUTH_EAST;
		if (y == dest_y) return C::SOUTH;
		if (y - 1 == dest_y) return C::SOUTH_WEST;
	}
	else if (x - 1 == dest_x) // north
	{
		if (y + 1 == dest_y) return C::NORTH_EAST;
		if (y == dest_y) return C::NORTH;
		if (y - 1 == dest_y) return C::NORTH_WEST;
	}

	return ""; // empty string object because the passed coordinate was not adjacent
}
