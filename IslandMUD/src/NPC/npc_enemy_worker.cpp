
/* Jim Viebke
Aug 15 2015 */

#include <queue>

#include "npc_enemy_worker.h"

Update_Messages Hostile_NPC_Worker::update()
{
	if (!fortress_planned)
	{
		plan_fortress();
		fortress_planned = true; // only set the flag after plans have been made

		for (int i = 0; i < 10000; ++i) // cheating here to start the NPC off with materials
		{
			this->insert(Craft::make(C::STONE_ID));
		}
	}

	// make sure the NPC has an axe to cut down trees
	if (i_dont_have(C::AXE_ID) && !im_planning_to_acquire(C::AXE_ID))
	{
		plan_to_get(C::AXE_ID);
	}

	// attempt to move to the destination, return if successful
	Update_Messages update_messages("");
	if (make_path_movement(update_messages)) { return update_messages; }

	// as an optimizations, only enter the next block if the AI doesn't have an axe
	if (i_dont_have(C::AXE_ID))
	{
		// in this block: take the item if it's here, move to the item if it is visible and reachable, otherwise plan to craft the item
		// and aquire needed resources
		for (std::deque<Objective>::iterator objective_iterator = objectives.begin();
			objective_iterator != objectives.end();)
		{
			// if the NPC is searching for an item
			if (objective_iterator->verb == C::AI_OBJECTIVE_ACQUIRE)
			{
				// if the item is here, take it, remove the current objective, and return
				if (world->room_at(location)->contains(objective_iterator->noun))
				{
					// remove the item from the room
					update_messages = take(objective_iterator->noun);

					if (objective_iterator->noun == objective_iterator->purpose)
					{
						// this item is an end goal
						erase_objectives_matching_purpose(objective_iterator->purpose); // erasing all objectives
					}
					else
					{
						// this item is a means to an end
						erase_objective(objective_iterator);
					}

					return update_messages;
				}

				// see if the item is reachable
				if (pathfind_to_closest_item(objective_iterator->noun, update_messages))
				{
					std::stringstream ss;
					ss << "Found a path to " << objective_iterator->noun << std::endl; // debugging
					std::cout << ss.str();
					return update_messages;
				}

				// a path could not be found to the item, plan to craft it if it is craftable and the NPC isn't planning to already

				// if i'm not already planning on crafting the item
				// AND the item is craftable
				if (!objective_iterator->already_planning_to_craft
					&& one_can_craft(objective_iterator->noun))
				{
					// plan to craft the item
					objective_iterator->already_planning_to_craft = true;
					plan_to_craft(objective_iterator->noun);
					objective_iterator = objectives.begin(); // obligatory reset
					continue; // jump to next iteration
				}
			}

			// if the NPC is planning on moving to an instance of an item
			if (objective_iterator->verb == C::AI_OBJECTIVE_GOTO)
			{
				// if the item is craftable and the NPC has the materials needed
				if (one_can_craft(objective_iterator->purpose) && crafting_requirements_met(objective_iterator->purpose))
				{
					// try to craft the item, using obj->purpose if the (obj->verb == GOTO), else use obj->noun (most cases)
					update_messages = craft(((objective_iterator->verb == C::AI_OBJECTIVE_GOTO) ? objective_iterator->purpose : objective_iterator->noun));

					if (update_messages.to_room != nullptr) // find a better way to determine if crafting was successful
					{
						// if successful, clear completed objectives

						if (objective_iterator->verb == C::AI_OBJECTIVE_GOTO)
						{
							// the item crafted was from a "goto" objective

							// save this because our firse erase will invalidate the iterator
							const std::string PURPOSE = objective_iterator->purpose;

							// erase the "goto" objective
							erase_goto_objective_matching(PURPOSE);

							// erase the "aquire" objective
							erase_acquire_objective_matching(PURPOSE);
						}
						else if (objective_iterator->noun == objective_iterator->purpose)
						{
							// this item is an end goal (no "parent" goal)
							erase_objectives_matching_purpose(objective_iterator->purpose);
						}
						else
						{
							// this item is only a means to an end
							erase_objective(objective_iterator);
						}

						return update_messages;
					}
				}
				// the item is not craftable, or it is not craftable at this time. Continue to
				// pathfind to the nearest instance of the item
				else if (pathfind_to_closest_item(objective_iterator->noun, update_messages))
				{
					return update_messages;
				}
			}

			// objectives were not modified, move to next objective
			++objective_iterator;
		}

		// the next block: work through all objectives, see which objectives can be resolved through crafting attemps.
		for (std::deque<Objective>::iterator objective_iterator = objectives.begin();
			objective_iterator != objectives.end(); ++objective_iterator)
		{
			// try to craft the item, using obj->purpose if the (obj->verb == GOTO), else use obj->noun (most cases)
			update_messages = craft(((objective_iterator->verb == C::AI_OBJECTIVE_GOTO) ? objective_iterator->purpose : objective_iterator->noun));

			if (update_messages.to_room != nullptr) // find a better way to determine if crafting was successful
			{
				// if successful, clear completed objectives

				if (objective_iterator->verb == C::AI_OBJECTIVE_GOTO)
				{
					// the item crafted was from a "goto" objective

					// save this because our firse erase will invalidate the iterator
					const std::string PURPOSE = objective_iterator->purpose;

					// erase the "goto" objective
					erase_goto_objective_matching(PURPOSE);

					// erase the "aquire" objective
					erase_acquire_objective_matching(PURPOSE);
				}
				else if (objective_iterator->noun == objective_iterator->purpose)
				{
					// this item is an end goal (no "parent" goal)
					erase_objectives_matching_purpose(objective_iterator->purpose);
				}
				else
				{
					// this item is only a means to an end
					erase_objective(objective_iterator);
				}

				return update_messages;
			}
		}
	}



	// count how many objectives the AI attempts, return after this passes AI_MAX_OBJECTIVE_ATTEMPTS to
	// prevent AI from getting stuck in infinite loops if no objective can be completed
	int objective_attempts = 0;

	// construct the outer wall
	for (std::deque<Objective>::iterator objective_it = objectives.begin();
		objective_it != objectives.end();)
	{
		// limit how many objective attempts the AI can make before control is returned
		if (++objective_attempts > C::AI_MAX_OBJECTIVE_ATTEMPTS) return Update_Messages("");

		// check if the objective is a construction objective
		if (objective_it->verb == C::AI_OBJECTIVE_CONSTRUCT)
		{
			// if the NPC is at the destination
			if (objective_it->objective_location == location)
			{
				// if the surface already exists, erase the objective and continue
				if (world->room_at(location)->has_surface((C::surface)objective_it->direction))
				{
					// erase() returns the next iterator
					objective_it = objectives.erase(objective_it);

					// next iteration
					continue;
				}

				// check if there is a tree in the way
				if (world->room_at(location)->contains(C::TREE_ID))
				{
					// if the axe is not equipped
					if (equipped_item == nullptr || equipped_item->get_name() != C::AXE_ID)
					{
						// equip the axe
						return equip(C::AXE_ID); // finished
					}

					// chop the tree
					return attack_item(C::TREE_ID); // finished
				}

				// determine whether the adjacent room has an opposite wall with a door. Neither needs to be intact.
				const Coordinate adjacent = location.get_after_move(objective_it->direction);

				// if an opposing surface exists, don't construct a surface here.
				if (world->room_at(adjacent)->has_surface(U::opposite_surface((C::surface)objective_it->direction)))
				{
					// If the NPC was about to build a surface with a door, move the door elsewhere.
					// It is possible that this will move the door to another structure.
					// It is therefore possible that a structure's only door could be moved to another structure.
					if (objective_it->modifier)
					{
						// starting with the current objective
						std::deque<Objective>::iterator it = objective_it;
						// for each objective
						while (++it != objectives.cend())
						{
							// if the objective is a construction objective AND its modifier is false (no door will be constructed)
							if (it->verb == C::AI_OBJECTIVE_CONSTRUCT && !it->modifier)
							{
								// set the flag to true
								it->modifier = true;
								// finished
								break;
							}
						}
					}

					// erase current objective
					objectives.erase(objective_it);

					// if there are more objectives
					if (objectives.size() > 0)
					{
						// continue with the next objective
						objective_it = objectives.begin();
						continue;
					}
					return Update_Messages(""); // there are no more objectives
				}

				// construct the surface, with a door if the modifier is true				
				update_messages = ((objective_it->modifier)
					? construct_surface_with_door(objective_it->material, (C::surface)objective_it->direction, objective_it->material)
					: construct_surface(objective_it->material, (C::surface)objective_it->direction));

				if (update_messages.to_user.find("You construct ") != std::string::npos) // find a better way to do this
				{
					// if successful, erase the objective and return
					objectives.erase(objective_it);
					return update_messages;
				}
			}
			// the NPC is not at the destination, attempt to pathfind to it
			else if (save_path_to(objective_it->objective_location))
			{
				// make the first move then return
				make_path_movement(update_messages);
				return update_messages;
			}

			// the NPC could not pathfind to the destination, try to move in the direction of the destination.
			if (best_attempt_pathfind(objective_it->objective_location, update_messages))
			{
				return update_messages;
			}

			// The NPC can't pathfind to the current destintation
			// Move the current objective to the end. Handle the next objective.

			// if the NPC is already on the last objective and could not pathfind to it
			if ((--objectives.end()) == objective_it)
			{
				return Update_Messages(""); // give up
			}

			// copy the current objective
			const Objective obj = *objective_it;

			// erase the current objective
			objectives.erase(objective_it);

			// add the objective to the end of the objective deque
			objectives.push_back(obj);

			// set the current objective back to the beginning of the list
			objective_it = objectives.begin();

			// begin the next iteration
			continue;
		}

		// next objective
		++objective_it;
	}



	if (planned_structures.size() > 0)
	{
		// for each planned structure
		for (std::deque<Structure_Objectives>::iterator structure_it = planned_structures.begin();
			structure_it != planned_structures.end();)
		{
			// if the structure has no remaining surfaces
			if (structure_it->structure_surface_objectives.size() == 0)
			{
				// erase the planned structure
				structure_it = planned_structures.erase(structure_it);
			}
			else
			{
				// move to next structure
				++structure_it;
			}
		}
	}



	if (planned_structures.size() > 0)
	{
		// select the next planned structure
		std::deque<Structure_Objectives>::iterator structure_it = planned_structures.begin();

		// for each construction objective
		for (std::vector<Objective>::iterator objective_it = structure_it->structure_surface_objectives.begin();
			objective_it != structure_it->structure_surface_objectives.end();)
		{
			// limit how many objective attempts the AI can make before control is returned
			if (++objective_attempts > C::AI_MAX_OBJECTIVE_ATTEMPTS) { return Update_Messages(""); }

			// if the NPC is at the destination
			if (objective_it->objective_location == location)
			{
				// if the doors have not yet been planned for this structure
				if (!structure_it->already_planned_doors())
				{
					// plan doors for this structure
					structure_it->plan_doors(world);

					// for some reason, in debug mode we're now obligated to reset the iterator to prevent invalid iterators
					objective_it = structure_it->structure_surface_objectives.begin();
				}
			}

			// if the NPC is at the destination (this check needs to be done a second time, because the iterator may change when doors are planned
			if (objective_it->objective_location == location)
			{
				// if the surface already exists, erase the objective and continue
				if (world->room_at(location)->has_surface((C::surface)objective_it->direction))
				{
					// erase() returns the next iterator
					objective_it = structure_it->structure_surface_objectives.erase(objective_it);

					// next iteration
					continue;
				}

				// check if there is a tree in the way
				if (world->room_at(location)->contains(C::TREE_ID))
				{
					// if the axe is not equipped
					if (equipped_item == nullptr || equipped_item->get_name() != C::AXE_ID)
					{
						// equip the axe
						return equip(C::AXE_ID);; // finished
					}

					// chop the tree
					return attack_item(C::TREE_ID); // finished
				}

				// construct the surface, with a door if the modifier is true				
				update_messages = ((objective_it->modifier)
					? construct_surface_with_door(objective_it->material, (C::surface)objective_it->direction, objective_it->material)
					: construct_surface(objective_it->material, (C::surface)objective_it->direction));

				if (update_messages.to_user.find("You construct ") != std::string::npos) // find a better way to do this
				{
					// if successful, erase the objective
					structure_it->structure_surface_objectives.erase(objective_it);

					// if this was the last construction objective for this structure, remove it
					if (structure_it->structure_surface_objectives.size() == 0)
					{
						planned_structures.erase(structure_it);
					}

					return update_messages;
				}
			}
			// the NPC is not at the destination, attempt to pathfind to it
			else if (save_path_to(objective_it->objective_location))
			{
				// make the first move then return
				make_path_movement(update_messages);
				return update_messages;
			}



			// the NPC could not pathfind to the destination, try to move in the direction of the destination.

			if (location.is_northwest_of(objective_it->objective_location)) // northwest
			{
				// if the target is out of view but inline with any part of the current visible area,
				// don't pathfind to corner; pathfind to the edge of the visible area that
				// is inline with the destination

				if (save_path_to(Coordinate(
					(((location.get_x() - objective_it->objective_location.get_x()) <= C::VIEW_DISTANCE) ? (objective_it->objective_location.get_x()) : (location.get_x() - C::VIEW_DISTANCE)),
					(((location.get_y() - objective_it->objective_location.get_y()) <= C::VIEW_DISTANCE) ? (objective_it->objective_location.get_y()) : (location.get_y() - C::VIEW_DISTANCE)))))
				{
					// attempt to move to the destination, return if successful
					if (make_path_movement(update_messages)) { return update_messages; }
				}
			}

			if (location.is_northeast_of(objective_it->objective_location)) // northeast
			{
				if (save_path_to(Coordinate(
					(((location.get_x() - objective_it->objective_location.get_x()) <= C::VIEW_DISTANCE) ? (objective_it->objective_location.get_x()) : (location.get_x() - C::VIEW_DISTANCE)),
					(((objective_it->objective_location.get_y() - location.get_y()) <= C::VIEW_DISTANCE) ? (objective_it->objective_location.get_y()) : (location.get_y() + C::VIEW_DISTANCE)))))
				{
					if (make_path_movement(update_messages)) { return update_messages; }
				}
			}

			if (location.is_southwest_of(objective_it->objective_location)) // southwest
			{
				if (save_path_to(Coordinate(
					(((objective_it->objective_location.get_x() - location.get_x()) <= C::VIEW_DISTANCE) ? (objective_it->objective_location.get_x()) : (location.get_x() + C::VIEW_DISTANCE)),
					(((location.get_y() - objective_it->objective_location.get_y()) <= C::VIEW_DISTANCE) ? (objective_it->objective_location.get_y()) : (location.get_y() - C::VIEW_DISTANCE)))))
				{
					if (make_path_movement(update_messages)) { return update_messages; }
				}
			}

			if (location.is_southeast_of(objective_it->objective_location)) // southeast
			{
				if (save_path_to(Coordinate(
					(((objective_it->objective_location.get_x() - location.get_x()) <= C::VIEW_DISTANCE) ? (objective_it->objective_location.get_x()) : (location.get_x() + C::VIEW_DISTANCE)),
					(((objective_it->objective_location.get_y() - location.get_y()) <= C::VIEW_DISTANCE) ? (objective_it->objective_location.get_y()) : (location.get_y() + C::VIEW_DISTANCE)))))
				{
					if (make_path_movement(update_messages)) { return update_messages; }
				}
			}

			// execution reaches here if a diagonal movement failed or the target is directly n/e/s/w or
			// the target is visible but unreachable

			if (location.get_x() > objective_it->objective_location.get_x()) // need to move north
			{
				// starting at the edge of view and working toward the player
				for (int i = C::VIEW_DISTANCE; i > 0; --i)
				{
					// if a path can be found
					if (save_path_to(Coordinate(location.get_x() - i, location.get_y())))
					{
						// make the first move
						make_path_movement(update_messages);

						if (location.get_x() == objective_it->objective_location.get_x()) // if the NPC is parallel with the destination, don't over shoot
						{
							path.clear();
						}

						return update_messages;
					}
				}
			}

			if (location.get_x() < objective_it->objective_location.get_x()) // need to move south
			{
				for (int i = C::VIEW_DISTANCE; i > 0; --i)
				{
					if (save_path_to(Coordinate(location.get_x() + i, location.get_y())))
					{
						make_path_movement(update_messages);

						if (location.get_x() == objective_it->objective_location.get_x())
						{
							path.clear();
						}

						return update_messages;
					}
				}
			}

			if (location.get_y() > objective_it->objective_location.get_y()) // need to move west
			{
				for (int i = C::VIEW_DISTANCE; i > 0; --i)
				{
					if (save_path_to(Coordinate(location.get_x(), location.get_y() - i)))
					{
						make_path_movement(update_messages);

						if (location.get_y() == objective_it->objective_location.get_y())
						{
							path.clear();
						}

						return update_messages;
					}
				}
			}

			if (location.get_y() < objective_it->objective_location.get_y()) // need to move east
			{
				for (int i = C::VIEW_DISTANCE; i > 0; --i)
				{
					if (save_path_to(Coordinate(location.get_x(), location.get_y() + i)))
					{
						make_path_movement(update_messages);

						if (location.get_y() == objective_it->objective_location.get_y())
						{
							path.clear();
						}

						return update_messages;
					}
				}
			}

			// a path could not be found
			return Update_Messages("");
		}
	}

	return Update_Messages("");
}

