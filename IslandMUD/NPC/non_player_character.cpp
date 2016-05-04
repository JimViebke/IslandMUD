/* Jim Viebke
Jun 3 2015 */

#include "non_player_character.h"

// using NPC = Non_Player_Character; // ...in order to put this here

// this can only be instantiated by its children, hostile and neutral. No NPC of this type "NPC" exists or should be instantiated
NPC::Non_Player_Character(const std::string & name, const std::string & faction_ID) : Character(name, faction_ID) {}

// objective debugging
std::string NPC::get_objectives() const
{
	std::stringstream result;
	result << name << " (" << x << "," << y << ") objectives:\n";
	for (unsigned i = 0; i < objectives.size(); ++i)
	{
		// verb, direction, material, noun;
		// objective_x, objective_y, objective_z;

		result << "[" << objectives[i].verb
			<< "][" << objectives[i].direction
			<< "][" << objectives[i].material
			<< "][" << objectives[i].noun
			<< "][" << objectives[i].purpose
			<< "][" << ((objectives[i].already_planning_to_craft) ? std::string("true") : std::string("false"))
			<< "] ("
			<< objectives[i].objective_x << ","
			<< objectives[i].objective_y << ","
			<< objectives[i].objective_z << ")" << std::endl;
	}

	return result.str();
}

// NPC objective constructors
NPC::Objective::Objective(const std::string & verb, const std::string & noun, const std::string & purpose) :
	verb(verb), noun(noun), purpose(purpose) {}
NPC::Objective::Objective(const std::string & verb, const std::string & noun, const int & objective_x, const int & objective_y, const int & objective_z) :
	verb(verb), noun(noun), objective_x(objective_x), objective_y(objective_y), objective_z(objective_z) {}
NPC::Objective::Objective(const std::string & verb, const std::string & noun, const std::string & material, const std::string & direction, const int & objective_x, const int & objective_y, const int & objective_z, const bool & modifier) :
	verb(verb), noun(noun), material(material), direction(direction), objective_x(objective_x), objective_y(objective_y), objective_z(objective_z), modifier(modifier) {}

// NPC Coordinate constructor
NPC::Coordinate::Coordinate(const int & set_x, const int & set_y, const int & set_z) : _x(set_x), _y(set_y), _z(set_z) {}

// objective creating and deletion
void NPC::add_objective(const Objective_Priority & priority, const std::string & verb, const std::string & noun, const std::string & purpose)
{
	(priority == Objective_Priority::high_priority) ?
		objectives.push_front(Objective(verb, noun, purpose)) :
		objectives.push_back(Objective(verb, noun, purpose));
}
void NPC::add_objective(const Objective_Priority & priority, const std::string & verb, const std::string & noun, const int & objective_x, const int & objective_y, const int & objective_z)
{
	(priority == Objective_Priority::high_priority) ?
		objectives.push_front(Objective(verb, noun, x, y, z)) :
		objectives.push_back(Objective(verb, noun, x, y, z));
}
void NPC::erase_objective(const std::deque<Objective>::iterator & objective_iterator)
{
	objectives.erase(objective_iterator);
}
void NPC::erase_objectives_matching_purpose(const std::string purpose)
{
	// arguement must be passed by value! Reference will change as the underlying structure is modified

	// erase all objectives matching a purpose

	// for each objective
	for (unsigned i = 0; i < objectives.size();)
	{
		// if the objective matches the passed purpose
		if (objectives[i].purpose == purpose)
		{
			// erase the objective
			objectives.erase(objectives.begin() + i);
		}
		else // the objective does not match the passed purpose and will not be erased
		{
			++i; // move to to next element
		}
	}
}
void NPC::erase_goto_objective_matching(const std::string & purpose)
{
	// erase one goto objective matching purpose

	// for each objective
	for (std::deque<Objective>::iterator objective_iterator = objectives.begin();
	objective_iterator != objectives.end(); ++objective_iterator)
	{
		// if the objective is a go-to objective matching the passed purpose
		if (objective_iterator->verb == C::AI_OBJECTIVE_GOTO && objective_iterator->purpose == purpose)
		{
			// erase the objective and return
			objectives.erase(objective_iterator);
			return;
		}
	}
}
void NPC::erase_acquire_objective_matching(const std::string & noun)
{
	// erase one acquire objective matching noun

	// for each objective
	for (std::deque<Objective>::iterator objective_iterator = objectives.begin();
	objective_iterator != objectives.end(); ++objective_iterator)
	{
		// if the objective is an acquire objective matching the passed noun
		if (objective_iterator->verb == C::AI_OBJECTIVE_ACQUIRE && objective_iterator->noun == noun)
		{
			// erase the objective and return
			objectives.erase(objective_iterator);
			return;
		}
	}
}

