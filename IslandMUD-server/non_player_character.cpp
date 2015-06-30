/* Jim Viebke
Jun 3 2015 */

#include "non_player_character.h"

// objective debugging
string NPC::get_objectives() const
{
	stringstream result;
	result << name << "'s objectives:\n";
	for (unsigned i = 0; i < objectives.size(); ++i)
	{
		// verb, direction, material, noun;
		// objective_x, objective_y, objective_z;

		result << "[" << objectives[i].verb
			<< "][" << objectives[i].direction
			<< "][" << objectives[i].material
			<< "][" << objectives[i].noun
			<< "][" << objectives[i].purpose
			<< "][" << ((objectives[i].already_planning_to_craft) ? string("true") : string("false"))
			<< "] ("
			<< objectives[i].objective_x << ","
			<< objectives[i].objective_y << ","
			<< objectives[i].objective_z << ")" << endl;
	}

	return result.str();
}

// objective creating and deletion
void Non_Player_Character::add_objective(const Objective_Priority & priority, const string & verb, const string & noun, const string & purpose)
{
	(priority == high_priority) ?
		objectives.push_front(Objective(verb, noun, purpose)) :
		objectives.push_back(Objective(verb, noun, purpose));
}
void Non_Player_Character::add_objective(const Objective_Priority & priority, const string & verb, const string & noun, const int & objective_x, const int & objective_y, const int & objective_z)
{
	(priority == high_priority) ?
		objectives.push_front(Objective(verb, noun, x, y, z)) :
		objectives.push_back(Objective(verb, noun, x, y, z));
}
void Non_Player_Character::erase_objective(const deque<Objective>::iterator & objective_iterator)
{
	objectives.erase(objective_iterator);
}
void Non_Player_Character::erase_objectives_matching_purpose(const string purpose)
{
	// arguement must be passed by value! Reference will change as the underlying structure is modified

	for (unsigned i = 0; i < objectives.size();)
	{
		if (objectives[i].purpose == purpose)
		{
			objectives.erase(objectives.begin() + i);
		}
		else
		{
			++i;
		}
	}
}

// objective information
string Non_Player_Character::the_item_im_looking_for() const
{
	return objectives.front().noun;
}
bool Non_Player_Character::one_can_craft(const string & item_id) const
{
	return recipes.has_recipe_for(item_id);
}
bool Non_Player_Character::i_have(const string & item_id) const
{
	return this->has(item_id);
}
bool Non_Player_Character::i_dont_have(const string & item_id) const
{
	return !this->has(item_id);
}
bool Non_Player_Character::im_planning_to_acquire(const string & item_ID) const
{
	for (const Objective objective : objectives)
	{
		if (objective.verb == C::AI_OBJECTIVE_ACQUIRE &&
			objective.noun == item_ID)
		{
			return true;
		}
	}
	return false;
}

// objective planning
void Non_Player_Character::plan_to_get(const string & item_id)
{
	add_objective(high_priority, C::AI_OBJECTIVE_ACQUIRE, item_id, item_id);
}
void Non_Player_Character::plan_to_craft(const string & item_id)
{
	// assumes item_id is craftable

	// The following loops deliberately do not take into account what the NPC already has.
	// If the NPC has 3 wood and needs 8, it will add 8 vine-finding objectives.
	// Rational is: don't count materials that are assumed to be present for another objective.

	for (const pair<string, int> & requirement : recipes.get_recipe(item_id).inventory_need)
	{
		for (int i = 0; i < requirement.second; ++i)
		{
			add_objective(high_priority, C::AI_OBJECTIVE_ACQUIRE, requirement.first, item_id);
		}
	}
	for (const pair<string, int> & requirement : recipes.get_recipe(item_id).inventory_remove)
	{
		for (int i = 0; i < requirement.second; ++i)
		{
			add_objective(high_priority, C::AI_OBJECTIVE_ACQUIRE, requirement.first, item_id);
		}
	}
}

