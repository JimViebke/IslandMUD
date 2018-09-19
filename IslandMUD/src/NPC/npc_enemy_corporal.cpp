
/* Jim Viebke
Jan 11 2016 */

#include "npc_enemy_corporal.h"

Hostile_NPC_Corporal::Hostile_NPC_Corporal(const std::string & name, std::unique_ptr<World> & world) : Hostile_NPC(name, world), wander_location(-1, -1) {}

Update_Messages Hostile_NPC_Corporal::update(std::unique_ptr<World> & world, std::map<character_id, std::shared_ptr<Character>> & actors)
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
	if (hunt_target_id == -1)
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
			hunt_target_id = -1;
			return wander(world);
		}
	}

	return Update_Messages("");
}



void Hostile_NPC_Corporal::acquire_new_hunt_target(std::unique_ptr<World> & world, std::map<character_id, std::shared_ptr<Character>> & actors)
{
	// get a new hunt target. Will reset the hunter target ID if one cannot be found.

	// get a new hostile (player)
	hunt_target_id = get_new_hostile_id(world, actors);

	// this->hunt_target_id
}
Update_Messages Hostile_NPC_Corporal::wander(std::unique_ptr<World> & world)
{
	Update_Messages results("");

	// if we're already in the wander location, generate a new one
	if (wander_location == location)
	{
		generate_new_wander_location();
	}

	// if no path is saved
	if (path.size() == 0)
	{
		// generate and store coordinates if none exist
		if (!wander_location.is_valid())
		{
			generate_new_wander_location();
		}

		// test if the destination is within range or requires a best attempt pathfind
		if (location.diagonal_distance_to(wander_location) > C::VIEW_DISTANCE)
		{
			if (best_attempt_pathfind(wander_location, world, results))
			{
				return results;
			}
			else
			{
				// what happens if a pathfind attempt fails?
			}
		}
		// the destination is visible, save a path to it
		else if (save_path_to(wander_location, world))
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
bool Hostile_NPC_Corporal::hunt(std::unique_ptr<World> & world, std::map<character_id, std::shared_ptr<Character>> & actors, Update_Messages & update_messages)
{
	// if no hunt target is saved
	if (hunt_target_id == -1)
	{
		acquire_new_hunt_target(world, actors);
	}

	// if no hunt target is saved still
	if (hunt_target_id == -1)
	{
		return false;
	}

	// extract a const iterator to the target
	const auto target_it = actors.find(hunt_target_id);

	// reset hunt target ID and return failure if the target is no longer online
	if (target_it == actors.cend())
	{
		hunt_target_id = -1;
		return false;
	}

	// extract a reference to the target
	std::shared_ptr<Character> target = target_it->second;

	if (location == target->get_location())
	{
		// do combat logic
		update_messages = attack_character(target, world);
		return true;
	}
	// else, I have a path AND the target is on the path in view or out of view
	else if (path.size() > 0 &&
		((location.diagonal_distance_to(target->get_location()) <= C::VIEW_DISTANCE && coordinates_are_on_path(target->get_location()))
			|| location.diagonal_distance_to(target->get_location()) > C::VIEW_DISTANCE))
	{
		if (make_path_movement(world, update_messages)) return true;

		// the path failed, generate a new path and try again
		save_path_to(target->get_location(), world);

		if (make_path_movement(world, update_messages)) return true;
	}
	// hunt target is visible
	else if (location.diagonal_distance_to(target->get_location()) <= C::VIEW_DISTANCE)
	{
		// any planned path is no longer good, plan a new path

		save_path_to(target->get_location(), world);

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

void Hostile_NPC_Corporal::generate_new_wander_location()
{
	wander_location = Coordinate(
		U::random_int_from(0, C::WORLD_X_DIMENSION - 1),
		U::random_int_from(0, C::WORLD_Y_DIMENSION - 1));
}