// objective information
bool NPC::one_can_craft(const std::string & item_id) const
{
	// if a recipe exists for an item, the item is craftable
	return recipes->has_recipe_for(item_id);
}
bool NPC::i_have(const std::string & item_id) const
{
	return this->contains(item_id);
}
bool NPC::i_dont_have(const std::string & item_id) const
{
	return !this->contains(item_id);
}
bool NPC::im_planning_to_acquire(const std::string & item_ID) const
{
	// return true if an "acquire" objective has a noun matching item_ID

	for (const Objective & objective : objectives)
	{
		if (objective.verb == C::AI_OBJECTIVE_ACQUIRE &&
			objective.noun == item_ID)
		{
			return true;
		}
	}
	return false;
}
bool NPC::crafting_requirements_met(const std::string & item_ID, const World & world) const
{
	// WARNING: this assumes item_ID is craftable

	// Check if an NPC has all of the inventory requirements to craft an item.
	// This must also check objectives to ensure the ingredients in the NPC's inventory
	// aren't for another purpose.

	Recipe recipe = recipes->get_recipe(item_ID);

	// check both types of inventory requirements
	for (std::map<std::string, int>::const_iterator inventory_need = recipe.inventory_need.cbegin();
	inventory_need != recipe.inventory_need.cend(); ++inventory_need)
	{
		if (!this->contains(inventory_need->first, inventory_need->second))
		{
			return false;
		}
	}
	for (std::map<std::string, int>::const_iterator inventory_remove = recipe.inventory_remove.cbegin();
	inventory_remove != recipe.inventory_remove.cend(); ++inventory_remove)
	{
		if (!this->contains(inventory_remove->first, inventory_remove->second))
		{
			return false;
		}
	}

	// check both types of local requirements
	for (std::map<std::string, int>::const_iterator local_need = recipe.local_need.cbegin();
	local_need != recipe.local_need.cend(); ++local_need)
	{
		if (!world.room_at(x, y, z)->contains(local_need->first, local_need->second))
		{
			return false;
		}
	}
	for (std::map<std::string, int>::const_iterator local_remove = recipe.local_remove.cbegin();
	local_remove != recipe.local_remove.cend(); ++local_remove)
	{
		if (!world.room_at(x, y, z)->contains(local_remove->first, local_remove->second))
		{
			return false;
		}
	}

	// for each objective
	for (std::deque<Objective>::const_iterator objective_iterator = objectives.cbegin();
	objective_iterator != objectives.cend(); ++objective_iterator)
	{
		// if I am still planning on acquiring an item for the purpose of crafting item_ID
		if (objective_iterator->verb == C::AI_OBJECTIVE_ACQUIRE && objective_iterator->purpose == item_ID)
		{
			return false;
		}
	}

	return true;
}

// objective planning
void NPC::plan_to_get(const std::string & item_id)
{
	add_objective(Objective_Priority::high_priority, C::AI_OBJECTIVE_ACQUIRE, item_id, item_id);
}
void NPC::plan_to_craft(const std::string & item_id)
{
	// assumes item_id is craftable

	// The following loops deliberately do not take into account what the NPC already has.
	// If the NPC has 3 wood and needs 8, it will add 8 wood-finding objectives.
	// Other materials are assumed to be present for other objective.

	// Add the local requirements/needs before the inventory requirements/needs because
	// the inventory requirements should always be completed first.

	// two GOTO objective types
	for (const std::pair<std::string, int> & requirement : recipes->get_recipe(item_id).local_need)
	{
		for (int i = 0; i < requirement.second; ++i)
		{
			add_objective(Objective_Priority::high_priority, C::AI_OBJECTIVE_GOTO, requirement.first, item_id);
		}
	}
	for (const std::pair<std::string, int> & requirement : recipes->get_recipe(item_id).local_remove)
	{
		for (int i = 0; i < requirement.second; ++i)
		{
			add_objective(Objective_Priority::high_priority, C::AI_OBJECTIVE_GOTO, requirement.first, item_id);
		}
	}

	// two ACQUIRE objective types
	for (const std::pair<std::string, int> & requirement : recipes->get_recipe(item_id).inventory_need)
	{
		for (int i = 0; i < requirement.second; ++i)
		{
			add_objective(Objective_Priority::high_priority, C::AI_OBJECTIVE_ACQUIRE, requirement.first, item_id);
		}
	}
	for (const std::pair<std::string, int> & requirement : recipes->get_recipe(item_id).inventory_remove)
	{
		for (int i = 0; i < requirement.second; ++i)
		{
			add_objective(Objective_Priority::high_priority, C::AI_OBJECTIVE_ACQUIRE, requirement.first, item_id);
		}
	}
}