void Hostile_NPC_Worker::plan_fortress()
{
	// generate the x and y dimensions of the fortress
	const int FORTRESS_HEIGHT = U::random_int_from(C::FORTRESS_MIN_X, C::FORTRESS_MAX_X),
		FORTRESS_WIDTH = U::random_int_from(C::FORTRESS_MIN_Y, C::FORTRESS_MAX_Y);

	int fortress_x = 0, fortress_y = 0;

	// generate the fortress coordinates until the center of the fortress is on land
	do
	{
		fortress_x = U::random_int_from(0, C::WORLD_X_DIMENSION);
		fortress_y = U::random_int_from(0, C::WORLD_Y_DIMENSION);
	} while (U::euclidean_distance(C::WORLD_X_DIMENSION / 2, C::WORLD_Y_DIMENSION / 2,
		fortress_x + (FORTRESS_HEIGHT / 2),
		fortress_y + (FORTRESS_WIDTH / 2)) >= C::WORLD_X_DIMENSION * 0.45);

	// the first partition is the size of the fortress 
	std::vector<Partition> partitions;
	partitions.push_back(Partition(fortress_x, fortress_y, FORTRESS_HEIGHT, FORTRESS_WIDTH));

	// flag whether any changes were made
	bool partition_divided_on_last_pass;

	// generate partitions
	do
	{
		// reset flag
		partition_divided_on_last_pass = false;

		// for each partition
		for (unsigned i = 0; i < partitions.size(); ++i)
		{
			// draw a 50/50 chance of attemping a horizontal or vertical split
			if (U::random_int_from(0, 1) == 1)
			{
				// horizontal split

				// if the partition is large enough to split
				if (partitions[i].height > 5)
				{
					partition_divided_on_last_pass = true; // a split is about to be made

					// calculate the amount to remove from the existing partition
					const int split = U::random_int_from(C::FORTRESS_PARTITION_MIN_SIZE, partitions[i].height - C::FORTRESS_PARTITION_MIN_SIZE);

					// remove the amount from the existing partition
					partitions[i].height -= split;

					// Create a new partition, add it to the vector. The x position is the current partition's x location 
					// plus the current partition's height. The y position is the same as the current partition.
					// The height of the new partition is equal to the amount removed from the current partition.
					// The width of the new partition is the same as the width of the current partition.
					partitions.push_back(Partition(partitions[i]._x + partitions[i].height, partitions[i]._y, split, partitions[i].width));
				}
			}
			else
			{
				// vertical split

				// if the partition is large enough to split
				if (partitions[i].width > 5)
				{
					partition_divided_on_last_pass = true; // a split is about to be made

					// calculate the amount to remove from the existing partition
					const int split = U::random_int_from(C::FORTRESS_PARTITION_MIN_SIZE, partitions[i].width - C::FORTRESS_PARTITION_MIN_SIZE);

					// remvoe the amount from the existing partition
					partitions[i].width -= split;

					// Create a new partition, add it to the vector. The x positions is the current partitions's x location.
					// The y position is the current partition's y location plus the current partition's width. The height of
					// the new partition is the same as the height of the existing partition. THe width of the new partition is
					// equal to the amount removed from the current partition
					partitions.push_back(Partition(partitions[i]._x, partitions[i]._y + partitions[i].width, partitions[i].height, split));
				}
			}
		}
	} while (partition_divided_on_last_pass); // repeat as long as the flag indicates at least one partition was split

	std::vector<Structure> structures;

	// generate one structure inside each partition
	for (const Partition & partition : partitions)
	{
		// this is used to determine if the structure will be smaller than the partition
		// in the x dimension, or the y dimension
		const bool reduction_toggle = (U::random_int_from(0, 1) == 1);

		// the height and width are at no smaller than the minimum partition size minus 1, and are no larger than
		// the dimensions of the host partition, minus 1 or 0 depending on the toggle flag.
		const int structure_height = U::random_int_from(C::FORTRESS_PARTITION_MIN_SIZE - 1, partition.height - ((reduction_toggle) ? 1 : 0));
		const int structure_width = U::random_int_from(C::FORTRESS_PARTITION_MIN_SIZE - 1, partition.width - ((reduction_toggle) ? 0 : 1));

		// the structure can be placed anywhere inside of the partition
		const int structure_x = U::random_int_from(partition._x, (partition._x + partition.height) - structure_height);
		const int structure_y = U::random_int_from(partition._y, (partition._y + partition.width) - structure_width);

		// create a new structure using the generated parameters
		structures.push_back(Structure(structure_x, structure_y, structure_height, structure_width));
	}

	// Create a 2D vector of booleans. Set flags to true to represent the footprint structures within the fortress.
	// For alignment, subtract fortress_x from all x and height values, and subtract fortress_y from all
	// y and width values. The magic number +4 comes from the need to have a padding of 2 around all sides of the fortress.
	// The magic number +2 is for the same reason.
	std::vector<std::vector<bool>> fortress_footprint(FORTRESS_HEIGHT + 4, std::vector<bool>(FORTRESS_WIDTH + 4, false));
	for (const Structure & structure : structures)
	{
		for (int i = 0; i < structure.height; ++i)
		{
			for (int j = 0; j < structure.width; ++j)
			{
				fortress_footprint[(structure._x - fortress_x) + 2 + i][
					(structure._y - fortress_y) + 2 + j] = true;
			}
		}
	}

	// the footprint of each structure is known at this time; plan an outer wall around the fortress' structures
	plan_fortress_outer_wall(fortress_x, fortress_y, fortress_footprint);

	// now add construction objectives objectives for every surface of every structure
	for (const Structure & structure : structures)
	{
		// add construction objectives to a Structure_Objectives object
		Structure_Objectives structure_objectives;

		// iterate over the west side of the structure
		for (int x_coord = structure._x; x_coord <= (structure._x + structure.height) - 1; ++x_coord)
		{
			structure_objectives.add(Objective(C::AI_OBJECTIVE_CONSTRUCT, C::SURFACE, C::STONE_ID, C::direction::west, Coordinate(x_coord, structure._y), false));
		}

		// iterate over the south side of the structure
		for (int y_coord = structure._y; y_coord <= (structure._y + structure.width) - 1; ++y_coord)
		{
			structure_objectives.add(Objective(C::AI_OBJECTIVE_CONSTRUCT, C::SURFACE, C::STONE_ID, C::direction::south, Coordinate((structure._x + structure.height) - 1, y_coord), false));
		}

		// iterate over the north side of the structure
		for (int y_coord = structure._y; y_coord <= (structure._y + structure.width) - 1; ++y_coord)
		{
			structure_objectives.add(Objective(C::AI_OBJECTIVE_CONSTRUCT, C::SURFACE, C::STONE_ID, C::direction::north, Coordinate(structure._x, y_coord), false));
		}

		// iterate over the east side of the structure
		for (int x_coord = structure._x; x_coord <= (structure._x + structure.height) - 1; ++x_coord)
		{
			structure_objectives.add(Objective(C::AI_OBJECTIVE_CONSTRUCT, C::SURFACE, C::STONE_ID, C::direction::east, Coordinate(x_coord, (structure._y + structure.width) - 1), false));
		}

		// save the object containing the objectives
		planned_structures.push_back(structure_objectives);
	}
}

