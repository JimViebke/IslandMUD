
#include "npc_enemy_bodyguard.h"

Update_Messages Hostile_NPC_Bodyguard::update(World & world, std::map<std::string, std::shared_ptr<Character>> & actors)
{
	// NPC bodyguards cheat by knowing their protect_target's location. Gameplay impact should be negligible.

	// idle if no protect_target has been set
	if (protect_target_id == "") return Update_Messages("");

	// if the protect_target does not exist
	if (actors.find(protect_target_id) == actors.end())
	{
		// reset and idle
		protect_target_id = "";
		return Update_Messages("");
	}

	// extract protect_target
	const std::shared_ptr<Character> protect_target = actors.find(protect_target_id)->second;

	// return to the protect_target if the NPC pursues the hunt_target to far.
	check_maximum_hunt_radius(protect_target);

	//	if I have a hunt_target
	if (hunt_target_id != "")
	{
		// extract the hunt_target
		std::shared_ptr<Character> hunt_target = actors.find(hunt_target_id)->second;

		// if the target is not in the actors list, the target is no longer in game
		if (hunt_target == nullptr)
		{
			hunt_target_id = ""; // reset target
		}
		else // the target is online
		{
			return this->hunt_target(hunt_target, protect_target, world, actors);
		}
	}

	// else, I do not have a hunt target

	// if I am out of guard range of my protect_target
	if (U::diagonal_distance(x, y, protect_target->x, protect_target->y) > guard_radius)
	{
		Update_Messages update_messages("");
		if (move_toward_protect_target(protect_target, world, actors, update_messages))
		{
			// approaching the protect_target succeeded
			return update_messages;
		}
		else // approaching the protect_target failed
		{
			// attempt remedial pathfinding
			if (best_attempt_pathfind(protect_target->x, protect_target->y, world, update_messages))
			{
				return update_messages;
			}
		}
	}

	// the NPC is in range of the protect target, check for a new hunt target

	// if a new hunt target can be found
	if (attempt_set_new_hunt_target(world, actors))
	{
		return approach_new_hunt_target(world);
	}

	// else, idle
	return Update_Messages("");
}

void Hostile_NPC_Bodyguard::set_protect_target(const std::string & set_protect_target_id)
{
	this->protect_target_id = set_protect_target_id;
}

bool Hostile_NPC_Bodyguard::attempt_set_new_hunt_target(World & world, std::map<std::string, std::shared_ptr<Character>> & actors)
{
	std::vector<std::string> hostile_ids;

	// for each visible room
	for (int cx = x - (int)C::VIEW_DISTANCE; cx <= x + (int)C::VIEW_DISTANCE; ++cx)
	{
		for (int cy = y - (int)C::VIEW_DISTANCE; cy <= y + (int)C::VIEW_DISTANCE; ++cy)
		{
			// for each actor in the room
			for (const std::string & actor_ID : world.room_at(cx, cy, z)->get_actor_ids())
			{
				// if the actor is a player character
				if (U::is<PC>(actors.find(actor_ID)->second))
				{
					// save the player character's ID
					hostile_ids.push_back(actor_ID);
				}
			}
		}
	}

	// no players are visible, no target was found
	if (hostile_ids.size() == 0) { return false; }

	// at least one player character is in range

	// pick a random player character, save their ID
	this->hunt_target_id = U::random_element_from(hostile_ids);

	// create a pointer to the player
	std::shared_ptr<Character> hunt_target = actors.find(hunt_target_id)->second;

	// save the player's current location as the player's last know location (in case they walk out of visible range)
	hunt_target_last_known_location._x = hunt_target->x;
	hunt_target_last_known_location._y = hunt_target->y;
	hunt_target_last_known_location._z = hunt_target->z;

	// we have a target
	return true;
}
bool Hostile_NPC_Bodyguard::attempt_update_hunt_target_last_known_location(const std::shared_ptr<Character> & hunt_target)
{
	// if the NPC can see the hunt_target
	if (U::diagonal_distance(x, y, hunt_target->x, hunt_target->y) <= C::VIEW_DISTANCE)
	{
		// update hunt_target_last_known_location
		hunt_target_last_known_location._x = hunt_target->x;
		hunt_target_last_known_location._y = hunt_target->y;
		hunt_target_last_known_location._z = hunt_target->z;
		return true;
	}

	// the NPC cannot see the hunt target, the last_known_location was not updated
	return false;
}

// AI subroutines

void Hostile_NPC_Bodyguard::check_maximum_hunt_radius(const std::shared_ptr<Character> & protect_target)
{
	// if the NPC has hunted a threat beyond the hunt radius
	if (U::diagonal_distance(x, y, protect_target->x, protect_target->y) > this->hunt_radius)
	{
		// forget about the threat, which will cause the NPC to return to the protect_target
		hunt_target_last_known_location.reset();
		hunt_target_id.clear();
	}
}