// returns true if successful
bool NPC::pathfind(const int & x_dest, const int & y_dest, World & world, Update_Messages & update_messages)
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



	std::vector<Node> open_list, closed_list;

	// Add current room to open list.
	open_list.push_back(Node(this->x, this->y, ""));

	// calculate current room's costs. G cost starts at 0.
	open_list[0].set_g_h_f(0, U::diagonal_movement_cost(x_dest, y_dest, this->x, this->y));

	// Do
	do
	{
		// -- Find lowest f - cost room on open list
		// -- Move it to closed list

		// find the cheapest room in the open list. Select it and move it to the closed list
		const Node current_room = move_and_get_lowest_g_cost(open_list, closed_list);

		// for each room adjacent to current
		for (const std::string & direction : C::direction_ids)
		{
			if (direction == C::UP || direction == C::DOWN) { continue; } // only pathfinding in 2D now

			// calculate the location deltas
			int dx = 0, dy = 0, dz = 0;
			U::assign_movement_deltas(direction, dx, dy, dz);

			// skip if the room if it is out of bounds,
			if (!U::bounds_check(current_room.x + dx, current_room.y + dy, C::GROUND_INDEX)) { continue; }
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
					(world.room_at(adjacent_room.x, adjacent_room.y, C::GROUND_INDEX)->contains(C::TREE_ID))
					?
					// determine if the movement is in a primary direction
					((U::contains(C::primary_direction_ids, direction) ? C::AI_MOVEMENT_COST_FOREST : C::AI_MOVEMENT_COST_FOREST_DIAGONAL))
					:
					((U::contains(C::primary_direction_ids, direction) ? C::AI_MOVEMENT_COST : C::AI_MOVEMENT_COST_DIAGONAL))
					);

				// use the g- and h-score to set the g-, h-, and f-score.
				adjacent_room.set_g_h_f(
					current_room.g + move_cost,
					U::diagonal_movement_cost(x_dest, y_dest, adjacent_room.x, adjacent_room.y));
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
					(world.room_at(adjacent_room.x, adjacent_room.y, C::GROUND_INDEX)->contains(C::TREE_ID))
					?
					// determine if the movement is in a primary direction
					((U::contains(C::primary_direction_ids, direction) ? C::AI_MOVEMENT_COST_FOREST : C::AI_MOVEMENT_COST_FOREST_DIAGONAL))
					:
					((U::contains(C::primary_direction_ids, direction) ? C::AI_MOVEMENT_COST : C::AI_MOVEMENT_COST_DIAGONAL))
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
						U::diagonal_movement_cost(x_dest, y_dest, adjacent_room.x, adjacent_room.y));
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
			update_messages = move(current_room.direction_from_parent, world);

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
bool NPC::best_attempt_pathfind(const int & x_dest, const int & y_dest, World & world, Update_Messages & update_messages)
{
	// the NPC could not pathfind to the destination, try to move in the direction of the destination.

	// returns a boolean indicating if the NPC moved.

	if (x > x_dest && y > y_dest) // northwest
	{
		// if the target is out of view but inline with any part of the current visible area,
		// don't pathfind to corner; pathfind to the edge of the visible area that
		// is inline with the destination

		if (save_path_to(
			(((x - x_dest) <= C::VIEW_DISTANCE) ? (x_dest) : (x - C::VIEW_DISTANCE)),
			(((y - y_dest) <= C::VIEW_DISTANCE) ? (y_dest) : (y - C::VIEW_DISTANCE)), world))
		{
			// attempt to move to the destination, return if successful
			if (make_path_movement(world, update_messages)) { return true; }
		}
	}

	if (x > x_dest && y < y_dest) // northeast
	{
		if (save_path_to(
			(((x - x_dest) <= C::VIEW_DISTANCE) ? (x_dest) : (x - C::VIEW_DISTANCE)),
			(((y_dest - y) <= C::VIEW_DISTANCE) ? (y_dest) : (y + C::VIEW_DISTANCE)), world))
		{
			if (make_path_movement(world, update_messages)) { return true; }
		}
	}

	if (x < x_dest && y > y_dest) // southwest
	{
		if (save_path_to(
			(((x_dest - x) <= C::VIEW_DISTANCE) ? (x_dest) : (x + C::VIEW_DISTANCE)),
			(((y - y_dest) <= C::VIEW_DISTANCE) ? (y_dest) : (y - C::VIEW_DISTANCE)), world))
		{
			if (make_path_movement(world, update_messages)) { return true; }
		}
	}

	if (x < x_dest && y < y_dest) // southeast
	{
		if (save_path_to(
			(((x_dest - x) <= C::VIEW_DISTANCE) ? (x_dest) : (x + C::VIEW_DISTANCE)),
			(((y_dest - y) <= C::VIEW_DISTANCE) ? (y_dest) : (y + C::VIEW_DISTANCE)), world))
		{
			if (make_path_movement(world, update_messages)) { return true; }
		}
	}

	// execution reaches here if a diagonal movement failed or the target is directly n/e/s/w or
	// the target is visible but unreachable

	if (x > x_dest) // north
	{
		// starting at the edge of view and working toward the player
		for (int i = C::VIEW_DISTANCE; i > 0; --i)
		{
			// if a path can be found
			if (save_path_to(x - i, y, world))
			{
				// make the first move
				make_path_movement(world, update_messages);
				return true;
			}
		}
	}

	if (x < x_dest) // south
	{
		for (int i = C::VIEW_DISTANCE; i > 0; --i)
		{
			if (save_path_to(x + i, y, world))
			{
				make_path_movement(world, update_messages);
				return true;
			}
		}
	}

	if (y > y_dest) // west
	{
		for (int i = C::VIEW_DISTANCE; i > 0; --i)
		{
			if (save_path_to(x, y - i, world))
			{
				make_path_movement(world, update_messages);
				return true;
			}
		}
	}

	if (y < y_dest) // east
	{
		for (int i = C::VIEW_DISTANCE; i > 0; --i)
		{
			if (save_path_to(x, y + i, world))
			{
				make_path_movement(world, update_messages);
				return true;
			}
		}
	}

	return false;
}
bool NPC::pathfind_to_closest_item(const std::string & item_id, World & world, Update_Messages & update_messages)
{
	// leave this for debugging
	// cout << "\nSearching for path from " << x << "," << y << " to any " << item_id << ".\n";

	/* 	Pathfinding resources:
	http://www.redblobgames.com/pathfinding/a-star/introduction.html
	http://theory.stanford.edu/~amitp/GameProgramming/Heuristics.html#S7

	F = G + H

	G : actual cost to reach a certain room
	H : estimated cost to reach destination from a certain room
	F-cost = G + H */



	std::vector<Node> open_list, closed_list;

	// Add current room to open list.
	open_list.push_back(Node(this->x, this->y, ""));

	// cost to reach current room is of course 0
	open_list[0].set_g(0);

	// Do
	do
	{
		// -- Find lowest f - cost room on open list
		// -- Move it to closed list

		// find the cheapest room in the open list. Select it and move it to the closed list
		const Node current_room = move_and_get_lowest_g_cost(open_list, closed_list);

		// for each room adjacent to current
		for (const std::string & direction : C::direction_ids)
		{
			if (direction == C::UP || direction == C::DOWN) { continue; } // only pathfinding in 2D now

			// calculate the location deltas
			int dx = 0, dy = 0, dz = 0;
			U::assign_movement_deltas(direction, dx, dy, dz);

			// skip if the room if it is out of bounds,
			if (!U::bounds_check(current_room.x + dx, current_room.y + dy, C::GROUND_INDEX)) { continue; }
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
					(world.room_at(adjacent_room.x, adjacent_room.y, C::GROUND_INDEX)->contains(C::TREE_ID))
					?
					// determine if the movement is in a primary direction
					((U::contains(C::primary_direction_ids, direction) ? C::AI_MOVEMENT_COST_FOREST : C::AI_MOVEMENT_COST_FOREST_DIAGONAL))
					:
					((U::contains(C::primary_direction_ids, direction) ? C::AI_MOVEMENT_COST : C::AI_MOVEMENT_COST_DIAGONAL))
					);

				// calculate the cost to this room
				adjacent_room.set_g(current_room.g + move_cost);
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
					(world.room_at(adjacent_room.x, adjacent_room.y, C::GROUND_INDEX)->contains(C::TREE_ID))
					?
					// determine if the movement is in a primary direction
					((U::contains(C::primary_direction_ids, direction) ? C::AI_MOVEMENT_COST_FOREST : C::AI_MOVEMENT_COST_FOREST_DIAGONAL))
					:
					((U::contains(C::primary_direction_ids, direction) ? C::AI_MOVEMENT_COST : C::AI_MOVEMENT_COST_DIAGONAL))
					);

				// if this way to the room is cheaper than the current best cost to the room
				if (current_room.g + move_cost < adjacent_room.g)
				{
					// update the adjacent room's parent room to the current room
					adjacent_room.parent_x = current_room.x;
					adjacent_room.parent_y = current_room.y;

					// update the g-score cost to the room
					adjacent_room.set_g(current_room.g + move_cost);
				}
			}

			// add the adjacent room to the open list
			open_list.push_back(adjacent_room);

		} // end for each adjacent room

		// keep searching as long as there are still rooms to search
	} while (open_list.size() > 0);



	// get the target (destination) room
	Node destination_room;
	destination_room.g = 999999;
	for (const Node & node : closed_list)
	{
		if (world.room_at(node.x, node.y, C::GROUND_INDEX)->contains(item_id) &&
			node.g < destination_room.g)
		{
			destination_room = node;
		}
	}

	// if no destination was found, there is no path
	if (destination_room.x == -1 || destination_room.y == -1) { return false; }


	Node current_room = destination_room;

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
			update_messages = move(current_room.direction_from_parent, world);

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
bool NPC::save_path_to(const int & x_dest, const int & y_dest, World & world)
{
	// Returns a boolean indicating if a path was found. If a path was found,
	// the path is now saved.

	// leave this for debugging
	// cout << "\nSearching for path from " << x << "," << y << " to " << x_dest << "," << y_dest << ".\n";

	/* 	Pathfinding resources:
	http://www.redblobgames.com/pathfinding/a-star/introduction.html
	http://theory.stanford.edu/~amitp/GameProgramming/Heuristics.html#S7

	F = G + H

	G : actual cost to reach a certain room
	H : estimated cost to reach destination from a certain room
	F-cost = G + H */

	// start by removing any existing planned path
	path.clear();

	std::vector<Node> open_list, closed_list;

	// Add current room to open list.
	open_list.push_back(Node(this->x, this->y, ""));

	// calculate current room's costs. G cost starts at 0.
	open_list[0].set_g_h_f(0, U::diagonal_movement_cost(x_dest, y_dest, this->x, this->y));

	// Do
	do
	{
		// -- Find lowest f - cost room on open list
		// -- Move it to closed list

		// find the cheapest room in the open list. Select it and move it to the closed list
		const Node current_room = move_and_get_lowest_g_cost(open_list, closed_list);

		// for each room adjacent to current
		for (const std::string & direction : C::direction_ids)
		{
			if (direction == C::UP || direction == C::DOWN) { continue; } // only pathfinding in 2D now

			// calculate the location deltas
			int dx = 0, dy = 0, dz = 0;
			U::assign_movement_deltas(direction, dx, dy, dz);

			// skip if the room if it is out of bounds,
			if (!U::bounds_check(current_room.x + dx, current_room.y + dy, C::GROUND_INDEX)) { continue; }
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
					(world.room_at(adjacent_room.x, adjacent_room.y, C::GROUND_INDEX)->contains(C::TREE_ID))
					?
					// determine if the movement is in a primary direction
					((U::contains(C::primary_direction_ids, direction) ? C::AI_MOVEMENT_COST_FOREST : C::AI_MOVEMENT_COST_FOREST_DIAGONAL))
					:
					((U::contains(C::primary_direction_ids, direction) ? C::AI_MOVEMENT_COST : C::AI_MOVEMENT_COST_DIAGONAL))
					);

				// use the g- and h-score to set the g-, h-, and f-score.
				adjacent_room.set_g_h_f(
					current_room.g + move_cost,
					U::diagonal_movement_cost(x_dest, y_dest, adjacent_room.x, adjacent_room.y));
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
					(world.room_at(adjacent_room.x, adjacent_room.y, C::GROUND_INDEX)->contains(C::TREE_ID))
					?
					// determine if the movement is in a primary direction
					((U::contains(C::primary_direction_ids, direction) ? C::AI_MOVEMENT_COST_FOREST : C::AI_MOVEMENT_COST_FOREST_DIAGONAL))
					:
					((U::contains(C::primary_direction_ids, direction) ? C::AI_MOVEMENT_COST : C::AI_MOVEMENT_COST_DIAGONAL))
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
						U::diagonal_movement_cost(x_dest, y_dest, adjacent_room.x, adjacent_room.y));
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
	if (current_room.x == -1 || current_room.y == -1)
	{
		return false;
	}
	else
	{
		// the target room is in the closed list, add it to the coordinate path
		path.push_back(Coordinate(current_room.x, current_room.y, C::GROUND_INDEX));
	}

	// Starting from the target room, continue finding the parent room until the current room is found
	// (this represents the path in reverse)
	// Add each coordinate to the NPC's path
	do
	{
		// get the parent room of the current room
		Node parent_room = get_node_at(current_room.parent_x, current_room.parent_y, closed_list);

		// leave this here for debugging
		// cout << "\nI can get to " << current_room.x << "," << current_room.y << " from " << parent_room.x << "," << parent_room.y << ".";

		// if the parent of current_room is our location
		if (parent_room.x == this->x && parent_room.y == this->y)
		{
			return true; // we're done here
		}
		// if there is no parent room in the closed list (?!)
		else if (parent_room.x == -1 || parent_room.y == -1)
		{
			return false; // something went horribly wrong
		}

		// add the parent room's coordinates to the path
		path.push_front(Coordinate(parent_room.x, parent_room.y, C::GROUND_INDEX));

		// move up the path by one room
		current_room = parent_room;

	} while (true);
}
bool NPC::make_path_movement(World & world, Update_Messages & update_messages)
{
	// if the NPC is currently pathfinding to a destination
	if (!path.empty())
	{
		// copy current coordinates
		const int cx = x, cy = y, cz = z;

		// attempt to move
		update_messages = this->move(U::get_movement_direction(x, y, path.begin()->_x, path.begin()->_y), world);

		// check to see if the NPC's coordinates have changed
		if (x != cx || y != cy || z != cz)
		{
			// if successful, remove the coordinate that we travelled to
			path.erase(path.begin());
			return true; // finished
		}
	}

	// there was no path to follow or the next planned move in the path could not be made
	return false;
}

