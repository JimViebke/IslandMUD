#pragma once

#include "constants.h"
#include "utilities.h"

class Coordinate
{
private:
	int x, y;
public:
	Coordinate(const int x, const int y);
	
	Coordinate get_after_move(const std::string & direction) const;

	inline int get_x() const { return x; }
	inline int get_y() const { return y; }
	inline int get_hash() const
	{
		return (x * C::WORLD_Y_DIMENSION) + y;
	}
	std::string to_string() const;

	inline int diagonal_distance_to(const Coordinate & destination) const
	{
		// the larger of the differences between the x and y is the diagonal distance to the destination
		return std::max(U::difference(x, destination.x), U::difference(y, destination.y));
	}
	
	inline bool is_valid() const
	{
		// x, and y must be from 0 to dimension inclusive
		return x >= 0 && y >= 0 &&
			x < C::WORLD_X_DIMENSION &&
			y < C::WORLD_Y_DIMENSION;
	}

	// relative location testing
	inline bool is_north_of(const Coordinate & start) const { return x < start.x; }
	inline bool is_south_of(const Coordinate & start) const { return x > start.x; }
	inline bool is_west_of(const Coordinate & start) const { return y < start.y; }
	inline bool is_east_of(const Coordinate & start) const { return y > start.y; }
	inline bool is_northwest_of(const Coordinate & start) const { return x < start.x && y < start.y; }
	inline bool is_northeast_of(const Coordinate & start) const { return x < start.x && y > start.y; }
	inline bool is_southwest_of(const Coordinate & start) const { return x > start.x && y < start.y; }
	inline bool is_southeast_of(const Coordinate & start) const { return x > start.x && y > start.y; }
	
	std::string get_movement_direction_to(const Coordinate & adjacent) const;
	
	void reset() { x = y = -1; } // reset the coordinate, leaving it in an invalid state

	friend inline bool operator==(const Coordinate & lhs, const Coordinate & rhs);
	friend inline bool operator!=(const Coordinate & lhs, const Coordinate & rhs);
};

inline bool operator==(const Coordinate & lhs, const Coordinate & rhs)
{
	return lhs.x == rhs.x && lhs.y == rhs.y;
}
inline bool operator!=(const Coordinate & lhs, const Coordinate & rhs)
{
	return !(lhs == rhs);
}