void Hostile_NPC_Worker::plan_fortress_outer_wall(const int & fortress_x, const int & fortress_y, const std::vector<std::vector<bool>> & fortress_footprint)
{
	// create a vector the same size as the fortress, populated with Area_type::fortress_exterior
	std::vector<std::vector<Area_Type>> area(fortress_footprint.size(), std::vector<Area_Type>(fortress_footprint[0].size(), Area_Type::fortress_exterior));
	
	// for each room that could possibly be a structure
	for (unsigned i = 2; i < fortress_footprint.size() - 2; ++i)
	{
		for (unsigned j = 2; j < fortress_footprint[0].size() - 2; ++j)
		{
			// if the room is a structure
			if (fortress_footprint[i][j])
			{
				// mark it as a structure on the area map
				area[i][j] = Area_Type::structure;

				// if the adjacent eight rooms are not structure, set them to interior

				if (area[i - 1][j - 1] != Area_Type::structure)
				{
					area[i - 1][j - 1] = Area_Type::fortress_interior;
				}
				if (area[i - 1][j] != Area_Type::structure)
				{
					area[i - 1][j] = Area_Type::fortress_interior;
				}
				if (area[i - 1][j + 1] != Area_Type::structure)
				{
					area[i - 1][j + 1] = Area_Type::fortress_interior;
				}

				if (area[i][j - 1] != Area_Type::structure)
				{
					area[i][j - 1] = Area_Type::fortress_interior;
				}
				if (area[i][j + 1] != Area_Type::structure)
				{
					area[i][j + 1] = Area_Type::fortress_interior;
				}

				if (area[i + 1][j - 1] != Area_Type::structure)
				{
					area[i + 1][j - 1] = Area_Type::fortress_interior;
				}
				if (area[i + 1][j] != Area_Type::structure)
				{
					area[i + 1][j] = Area_Type::fortress_interior;
				}
				if (area[i + 1][j + 1] != Area_Type::structure)
				{
					area[i + 1][j + 1] = Area_Type::fortress_interior;
				}
			}
		}
	}

	// create a 2D vector of booleans to indicate which nodes/rooms the below flood fill visited
	std::vector<std::vector<bool>> exterior_visited(area.size(), std::vector<bool>(area[0].size(), false));

	// use a deque-based 4-directional flood fill
	std::deque<Coordinate> flood_fill;

	// start with 0,0
	flood_fill.push_back(Coordinate(0, 0));

	class Flood_Node
	{
	public:
		int _x, _y;
	};

	// while there are still nodes to examine
	while (!flood_fill.empty())
	{
		// extract and remove the next node
		Coordinate node = flood_fill.front();
		flood_fill.erase(flood_fill.begin());

		// add neighboring nodes

		// if this is an exterior node
		if (area[node.get_x()][node.get_y()] == Area_Type::fortress_exterior)
		{
			// add each neighbor that has not been visisted
			if (node.get_x() - 1 >= 0 && !exterior_visited[node.get_x() - 1][node.get_y()]) // north
			{
				// if the adjacent node is not within the fortress
				if (area[node.get_x() - 1][node.get_y()] != Area_Type::fortress_interior)
				{
					// add the adjacent node to the list to explore
					flood_fill.push_front(Coordinate(node.get_x() - 1, node.get_y()));
					exterior_visited[node.get_x() - 1][node.get_y()] = true;
				}
				else // the adjacent node is within the fortress
				{
					// add an objective to construct an outer wall here
					objectives.push_front(Objective(C::AI_OBJECTIVE_CONSTRUCT, C::SURFACE, C::STONE_ID, C::direction::north,
						Coordinate(fortress_x + node.get_x() - 2, fortress_y + node.get_y() - 2),
						U::random_int_from(1, 6) == 1));
				}
			}
			if (node.get_x() + 1 < (int)area.size() && !exterior_visited[node.get_x() + 1][node.get_y()]) // south
			{
				if (area[node.get_x() + 1][node.get_y()] != Area_Type::fortress_interior)
				{
					flood_fill.push_front(Coordinate(node.get_x() + 1, node.get_y()));
					exterior_visited[node.get_x() + 1][node.get_y()] = true;
				}
				else
				{
					objectives.push_front(Objective(C::AI_OBJECTIVE_CONSTRUCT, C::SURFACE, C::STONE_ID, C::direction::south,
						Coordinate(fortress_x + node.get_x() - 2, fortress_y + node.get_y() - 2),
						U::random_int_from(1, 6) == 1));
				}
			}
			if (node.get_y() - 1 >= 0 && !exterior_visited[node.get_x()][node.get_y() - 1]) // west
			{
				if (area[node.get_x()][node.get_y() - 1] != Area_Type::fortress_interior)
				{
					flood_fill.push_front(Coordinate(node.get_x(), node.get_y() - 1));
					exterior_visited[node.get_x()][node.get_y() - 1] = true;
				}
				else
				{
					objectives.push_front(Objective(C::AI_OBJECTIVE_CONSTRUCT, C::SURFACE, C::STONE_ID, C::direction::west,
						Coordinate(fortress_x + node.get_x() - 2, fortress_y + node.get_y() - 2),
						U::random_int_from(1, 6) == 1));
				}
			}
			if (node.get_y() + 1 < (int)area.size() && !exterior_visited[node.get_x()][node.get_y() + 1]) // east
			{
				if (area[node.get_x()][node.get_y() + 1] != Area_Type::fortress_interior)
				{
					flood_fill.push_front(Coordinate(node.get_x(), node.get_y() + 1));
					exterior_visited[node.get_x()][node.get_y() + 1] = true;
				}
				else
				{
					objectives.push_front(Objective(C::AI_OBJECTIVE_CONSTRUCT, C::SURFACE, C::STONE_ID, C::direction::east,
						Coordinate(fortress_x + node.get_x() - 2, fortress_y + node.get_y() - 2),
						U::random_int_from(1, 6) == 1));
				}
			}
		}
	}
}

