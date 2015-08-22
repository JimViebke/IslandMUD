
/* Jim Viebke
Aug 15 2015 */

#include "npc_enemy_worker.h"

void Hostile_NPC_Worker::update(World & world, map<string, shared_ptr<Character>> & actors)
{
	if (!fortress_planned)
	{
		plan_fortress();
		fortress_planned = true; // only set the flag after plans have been made

		for (int i = 0; i < 10000; ++i) // cheating here to start the NPC off with materials
		{
			// start off with some materials
			this->add(Craft::make(C::STONE_ID));
		}
	}

	// for each objective
	for (deque<Objective>::iterator objective_it = objectives.begin();
		objective_it != objectives.end();)
	{
		// check if the objective is a construction objective
		if (objective_it->verb == C::AI_OBJECTIVE_CONSTRUCT)
		{
			// if we are at the destination
			if (x == objective_it->objective_x && y == objective_it->objective_y)
			{
				// if the surface already exists, erase the objective and continue
				if (world.room_at(x, y, z)->has_surface(objective_it->direction))
				{
					// erase() returns the next iterator
					objective_it = objectives.erase(objective_it);

					// next iteration
					continue;
				}

				// check if there is a tree in the way
				if (world.room_at(x, y, z)->contains_item(C::TREE_ID))
				{
					// tree needs chopping - just simulate it for now
					world.room_at(x, y, z)->remove_item(C::TREE_ID);
					return;
				}

				// construct the surface
				// If the objective's modifier is true (in this case meaning the wall will have a door), compare
				// the results of constructing a surface with a door. If the objective's modifier is false,
				// compare the results of constructing just a surface

				// HERE ****** The should also take into account if the adjacent room has an opposite wall with a door. Then
				// this room should have a door there as well.
				if (((objective_it->modifier) ?
					(construct_surface_with_door(objective_it->material, objective_it->direction, objective_it->material, world).find("You construct a ")) :
					(construct_surface(objective_it->material, objective_it->direction, world).find("You construct a ")))
					!= string::npos)
				{
					// if successful, erase the objective and return
					objectives.erase(objective_it);
					return;
				}
			}
			// we are not at the destination, attempt to pathfind to it
			else if (pathfind(objective_it->objective_x, objective_it->objective_y, world))
			{
				return;
			}
			// we could not pathfind to the destination, test if it is out of sight range
			else if (R::diagonal_distance(x, y, objective_it->objective_x, objective_it->objective_y) > C::VIEW_DISTANCE)
			{
				// The location is out of sight range. Move in the direction of the destination.

				if (x > objective_it->objective_x && y > objective_it->objective_y) // northwest
				{
					// if the target is out of view but inline with any part of the current visible area,
					// don't pathfind to corner; pathfind to the edge of the visible area that
					// is inline with the destination

					if (pathfind(
						(((x - objective_it->objective_x) <= C::VIEW_DISTANCE) ? (objective_it->objective_x) : (x - C::VIEW_DISTANCE)),
						(((y - objective_it->objective_y) <= C::VIEW_DISTANCE) ? (objective_it->objective_y) : (y - C::VIEW_DISTANCE)), world))
					{
						return;
					}
				}

				if (x > objective_it->objective_x && y < objective_it->objective_y) // northeast
				{
					if (pathfind(
						(((x - objective_it->objective_x) <= C::VIEW_DISTANCE) ? (objective_it->objective_x) : (x - C::VIEW_DISTANCE)),
						(((objective_it->objective_y - y) <= C::VIEW_DISTANCE) ? (objective_it->objective_y) : (y + C::VIEW_DISTANCE)), world))
					{
						return;
					}
				}

				if (x < objective_it->objective_x && y > objective_it->objective_y) // southwest
				{
					if (pathfind(
						(((objective_it->objective_x - x) <= C::VIEW_DISTANCE) ? (objective_it->objective_x) : (x + C::VIEW_DISTANCE)),
						(((y - objective_it->objective_y) <= C::VIEW_DISTANCE) ? (objective_it->objective_y) : (y - C::VIEW_DISTANCE)), world))
					{
						return;
					}
				}

				if (x < objective_it->objective_x && y < objective_it->objective_y) // southeast
				{
					if (pathfind(
						(((objective_it->objective_x - x) <= C::VIEW_DISTANCE) ? (objective_it->objective_x) : (x + C::VIEW_DISTANCE)),
						(((objective_it->objective_y - y) <= C::VIEW_DISTANCE) ? (objective_it->objective_y) : (y + C::VIEW_DISTANCE)), world))
					{
						return;
					}
				}

				// execution reaches here if a diagonal movment failed or the target is directly n/e/s/w

				if (x > objective_it->objective_x) // north
				{
					if (pathfind(x - C::VIEW_DISTANCE, y, world)) { return; }
				}

				if (x < objective_it->objective_x) // south
				{
					if (pathfind(x + C::VIEW_DISTANCE, y, world)) { return; }
				}

				if (y > objective_it->objective_y) // west
				{
					if (pathfind(x, y - C::VIEW_DISTANCE, world)) { return; }
				}

				if (y < objective_it->objective_y) // east
				{
					if (pathfind(x, y + C::VIEW_DISTANCE, world)) { return; }
				}
			}
		}

		// next objective
		++objective_it;
	}
}