// other pathfinding utilities
bool NPC::coordinates_are_on_path(const int & find_x, const int & find_y) const
{
	for (const Coordinate & coordinate : path)
	{
		if (coordinate._x == find_x && coordinate._y == find_y) return true;
	}

	return false;
}

// Node constructors
NPC::Node::Node() {}
NPC::Node::Node(const int & set_x, const int & set_y, const std::string & dir) : x(set_x), y(set_y), direction_from_parent(dir) {}

// Node member setter
void NPC::Node::set_g_h_f(const int & set_g, const int & set_h)
{
	g = set_g;
	h = set_h;
	f = g + h;
}
void NPC::Node::set_g(const int & set_g)
{
	g = set_g;
}

// pathfinding node utilities
NPC::Node NPC::move_and_get_lowest_f_cost(std::vector<Node> & open, std::vector<Node> & closed)
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
NPC::Node NPC::move_and_get_lowest_g_cost(std::vector<Node> & open, std::vector<Node> & closed)
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
bool NPC::room_in_node_list(const int & find_x, const int & find_y, const std::vector<Node> & node_list) const
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
NPC::Node NPC::get_node_at(const int & find_x, const int & find_y, const std::vector<Node> & node_list) const
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

	// the node could not be found, return a default (invalid) node
	return Node();
}
