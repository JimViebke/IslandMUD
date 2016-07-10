
/* Jim Viebke
Jan 11 2016 */

#include "npc_enemy_corporal.h"

Hostile_NPC_Corporal::Hostile_NPC_Corporal(const std::string & name) : Hostile_NPC(name) {}

Update_Messages Hostile_NPC_Corporal::update(World & world, std::map<std::string, std::shared_ptr<Character>> & actors)
{
	Update_Messages update_messages("");

	if (hunt(world, actors, update_messages))
	{
		return update_messages;
	}

	// else, I have no hunt target or my hunt target is not visible

	// get new hunt target
	this->acquire_new_hunt_target(world, actors);

	// if the NPC still has no hunt target
	if (hunt_target_id == "")
	{
		return wander(world);
	}
	else // I now have a hunt target
	{
		if (hunt(world, actors, update_messages))
		{
			return update_messages;
		}
		else // I have no hunt target or my hunt target is not visible
		{
			hunt_target_id = "";
			return wander(world);
		}
	}

	return Update_Messages("");
}



void Hostile_NPC_Corporal::acquire_new_hunt_target(World & world, std::map<std::string, std::shared_ptr<Character>> & actors)
{
	// get a new hunt target. Will reset the hunter target ID if one cannot be found.

	// get a new hostile (player)
	hunt_target_id = get_new_hostile_id(world, actors);

	// this->hunt_target_id
}
Update_Messages Hostile_NPC_Corporal::wander(World & world)
{
	Update_Messages results("");

	// if no path is saved
	if (path.size() == 0)
	{
		// generate and store coordinates if none exist
		if (wander_x == -1 || wander_y == -1)
		{
			// generate and store new wander coordinates
			wander_x = U::random_int_from(0, C::WORLD_X_DIMENSION - 1);
			wander_y = U::random_int_from(0, C::WORLD_Y_DIMENSION - 1);
		}

		// test if the destinatino is within range or requires a best attempt pathfind
		if (U::diagonal_distance(x, y, wander_x, wander_y > C::VIEW_DISTANCE))
		{
			if (best_attempt_pathfind(wander_x, wander_y, world, results))
			{
				return results;
			}
			else
			{
				// what happens if a pathfind attempt fails?
			}
		}
		// the destination is visible, save a path to it
		else if (save_path_to(wander_x, wander_y, world))
		{
			make_path_movement(world, results);
			return results;
		}
		else
		{
			// what happens if destination is in range but cannot be reached?
		}
	}
	// use the stored path
	else
	{
		if (make_path_movement(world, results))
		{
			return results;
		}
		else
		{
			// what happens if a stored path fails?
		}
	}

	// nothing happens
	return results;
}
bool Hostile_NPC_Corporal::hunt(World & world, std::map<std::string, std::shared_ptr<Character>> & actors, Update_Messages & update_messages)
{
	// if no hunt target is saved
	if (hunt_target_id == "")
	{
		acquire_new_hunt_target(world, actors);
	}

	// if no hunt target is saved still
	if (hunt_target_id == "")
	{
		return false;
	}

	// extract a const iterator to the target
	const auto target_it = actors.find(hunt_target_id);

	// reset hunt target ID and return failure if the target is no longer online
	if (target_it == actors.cend())
	{
		hunt_target_id = "";
		return false;
	}

	// extract a reference to the target
	std::shared_ptr<Character> target = target_it->second;

	if (x == target->x && y == target->y)
	{
		// do combat logic
		update_messages = attack_character(target, world);
		return true;
	}
	// else, I have a path AND the target is on the path in view or out of view
	else if (path.size() > 0 &&
		((U::diagonal_distance(x, y, target->x, target->y) <= C::VIEW_DISTANCE && coordinates_are_on_path(target->x, target->y)
			|| U::diagonal_distance(x, y, target->x, target->y) > C::VIEW_DISTANCE)))
	{
		if (make_path_movement(world, update_messages)) return true;

		// the path failed, generate a new path and try again
		save_path_to(target->x, target->y, world);

		if (make_path_movement(world, update_messages)) return true;
	}
	// hunt target is visible
	else if (U::diagonal_distance(x, y, target->x, target->y) <= C::VIEW_DISTANCE)
	{
		// any planned path is no longer good, plan a new path

		save_path_to(target->x, target->y, world);

		if (make_path_movement(world, update_messages)) return true;
	}

	// unable to hunt target
	return false;



	// below is copy/pasted hunt logic



	//// if no hunt target could be found, return failure
	//if (hunt_target_id == "") return Update_Messages("");

	//// extract the target
	//const std::shared_ptr<Character> target = actors.find(hunt_target_id)->second;

	//// if the NPC is at the target's location
	//if (x == target->x && y == target->y)
	//{
	//	// do combat logic
	//}

	//// if a path can be found to the target
	//if (save_path_to(target->x, target->y, world))
	//{
	//	// update the stored path type
	//	stored_path_type = Stored_Path_Type::to_hunt_target_last_known_location;

	//	// move toward the target
	//	Update_Messages result("");
	//	if (make_path_movement(world, result))
	//	{
	//		// if the NPC was able to move to the target
	//		return result;
	//	}
	//}
	//else
	//{
	//	return wander(world, actors);
	//}
}