void Hostile_NPC_Worker::plan_fortress()
{
	// generate the x and y dimensions of the fortress
	const int FORTRESS_HEIGHT = R::random_int_from(C::FORTRESS_MIN_X, C::FORTRESS_MAX_X),
		FORTRESS_WIDTH = R::random_int_from(C::FORTRESS_MIN_Y, C::FORTRESS_MAX_Y);

	int fortress_x = 0, fortress_y = 0;

	// generate the fortress coordinates until the center of the fortress is on land
	do
	{
		fortress_x = R::random_int_from(0, C::WORLD_X_DIMENSION);
		fortress_y = R::random_int_from(0, C::WORLD_Y_DIMENSION);
	} while (R::euclidean_distance(C::WORLD_X_DIMENSION / 2, C::WORLD_Y_DIMENSION / 2,
		fortress_x + (FORTRESS_HEIGHT / 2),
		fortress_y + (FORTRESS_WIDTH / 2)) >= C::WORLD_X_DIMENSION * 0.45);


	// the first partition is the size of the fortress 
	vector<Partition> partitions;
	partitions.push_back(Partition(fortress_x, fortress_y, FORTRESS_HEIGHT, FORTRESS_WIDTH));

	// flag whether any changes were made
	bool partition_divided_on_last_pass;

	do
	{
		// reset flag
		partition_divided_on_last_pass = false;

		// for each partition
		for (unsigned i = 0; i < partitions.size(); ++i)
		{
			// draw a 50/50 chance of attemping a horizontal or vertical split
			if (R::random_int_from(0, 1) == 1)
			{
				// horizontal split

				// if the partition is large enough to split
				if (partitions[i].height > 5)
				{
					partition_divided_on_last_pass = true; // a split is about to be made

					// calculate the amount to remove from the existing partition
					const int split = R::random_int_from(C::FORTRESS_PARTITION_MIN_SIZE, partitions[i].height - C::FORTRESS_PARTITION_MIN_SIZE);

					// remove the amount from the existing partition
					partitions[i].height -= split;

					// Create a new partition, add it to the vector. The x position is the current partition's x location 
					// plus the current partition's height. The y position is the same as the current partition.
					// The height of the new partition is equal to the amount removed from the current partition.
					// The width of the new partition is the same as the width of the current partition.
					partitions.push_back(Partition(partitions[i].x + partitions[i].height, partitions[i].y, split, partitions[i].width));
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
					const int split = R::random_int_from(C::FORTRESS_PARTITION_MIN_SIZE, partitions[i].width - C::FORTRESS_PARTITION_MIN_SIZE);

					// remvoe the amount from the existing partition
					partitions[i].width -= split;

					// Create a new partition, add it to the vector. The x positions is the current partitions's x location.
					// The y position is the current partition's y location plus the current partition's width. The height of
					// the new partition is the same as the height of the existing partition. THe width of the new partition is
					// equal to the amount removed from the current partition
					partitions.push_back(Partition(partitions[i].x, partitions[i].y + partitions[i].width, partitions[i].height, split));
				}
			}
		}
	} while (partition_divided_on_last_pass); // repeat as long as the flag indicates at least one partition was split

	// this prints the location and dimensions of each partition
	/* cout << "partitions = [\n";
	for (int i = 0; i < partitions.size(); ++i)
	{
	cout << "[" << partitions[i].x << ", " << partitions[i].y << ", " << partitions[i].height << ", " << partitions[i].width << "], ";
	if ((i + 1) % 5 == 0) { cout << endl; }
	}
	cout << "]" << endl; */

	vector<Structure> structures;

	// generate one structure inside each partition
	for (const Partition & partition : partitions)
	{
		// this is used to determine if the structure will be smaller than the partition
		// in the x dimension, or the y dimension
		const bool reduction_toggle = (R::random_int_from(0, 1) == 1);

		// the height and width are at no smaller than the minimum partition size minus 1, and are no larger than
		// the dimensions of the host partition, minus 1 or 0 depending on the toggle flag.
		const int structure_height = R::random_int_from(C::FORTRESS_PARTITION_MIN_SIZE - 1, partition.height - ((reduction_toggle) ? 1 : 0));
		const int structure_width = R::random_int_from(C::FORTRESS_PARTITION_MIN_SIZE - 1, partition.width - ((reduction_toggle) ? 0 : 1));

		// the structure can be placed anywhere inside of the partition
		const int structure_x = R::random_int_from(partition.x, (partition.x + partition.height) - structure_height);
		const int structure_y = R::random_int_from(partition.y, (partition.y + partition.width) - structure_width);

		// create a new structure using the generated parameters
		structures.push_back(Structure(structure_x, structure_y, structure_height, structure_width));
	}

	// this prints the location and dimensions of each structure in the fortress
	/* cout << "structures = [\n";
	for (int i = 0; i < structures.size(); ++i)
	{
	cout << "[" << structures[i].x << ", " << structures[i].y << ", " << structures[i].height << ", " << structures[i].width << "], ";
	if ((i + 1) % 5 == 0) { cout << endl; }
	}
	cout << "]" << endl; */

	// now add construction objectives objectives for every surface of every structure
	for (const Structure & structure : structures)
	{
		// the purpose of the structure could be determined here, along with
		// the material the structure should be made out of

		// determine the location of doors on the current structure

		const int structure_diameter = (structure.width + structure.height * 2);

		// all buildings have a door, located anywhere
		int door_one_position = R::random_int_from(1, structure_diameter),
			door_two_position = -1,
			door_three_position = -1;

		const int door_spawn = R::random_int_from(1, 10);

		// half of buildings have a second door, located anywhere other than the first door
		if (door_spawn > 5)
		{
			// Use do..while loops to ensure all three possible doors have unique positions on the structure

			do
			{
				door_two_position = R::random_int_from(1, structure_diameter);
			} while (door_two_position == door_one_position);

			// a fifth of buildings have a third door, located anywhere other than the first two doors
			if (door_spawn > 8)
			{
				do
				{
					door_three_position = R::random_int_from(1, structure_diameter);
				} while (door_three_position == door_one_position || door_three_position == door_two_position);
			}
		}

		// add construction objectives

		// create a counter that will be incremented as each surface is planned
		// When the counter equals a door position, plan to construct a surface with a door
		// instead of just a door, using the modifier flag of the Objective object
		int door_counter = 1;

		// iterate over the west side of the structure
		for (int x_coord = structure.x; x_coord <= (structure.x + structure.height) - 1; ++x_coord)
		{
			this->objectives.push_back(Objective(C::AI_OBJECTIVE_CONSTRUCT, C::SURFACE, C::STONE_ID, C::WEST, x_coord, structure.y, C::GROUND_INDEX,
				(door_counter == door_one_position || door_counter == door_two_position || door_counter == door_three_position)));
			++door_counter;
		}

		// iterate over the south side of the structure
		for (int y_coord = structure.y; y_coord <= (structure.y + structure.width) - 1; ++y_coord)
		{
			this->objectives.push_back(Objective(C::AI_OBJECTIVE_CONSTRUCT, C::SURFACE, C::STONE_ID, C::SOUTH, (structure.x + structure.height) - 1, y_coord, C::GROUND_INDEX,
				(door_counter == door_one_position || door_counter == door_two_position || door_counter == door_three_position)));
			++door_counter;
		}

		// iterate over the north side of the structure
		for (int y_coord = structure.y; y_coord <= (structure.y + structure.width) - 1; ++y_coord)
		{
			this->objectives.push_back(Objective(C::AI_OBJECTIVE_CONSTRUCT, C::SURFACE, C::STONE_ID, C::NORTH, structure.x, y_coord, C::GROUND_INDEX,
				(door_counter == door_one_position || door_counter == door_two_position || door_counter == door_three_position)));
			++door_counter;
		}

		// iterate over the east side of the structure
		for (int x_coord = structure.x; x_coord <= (structure.x + structure.height) - 1; ++x_coord)
		{
			this->objectives.push_back(Objective(C::AI_OBJECTIVE_CONSTRUCT, C::SURFACE, C::STONE_ID, C::EAST, x_coord, (structure.y + structure.width) - 1, C::GROUND_INDEX,
				(door_counter == door_one_position || door_counter == door_two_position || door_counter == door_three_position)));
			++door_counter;
		}

		// finished planning for this structure
	}

	// finished planning for all structures
}