Update_Messages Hostile_NPC_Bodyguard::hunt_target(std::shared_ptr<Character> & hunt_target, const std::shared_ptr<Character> & protect_target, World & world, std::map<std::string, std::shared_ptr<Character>> & actors)
{
	// if I am at the target's location, do combat logic
	if (hunt_target->x == x && hunt_target->y == y && hunt_target->z == z)
	{
		return Update_Messages(""); // combat logic here
	}

	// else the target is online and I am not at the target's location

	// if the hunt target is visible
	if (attempt_update_hunt_target_last_known_location(hunt_target))
	{
		// if the path is empty or going the wrong direction, or the target has moved
		if (path.size() == 0 || stored_path_type != Stored_Path_Type::to_hunt_target ||
			hunt_target->x != path.back()._x ||
			hunt_target->y != path.back()._y)
		{
			// generate a new path
			save_path_to(hunt_target->x, hunt_target->y, world);
			stored_path_type = Stored_Path_Type::to_hunt_target;
		}

		// make the next movement
		Update_Messages update_messages("");
		if (!make_path_movement(world, update_messages)) // if the next movement fails, regenerate the path and try again
		{
			save_path_to(hunt_target->x, hunt_target->y, world);
			make_path_movement(world, update_messages);
		}

		return update_messages;
	}

	// else the hunt target is not visible

	// if have a last_known_location for hunt_target
	if (hunt_target_last_known_location._x != -1 &&
		hunt_target_last_known_location._y != -1)
	{
		// if my location is not last_known_location
		if (hunt_target_last_known_location._x != x ||
			hunt_target_last_known_location._y != y ||
			hunt_target_last_known_location._z != z)
		{
			// if the path is empty or going to the wrong destination
			if (path.size() == 0 || stored_path_type != Stored_Path_Type::to_hunt_target)
			{
				// generate a new path
				save_path_to(hunt_target_last_known_location._x, hunt_target_last_known_location._y, world);
				stored_path_type = Stored_Path_Type::to_hunt_target;
			}

			// make the next move
			Update_Messages update_messages("");
			if (!make_path_movement(world, update_messages)) // if the next movement fails, regenerate the path and try again
			{
				save_path_to(hunt_target_last_known_location._x, hunt_target_last_known_location._y, world);
				make_path_movement(world, update_messages);
			}

			return update_messages; // action was used
		}

		// else I am at the last known location and cannot see the target (this condition is handled in the next block)
	}

	// hunt target is not visible and I don't have a last known location for the hunt target
	hunt_target_last_known_location.reset();

	// if the path is empty or going to the wrong destination
	if (path.size() == 0 || stored_path_type != Stored_Path_Type::to_protect_target)
	{
		// generate a new path
		save_path_to(protect_target->x, protect_target->y, world);
		stored_path_type = Stored_Path_Type::to_protect_target;
	}

	// make the next movement
	Update_Messages update_messages("");
	make_path_movement(world, update_messages);

	// if the hunt target is (now) visible
	if (attempt_update_hunt_target_last_known_location(hunt_target))
	{
		save_path_to(hunt_target_last_known_location._x, hunt_target_last_known_location._y, world); // save a new path
		stored_path_type = Stored_Path_Type::to_hunt_target; // update the stored path type
	}

	return update_messages; // action was used
}

bool Hostile_NPC_Bodyguard::move_toward_protect_target(const std::shared_ptr<Character> & protect_target, World & world, std::map<std::string, std::shared_ptr<Character>> & actors, Update_Messages & update_messages)
{
	// if the path is empty or going to the wrong destination
	if (path.size() == 0 || stored_path_type != Stored_Path_Type::to_protect_target)
	{
		// generate a new path
		save_path_to(protect_target->x, protect_target->y, world);
		stored_path_type = Stored_Path_Type::to_protect_target;
	}

	// make the next movement
	return make_path_movement(world, update_messages);
}

Update_Messages Hostile_NPC_Bodyguard::approach_new_hunt_target(World & world)
{
	// if the path is empty or going to the wrong destination
	if (path.size() == 0 || stored_path_type != Stored_Path_Type::to_hunt_target)
	{
		// generate a new path
		save_path_to(hunt_target_last_known_location._x, hunt_target_last_known_location._y, world);
		stored_path_type = Stored_Path_Type::to_hunt_target;
	}

	// make the next movement
	Update_Messages update_messages("");
	make_path_movement(world, update_messages);

	return update_messages; // action was used
}