// subclass definitions

void Hostile_NPC_Worker::Structure_Objectives::add(const Objective & obj)
{
	structure_surface_objectives.push_back(obj);
}

void Hostile_NPC_Worker::Structure_Objectives::plan_doors(const std::observer_ptr<World> world)
{
	// this function only runs once
	if (doors_planned) { return; }
	doors_planned = true;

	// for each surface in this room
	for (unsigned i = 0; i < structure_surface_objectives.size();)
	{
		// get a coordinate to the adjacent room
		const Coordinate current = structure_surface_objectives[i].objective_location.get_after_move(structure_surface_objectives[i].direction);

		// determine if the adjacent room has an opposing wall
		if (world->room_at(current)->has_surface((C::surface)U::opposite_direction(structure_surface_objectives[i].direction)))
		{
			// if the adjacent room has an opposing wall, this wall will never be constructed; remove it
			structure_surface_objectives.erase(structure_surface_objectives.begin() + i);
		}
		else
		{
			// the surface is elegible for a door, advance
			++i;
		}
	}

	// all building have at least one door
	structure_surface_objectives[U::random_int_from(0, (int)structure_surface_objectives.size() - 1)].modifier = true;

	// half of buildings may have a second door
	if (U::random_int_from(1, 10) > 5)
	{
		structure_surface_objectives[U::random_int_from(0, (int)structure_surface_objectives.size() - 1)].modifier = true;
	}

	// a fifth of buildings may have a third door
	if (U::random_int_from(1, 10) > 8)
	{
		structure_surface_objectives[U::random_int_from(0, (int)structure_surface_objectives.size() - 1)].modifier = true;
	}
}

bool Hostile_NPC_Worker::Structure_Objectives::already_planned_doors() const
{
	return doors_planned;
}
