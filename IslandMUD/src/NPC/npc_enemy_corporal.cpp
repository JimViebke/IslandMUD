
/* Jim Viebke
Jan 11 2016 */

#include "npc_enemy_corporal.h"

Hostile_NPC_Corporal::Hostile_NPC_Corporal(const std::string & name, std::observer_ptr<Game> game) : Hostile_NPC(name, game), wander_location(-1, -1) {}

Update_Messages Hostile_NPC_Corporal::update()
{
	Update_Messages update_messages("");

	// try hunt
	if (hunt(update_messages)) return update_messages;

	// Hunting failed, get a new hunt target.
	// This function is bad because it could return a visible but unreachable target,
	// resulting in a "hunt step, wander step, hunt step" loop until the target
	// becomes non-visible or reachable.
	acquire_new_hunt_target();

	// if the NPC still has no hunt target
	if (hunt_target_id == -1) return wander();

	// try hunt again
	if (hunt(update_messages)) return update_messages;

	// hunting failed with a new target, probably because the target is visible but unreachable

	hunt_target_id = -1;
	return wander();
}



void Hostile_NPC_Corporal::acquire_new_hunt_target()
{
	// Get a new hunt target ID, or 1 if one cannot be found.
	hunt_target_id = get_new_hostile_id();
}
Update_Messages Hostile_NPC_Corporal::wander()
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
			if (best_attempt_pathfind(wander_location, results))
			{
				return results;
			}
			else
			{
				// what happens if a pathfind attempt fails?
			}
		}
		// the destination is visible, save a path to it
		else if (save_path_to(wander_location))
		{
			make_path_movement(results);
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
		if (make_path_movement(results))
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
bool Hostile_NPC_Corporal::hunt(Update_Messages & update_messages)
{
	// if no hunt target is saved
	if (hunt_target_id == -1)
	{
		acquire_new_hunt_target();
	}

	// if no hunt target is saved still
	if (hunt_target_id == -1)
	{
		return false;
	}

	// extract a const iterator to the target
	const auto target_it = actors->find(hunt_target_id);

	// reset hunt target ID and return failure if the target is no longer online
	if (target_it == actors->cend())
	{
		hunt_target_id = -1;
		return false;
	}

	// extract a reference to the target
	std::shared_ptr<Character> target = target_it->second;

	if (location == target->get_location())
	{
		// do combat logic
		update_messages = attack_character(target);
		return true;
	}
	// else, I have a path AND the target is on the path in view or out of view
	else if (path.size() > 0 &&
		((location.diagonal_distance_to(target->get_location()) <= C::VIEW_DISTANCE && coordinates_are_on_path(target->get_location()))
			|| location.diagonal_distance_to(target->get_location()) > C::VIEW_DISTANCE))
	{
		if (make_path_movement(update_messages)) return true;

		// the path failed, generate a new path and try again
		save_path_to(target->get_location());

		if (make_path_movement(update_messages)) return true;
	}
	// hunt target is visible
	else if (location.diagonal_distance_to(target->get_location()) <= C::VIEW_DISTANCE)
	{
		// any planned path is no longer good, plan a new path

		save_path_to(target->get_location());

		if (make_path_movement(update_messages)) return true;
	}

	// unable to hunt target
	return false;
}

void Hostile_NPC_Corporal::generate_new_wander_location()
{
	wander_location = Coordinate(
		U::random_int_from(0, C::WORLD_X_DIMENSION - 1),
		U::random_int_from(0, C::WORLD_Y_DIMENSION - 1));
}
