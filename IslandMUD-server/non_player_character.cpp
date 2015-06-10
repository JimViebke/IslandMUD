/* Jim Viebke
Jun 3 2015 */

#include "non_player_character.h"

void NPC::a_star_pathfind(const int & x_dest, const int & y_dest, World & world)
{
	cout << "\nSearching for path from " << x << "," << y << " to " << x_dest << "," << y_dest << ".\n";

	vector<Node> open_list, closed_list;

	// Add current room to open list.
	open_list.push_back(Node(this->x, this->y, ""));

	// Do
	do
	{
		// -- Find lowest f-cost room on open list		
		// -- Move it to closed list
		Node current_room = move_and_get_lowest_f_cost(open_list, closed_list);

		// -- For each adjacent room to current
		for (const string & direction : C::direction_ids)
		{
			if (direction == C::UP || direction == C::DOWN) { continue; } // only pathfinding in 2D now

			// calculate the location deltas
			int dx = 0, dy = 0, dz = 0;
			R::assign_movement_deltas(direction, dx, dy, dz);

			// skip if the room is out of bounds
			if (!R::bounds_check(current_room.x + dx, current_room.y + dy, C::GROUND_INDEX)) { continue; }
			// skip the room if it is not loaded
			if (world.room_at(current_room.x + dx, current_room.y + dy, C::GROUND_INDEX) == nullptr) { continue; }
			// skip the room if it is not within view distance
			if (!world.room_at(current_room.x + dx, current_room.y + dy, C::GROUND_INDEX)->is_observed_by(this->name)) { continue; }

			// create a node to select the next adjacent room
			Node adjacent_room(current_room.x + dx, current_room.y + dy, direction);

			// -- -- pass if it is on the closed list
			if (room_in_node_list(adjacent_room.x, adjacent_room.y, closed_list)) { continue; }

			// -- -- pass if we can not travel to it from the current room
			if (validate_movement(current_room.x, current_room.y, C::GROUND_INDEX, direction, dx, dy, 0, world) != C::GOOD_SIGNAL) { continue; }

			// -- -- if room is not on open list
			if (!room_in_node_list(adjacent_room.x, adjacent_room.y, open_list))
			{
				// -- -- -- Make current room its parent
				adjacent_room.parent_x = current_room.x;
				adjacent_room.parent_y = current_room.y;
				// -- -- -- Record F, G, H costs of room
				adjacent_room.set_g_h_f(
					current_room.g + (R::contains(C::primary_direction_ids, direction) ? C::AI_MOVEMENT_COST : C::AI_MOVEMENT_COST_DIAGONAL), // check if the movement is non-diagonal or diagonal
					R::diagonal_distance(x_dest, y_dest, adjacent_room.x, adjacent_room.y));
				// -- -- -- Add room to open list
				open_list.push_back(adjacent_room);
			}
			// -- -- (else room is on open list)
			else
			{
				// pull adjacent_room out of open_list
				for (unsigned i = 0; i < open_list.size(); ++i) // for each node in the open list
				{
					if (open_list[i].x == adjacent_room.x && open_list[i].y == adjacent_room.y) // if the node is the current room
					{
						adjacent_room = get_node_at(adjacent_room.x, adjacent_room.y, open_list); // save it
						open_list.erase(open_list.begin() + i); // erase the original
						break; // adjacent_room is now the one from the list
					}
				}

				// -- -- if this g-cost to room is less than current g-cost to room
				if (adjacent_room.g > current_room.g + (R::contains(C::primary_direction_ids, direction) ? C::AI_MOVEMENT_COST : C::AI_MOVEMENT_COST_DIAGONAL))
				{
					// -- -- -- update current room to new parent of room
					adjacent_room.parent_x = current_room.x;
					adjacent_room.parent_y = current_room.y;
					// -- -- -- update costs
					adjacent_room.set_g_h_f(
						current_room.g + (R::contains(C::primary_direction_ids, direction) ? C::AI_MOVEMENT_COST : C::AI_MOVEMENT_COST_DIAGONAL), // check if the movement is non-diagonal or diagonal
						R::diagonal_distance(x_dest, y_dest, adjacent_room.x, adjacent_room.y));
				}
			}

		} // end for each adjacent room

		// keep searching if we have not reached the destination OR there are still rooms to search
	} while (!room_in_node_list(x_dest, y_dest, closed_list) || open_list.size() > 0);

	// Starting from target room, continue finding parent until the current room is found
	// (this represents the path in reverse)

	// get the target room
	Node current_room = get_node_at(x_dest, y_dest, closed_list);

	// if there is no target room in the closed list, a path could not be found
	if (current_room.x == -1 || current_room.y == -1) { return; }

	do
	{
		// get the parent room
		Node parent_room = get_node_at(current_room.parent_x, current_room.parent_y, closed_list);

		// if the parent of current_room is our location, move to current_room
		if (parent_room.x == this->x && parent_room.y == this->y)
		{
			// move to current
			this->move(C::opposite_direction_id.find(current_room.direction_from_parent)->second, world);

			// debugging
			cout << endl;
			for (const Node & node : closed_list)
			{
				cout << "Parent of " << node.x << "," << node.y << " is " << node.parent_x << "," << node.parent_y << endl <<
					"Actual cost to reach this node: " << node.g << ". Estimated cost to target: " << node.h << endl << endl;
			}

			return; // we're done here
		}
		// if there is no parent room in the closed list (?!)
		else if (parent_room.x == -1 || parent_room.y == -1)
		{
			return; // something went horribly wrong
		}

		// move up the path by one room
		current_room = parent_room;

	} while (true);
}
