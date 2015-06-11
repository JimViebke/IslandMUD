/* Jim Viebke
Jun 3 2015 */

#include "non_player_character.h"

void NPC::a_star_pathfind(const int & x_dest, const int & y_dest, World & world)
{
	// leave this for debugging
	// cout << "\nSearching for path from " << x << "," << y << " to " << x_dest << "," << y_dest << ".\n";

	/* 	Pathfinding resources:
	http://www.redblobgames.com/pathfinding/a-star/introduction.html
	http://theory.stanford.edu/~amitp/GameProgramming/Heuristics.html#S7

	F = G + H

	G : actual cost to reach a certain room
	H : estimated cost to reach destination from a certain room
	F-cost = G + H */



	vector<Node> open_list, closed_list;

	// Add current room to open list.
	open_list.push_back(Node(this->x, this->y, ""));
	// calculate current room's costs. G cost starts at 0.
	open_list[0].set_g_h_f(0, R::diagonal_distance(x_dest, y_dest, this->x, this->y));

	// Do
	do
	{
		/*/ / --Find lowest f - cost room on open list
		// -- Move it to closed list
		Node current_room = move_and_get_lowest_f_cost(open_list, closed_list); */

		// work through every open room
		Node current_room = open_list[0]; // copy
		closed_list.push_back(current_room); // move to closed
		open_list.erase(open_list.begin()); // erase original

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
				// -- -- -- Make current room the parent of adjacent
				adjacent_room.parent_x = current_room.x;
				adjacent_room.parent_y = current_room.y;
				// -- -- -- Record F, G, H costs of room
				adjacent_room.set_g_h_f(
					current_room.g + (R::contains(C::primary_direction_ids, direction) ? C::AI_MOVEMENT_COST : C::AI_MOVEMENT_COST_DIAGONAL), // check if the movement is non-diagonal or diagonal
					R::diagonal_distance(x_dest, y_dest, adjacent_room.x, adjacent_room.y));
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
				if (current_room.g + (R::contains(C::primary_direction_ids, direction) ? C::AI_MOVEMENT_COST : C::AI_MOVEMENT_COST_DIAGONAL) < adjacent_room.g)
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

			// -- -- -- Add room to open list
			open_list.push_back(adjacent_room);

		} // end for each adjacent room

		// keep searching if we have not reached the destination OR there are still rooms to search
	} while (/*!room_in_node_list(x_dest, y_dest, closed_list) ||*/ open_list.size() > 0);

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

		// leave this here for debugging
		// cout << "\nI can get to " << current_room.x << "," << current_room.y << " from " << parent_room.x << "," << parent_room.y << ".";

		// if the parent of current_room is our location, move to current_room
		if (parent_room.x == this->x && parent_room.y == this->y)
		{
			// move to current
			//this->move(C::opposite_direction_id.find(current_room.direction_from_parent)->second, world);
			move(current_room.direction_from_parent, world);

			/* leave this here for debugging
			cout << endl;
			for (const Node & node : closed_list)
			{
			cout << "Parent of " << node.x << "," << node.y << " is " << node.parent_x << "," << node.parent_y << endl <<
			"Actual cost to reach this node: " << node.g << ". Estimated cost to target: " << node.h << endl << endl;
			}*/

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

// Node member setter
void NPC::Node::set_g_h_f(const int & set_g, const int & set_h)
{
	g = set_g;
	h = set_h;
	f = g + h;
}

// pathfinding node utilities
NPC::Node NPC::move_and_get_lowest_f_cost(vector<Node> & open, vector<Node> & closed)
{
	// save the f cost of the first node
	int lowest_f_cost = open[0].f;

	// save the index of the first node
	unsigned lowest_f_cost_index = 0;

	// for each node
	for (unsigned i = 0; i < open.size(); ++i)
	{
		// if the node has a lower f cost than found so far
		if (open[i].f < lowest_f_cost)
		{
			// save the f cost
			lowest_f_cost = open[i].f;
			// save the index
			lowest_f_cost_index = i;
		}
	}

	// save the lowest f-cost node
	Node lowest_f_cost_node = open[lowest_f_cost_index];

	// add the lowest f-cost node to closed
	closed.push_back(lowest_f_cost_node);

	// remove the lowest f-cost node from open
	open.erase(open.begin() + lowest_f_cost_index);

	// return the lowest f-cost node
	return lowest_f_cost_node;
}
bool NPC::room_in_node_list(const int & find_x, const int & find_y, const vector<Node> & node_list) const
{
	// test if a given room node exists in a given node list

	// for each node
	for (const Node & node : node_list)
	{
		// if the node is the current room
		if (node.x == find_x && node.y == find_y)
		{
			// the node is already in the list
			return true;
		}
	}

	// the node is not in the list
	return false;
}
NPC::Node NPC::get_node_at(const int & find_x, const int & find_y, const vector<Node> & node_list) const
{
	// for each node
	for (const Node & node : node_list)
	{
		// if the node is the node we're searching for
		if (node.x == find_x && node.y == find_y)
		{
			// return it
			return node;
		}
	}

	// the node could not be found
	return Node();
}