// returns true if successful
bool NPC::pathfind(const int & x_dest, const int & y_dest, World & world)
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
		// -- Find lowest f - cost room on open list
		// -- Move it to closed list

		// find the cheapest room in the open list. Select it and move it to the closed list
		Node current_room = move_and_get_lowest_g_cost(open_list, closed_list);

		// for each room adjacent to current
		for (const string & direction : C::direction_ids)
		{
			if (direction == C::UP || direction == C::DOWN) { continue; } // only pathfinding in 2D now

			// calculate the location deltas
			int dx = 0, dy = 0, dz = 0;
			R::assign_movement_deltas(direction, dx, dy, dz);

			// skip if the room if it is out of bounds,
			if (!R::bounds_check(current_room.x + dx, current_room.y + dy, C::GROUND_INDEX)) { continue; }
			// or it is not loaded,
			if (world.room_at(current_room.x + dx, current_room.y + dy, C::GROUND_INDEX) == nullptr) { continue; }
			// or it is not within view distance,
			if (!world.room_at(current_room.x + dx, current_room.y + dy, C::GROUND_INDEX)->is_observed_by(this->name)) { continue; }
			// or we can not travel to it from the current room
			if (validate_movement(current_room.x, current_room.y, C::GROUND_INDEX, direction, dx, dy, 0, world) != C::GOOD_SIGNAL) { continue; }

			// create a node to select the next adjacent room
			Node adjacent_room(current_room.x + dx, current_room.y + dy, direction);

			// pass if the adjacent node is already on the closed list
			if (room_in_node_list(adjacent_room.x, adjacent_room.y, closed_list)) { continue; }

			// if the room is not on the open list
			if (!room_in_node_list(adjacent_room.x, adjacent_room.y, open_list))
			{
				// make the current room the parent of the adjacent room
				adjacent_room.parent_x = current_room.x;
				adjacent_room.parent_y = current_room.y;

				// calculate the movement cost
				int move_cost = (
					// check if the NPC will have to move through a forest
					(world.room_at(adjacent_room.x, adjacent_room.y, C::GROUND_INDEX)->contains_item(C::TREE_ID))
					?
					// determine if the movement is in a primary direction
					((R::contains(C::primary_direction_ids, direction) ? C::AI_MOVEMENT_COST_FOREST : C::AI_MOVEMENT_COST_FOREST_DIAGONAL))
					:
					((R::contains(C::primary_direction_ids, direction) ? C::AI_MOVEMENT_COST : C::AI_MOVEMENT_COST_DIAGONAL))
					);

				// use the g- and h-score to set the g-, h-, and f-score.
				adjacent_room.set_g_h_f(
					current_room.g + move_cost,
					R::diagonal_distance(x_dest, y_dest, adjacent_room.x, adjacent_room.y));
			}
			// else the room is on the open list
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

				// calculate the move cost
				int move_cost = (
					// check if the NPC will have to move through a forest
					(world.room_at(adjacent_room.x, adjacent_room.y, C::GROUND_INDEX)->contains_item(C::TREE_ID))
					?
					// determine if the movement is in a primary direction
					((R::contains(C::primary_direction_ids, direction) ? C::AI_MOVEMENT_COST_FOREST : C::AI_MOVEMENT_COST_FOREST_DIAGONAL))
					:
					((R::contains(C::primary_direction_ids, direction) ? C::AI_MOVEMENT_COST : C::AI_MOVEMENT_COST_DIAGONAL))
					);

				// if this way to the room is cheaper than the current best cost to the room
				if (current_room.g + move_cost < adjacent_room.g)
				{
					// update the adjacent room's parent room to the current room
					adjacent_room.parent_x = current_room.x;
					adjacent_room.parent_y = current_room.y;

					// use the g- and h-score to set the g-, h-, and f-score.
					adjacent_room.set_g_h_f(
						current_room.g + move_cost,
						R::diagonal_distance(x_dest, y_dest, adjacent_room.x, adjacent_room.y));
				}
			}

			// add the adjacent room to the open list
			open_list.push_back(adjacent_room);

		} // end for each adjacent room

		// keep searching as long as there are still rooms to search
	} while (open_list.size() > 0);



	// get the target (destination) room
	Node current_room = get_node_at(x_dest, y_dest, closed_list);

	// if the target room is not in the closed list, a path could not be found
	if (current_room.x == -1 || current_room.y == -1) { return false; }

	// Starting from the target room, continue finding the parent room until the current room is found
	// (this represents the path in reverse)
	do
	{
		// get the parent room of the current room
		Node parent_room = get_node_at(current_room.parent_x, current_room.parent_y, closed_list);

		// leave this here for debugging
		// cout << "\nI can get to " << current_room.x << "," << current_room.y << " from " << parent_room.x << "," << parent_room.y << ".";

		// if the parent of current_room is our location
		if (parent_room.x == this->x && parent_room.y == this->y)
		{
			// move to current_room
			move(current_room.direction_from_parent, world);

			/* leave this here for debugging
			cout << endl;
			for (const Node & node : closed_list)
			{
			cout << "Parent of " << node.x << "," << node.y << " is " << node.parent_x << "," << node.parent_y << ". Actual cost to node: " << node.g << ". Estimated cost to target: " << node.h << endl;
			} */

			return true; // we're done here
		}
		// if there is no parent room in the closed list (?!)
		else if (parent_room.x == -1 || parent_room.y == -1)
		{
			return false; // something went horribly wrong
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
NPC::Node NPC::move_and_get_lowest_g_cost(vector<Node> & open, vector<Node> & closed)
{
	// save the g cost of the first node
	int lowest_g_cost = open[0].g;

	// save the index of the first node
	unsigned lowest_g_cost_index = 0;

	// for each node
	for (unsigned i = 0; i < open.size(); ++i)
	{
		// if the node has a lower g cost than found so far
		if (open[i].g < lowest_g_cost)
		{
			// save the g cost
			lowest_g_cost = open[i].g;
			// save the index
			lowest_g_cost_index = i;
		}
	}

	// save the lowest g-cost node
	Node lowest_g_cost_node = open[lowest_g_cost_index];

	// add the lowest g-cost node to closed
	closed.push_back(lowest_g_cost_node);

	// remove the lowest g-cost node from open
	open.erase(open.begin() + lowest_g_cost_index);

	// return the lowest g-cost node
	return lowest_g_cost_node;
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
