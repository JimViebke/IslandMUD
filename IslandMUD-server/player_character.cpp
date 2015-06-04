/* Jim Viebke
May 15 2015 */

#include "player_character.h"
#include "npc_enemy.h"
#include "npc_unaffiliated.h"

string PC::print() const
{
	stringstream output;

	// if the player is carrying any materials, list them above equipment
	if (material_inventory.size() > 0)
	{
		for (multimap<string, shared_ptr<Material>>::const_iterator item_it = material_inventory.cbegin(); // for each item
			item_it != material_inventory.cend(); ++item_it)
		{
			output << item_it->second->name << " (x" << item_it->second->amount << ") "; // add its name to the output: stick (x5)
		}
	}

	// if the player is carrying any equipment, list them after materials
	if (equipment_inventory.size() > 0) // if the player is carrying anything
	{
		for (multimap<string, shared_ptr<Equipment>>::const_iterator item_it = equipment_inventory.cbegin(); // for each item
			item_it != equipment_inventory.cend(); ++item_it)
		{
			output << item_it->second->name << " "; // add its name to the output
		}
	}

	return (output.str().size() > 0) ? // if there is anything to print
		"You have " + output.str() : // return the output
		"You aren't carrying anything."; // return generic "no items" message
}

// Build and return a top-down area map around a given coordinate
string PC::generate_area_map(const World & world, const map<string, shared_ptr<Character>> & actors) const
{
	bool
		// is a wall present in a given direction?
		n, e, s, w, // "north"

		// is there a door present in a given location?
		nd, ed, sd, wd, // "north door?"

		// is a wall rubble in a given direction?
		nr, er, sr, wr, // "north rubble?"

		// is a forest area in a given direction from a coordinate? (f_n == forest_north)
		f_n, f_ne, f_e, f_se, f_s, f_sw, f_w, f_nw;

	stringstream user_map, a, b, c; // three stringstreams feed into one master stringstream

	// create a 2D vector to represent whether or not a tree is at a location
	vector<vector<bool>> forest_grid;
	for (int i = 0; i < (C::VIEW_DISTANCE * 2) + 3; ++i) // (view+1+view) plus a padding of 1 on each side
	{
		vector<bool> row;
		for (int j = 0; j < (C::VIEW_DISTANCE * 2) + 3; ++j)
		{
			row.push_back(false);
		}
		forest_grid.push_back(row);
	}

	/*
	We're looking for presence/absence info on trees in a 11*11 radius, and fitting it into a 13x13 grid, with a ring around for bounds padding.

	Top row is an example of indexes, botttom row is the forest_grid indexes these have to line up with

	player ---------v
	45 46 47 48 49 (50) 51 52 53 54 55
	1   2  3  4  5   6   7  8  9 10 11

	The following formula is applied to the indexes we're iterating over:
	tree_grid index = current index - (player index - (view distance + 1)) */
	for (int cx = x - (int)C::VIEW_DISTANCE; cx <= x + (int)C::VIEW_DISTANCE; ++cx)
	{
		int i = cx - (x - (C::VIEW_DISTANCE + 1));
		for (int cy = y - (int)C::VIEW_DISTANCE; cy <= y + (int)C::VIEW_DISTANCE; ++cy)
		{
			if (R::bounds_check(cx, cy))
			{
				forest_grid[i][cy - (y - (C::VIEW_DISTANCE + 1))] = world.room_at(cx, cy, C::GROUND_INDEX)->contains_item(C::TREE_ID);
			}
		}
	}

	/* for each row in sight range
	for each room in sight range in the row
	set n, e, s, w to(room contains surface ? ); */
	for (int cx = x - (int)C::VIEW_DISTANCE; cx <= x + (int)C::VIEW_DISTANCE; ++cx)
	{
		for (int cy = y - (int)C::VIEW_DISTANCE; cy <= y + (int)C::VIEW_DISTANCE; ++cy)
		{
			// if the room is out of bounds
			if (!R::bounds_check(cx, cy, C::GROUND_INDEX))
			{
				// draw the "room"
				a << "***";
				b << "***";
				c << "***";

				// nothing left to do with this room, skip to next room
				continue;
			}

			// if the coordinates are valid but the room is not loaded (this would be an error)
			if (world.room_at(cx, cy, C::GROUND_INDEX) == nullptr)
			{
				// draw the room as an exclaimation mark
				a << "   ";
				b << " ! ";
				c << "   ";

				// skip to next room
				continue;
			}

			// if there is a forest char at the given location
			// Somehow a tree is currently chosen to represent a forest room.

			// 23 24 25 26 27 (28) 29 30 31 32 33

			//  1  2  3  4  5   6   7  8  9 10 11

			// access_x = current index - (player index - (view distance + 1)) */
			//      1   =     23        - (      28     - (    5        + 1 ))

			// FGA = forest grid access
			int fga_x = cx - (x - (C::VIEW_DISTANCE + 1));
			int fga_y = cy - (y - (C::VIEW_DISTANCE + 1));
			if (forest_grid[fga_x][fga_y]) // if there is a tree here, determine how the tile should be drawn
			{
				// is a forest area at the neighbouring coordinates? f_n == forest_north
				f_n = forest_grid[fga_x - 1][fga_y];
				f_ne = forest_grid[fga_x - 1][fga_y + 1];
				f_e = forest_grid[fga_x][fga_y + 1];
				f_se = forest_grid[fga_x + 1][fga_y + 1];
				f_s = forest_grid[fga_x + 1][fga_y];
				f_sw = forest_grid[fga_x + 1][fga_y - 1];
				f_w = forest_grid[fga_x][fga_y - 1];
				f_nw = forest_grid[fga_x - 1][fga_y - 1];

				// conditionally draw a tree or an empty space in the corners, other five are always draw as trees
				a << ((f_n || f_nw || f_w) ? C::FOREST_CHAR : C::LAND_CHAR) << C::FOREST_CHAR << ((f_n || f_ne || f_e) ? C::FOREST_CHAR : C::LAND_CHAR);
				b << C::FOREST_CHAR << ((cx == x && cy == y) ? C::PLAYER_CHAR : C::FOREST_CHAR) << C::FOREST_CHAR;
				c << ((f_s || f_sw || f_w) ? C::FOREST_CHAR : C::LAND_CHAR) << C::FOREST_CHAR << ((f_s || f_se || f_e) ? C::FOREST_CHAR : C::LAND_CHAR);
			}
			// if the room is water
			else if (world.room_at(cx, cy, C::GROUND_INDEX)->is_water())
			{
				// Either draw a 3x3 grid with a "wave", or a 3x3 grid with the player's icon.

				a << "   ";
				b << " " << ((cx == x && cy == y) ? C::PLAYER_CHAR : C::WATER_CHAR) << " ";
				c << "   ";
			}
			// there is no tree, so there may be a structure
			else
			{
				// use a boolean value to indicate the presence or absence of a wall
				n = world.room_at(cx, cy, C::GROUND_INDEX)->has_surface(C::NORTH);
				e = world.room_at(cx, cy, C::GROUND_INDEX)->has_surface(C::EAST);
				s = world.room_at(cx, cy, C::GROUND_INDEX)->has_surface(C::SOUTH);
				w = world.room_at(cx, cy, C::GROUND_INDEX)->has_surface(C::WEST);

				nd = ed = sd = wd = nr = er = sr = wr = false; // set all door and rubble flags to false

				/*
				if a north wall is present
				-- north_has_door = room_at(x, y, z)...(surface_ID)...has_door();
				-- north_is_rubble = (the wall is rubble OR
				-- -- (a door exists AND the door is rubble)
				*/
				if (n)
				{
					nd = world.room_at(cx, cy, C::GROUND_INDEX)->get_room_sides().find(C::NORTH)->second.has_door();
					nr = (!world.room_at(cx, cy, C::GROUND_INDEX)->is_standing_wall(C::NORTH) ||
						(nd && world.room_at(cx, cy, C::GROUND_INDEX)->get_room_sides().find(C::NORTH)->second.get_door()->is_rubble()));
				}
				if (e)
				{
					ed = world.room_at(cx, cy, C::GROUND_INDEX)->get_room_sides().find(C::EAST)->second.has_door();
					er = (!world.room_at(cx, cy, C::GROUND_INDEX)->is_standing_wall(C::EAST) ||
						(ed && world.room_at(cx, cy, C::GROUND_INDEX)->get_room_sides().find(C::EAST)->second.get_door()->is_rubble()));
				}
				if (s)
				{
					sd = world.room_at(cx, cy, C::GROUND_INDEX)->get_room_sides().find(C::SOUTH)->second.has_door();
					sr = (!world.room_at(cx, cy, C::GROUND_INDEX)->is_standing_wall(C::SOUTH) ||
						(sd && world.room_at(cx, cy, C::GROUND_INDEX)->get_room_sides().find(C::SOUTH)->second.get_door()->is_rubble()));
				}
				if (w)
				{
					wd = world.room_at(cx, cy, C::GROUND_INDEX)->get_room_sides().find(C::WEST)->second.has_door();
					wr = (!world.room_at(cx, cy, C::GROUND_INDEX)->is_standing_wall(C::WEST) ||
						(wd && world.room_at(cx, cy, C::GROUND_INDEX)->get_room_sides().find(C::WEST)->second.get_door()->is_rubble()));
				}

				// count the enemies standing at a coordinate
				unsigned enemy_count = 0, neutral_count = 0;
				// for each actor in the room
				for (const string & actor_ID : world.room_at(cx, cy, z)->get_actor_ids())
				{
					// if the actor is a hostile NPC
					if (R::is<Hostile_NPC>(actors.find(actor_ID)->second))
					{
						++enemy_count; // count one more enemy in the room
					}
					else if (R::is<Neutral_NPC>(actors.find(actor_ID)->second))
					{
						++neutral_count;
					}
				}
				// reduce enemy count to a single-digit number
				enemy_count = ((enemy_count > 9) ? 9 : enemy_count);

				// debugging only
				if (enemy_count > 0) { cout << "\nAt " << cx << "," << cy << " there are " << enemy_count << " enemies."; }
				if (neutral_count > 0) { cout << "\nAt " << cx << "," << cy << " there are " << neutral_count << " neutrals."; }

				// time for glorious nested ternary statements to do this cheap
				a
					<< ((n && w) ? C::NW_CORNER : (n) ? C::WE_WALL : (w) ? C::NS_WALL : C::LAND_CHAR)
					<< ((nr) ? C::RUBBLE_CHAR : ((nd) ? C::WE_DOOR : ((n) ? C::WE_WALL : C::LAND_CHAR)))
					<< ((n && e) ? C::NE_CORNER : (n) ? C::WE_WALL : (e) ? C::NS_WALL : C::LAND_CHAR);
				b
					<< ((wr) ? C::RUBBLE_CHAR : ((wd) ? C::NS_DOOR : ((w) ? C::NS_WALL : C::LAND_CHAR)))
					// if the current coordinates are the player's, draw an @ icon, else if there is an enemy, draw enemy count, else if there is an item, draw an item char, else empty
					<< ((cx == x && cy == y) ? C::PLAYER_CHAR : ((enemy_count > 0) ? R::to_char(enemy_count) : ((neutral_count > 0) ? C::NPC_NEUTRAL_CHAR : ((world.room_at(cx, cy, C::GROUND_INDEX)->contains_no_items()) ? C::LAND_CHAR : C::ITEM_CHAR))))
					<< ((er) ? C::RUBBLE_CHAR : ((ed) ? C::NS_DOOR : ((e) ? C::NS_WALL : C::LAND_CHAR)));
				c
					<< ((s && w) ? C::SW_CORNER : (s) ? C::WE_WALL : (w) ? C::NS_WALL : C::LAND_CHAR)
					<< ((sr) ? C::RUBBLE_CHAR : ((sd) ? C::WE_DOOR : ((s) ? C::WE_WALL : C::LAND_CHAR)))
					<< ((s && e) ? C::SE_CORNER : (s) ? C::WE_WALL : (e) ? C::NS_WALL : C::LAND_CHAR);
			}
		} // end for each room in row

		// each iteration, push the three stringstreams representing the row into the user's map
		user_map << a.str() << endl << b.str() << endl << c.str() << endl;

		// reset the stringstreams for the next row of rooms
		a.str(std::string()); b.str(std::string()); c.str(std::string());
	} // end for each row

	return user_map.str(); // this now contains the client's entire visible area. No formatting is required, just dump this to console.
}
