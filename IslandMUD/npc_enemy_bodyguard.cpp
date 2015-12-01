
#include "npc_enemy_bodyguard.h"

Update_Messages Hostile_NPC_Bodyguard::update(World & world, map<string, shared_ptr<Character>> & actors)
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
	shared_ptr<Character> protect_target = actors.find(protect_target_id)->second;

	//	if I have a kill_target
	if (kill_target_id != "")
	{
		// extract the kill_target
		shared_ptr<Character> kill_target = actors.find(kill_target_id)->second;

		// if the target is not in the actors list, the target is no longer in game
		if (kill_target == nullptr)
		{
			kill_target_id = ""; // reset target
		}
		else // the target is online
		{
			// if I am at the target's location, do combat logic
			if (kill_target->x == x && kill_target->y == y && kill_target->z == z)
			{
				return Update_Messages(""); // combat logic here
			}

			// else the target is online and I am not at the target's location

			// if the kill target is visible
			if (attempt_update_kill_target_last_known_location(kill_target))
			{
				// if the path is empty or going the wrong direction, or the target has moved
				if (path.size() == 0 || stored_path_type != Stored_Path_Type::to_kill_target ||
					kill_target->x != path.back()._x ||
					kill_target->y != path.back()._y)
				{
					// generate a new path
					save_path_to(kill_target->x, kill_target->y, world);
					stored_path_type = Stored_Path_Type::to_kill_target;
				}

				// make the next movement
				Update_Messages update_messages("");
				if (!make_path_movement(world, update_messages)) // if the next movement fails, regenerate the path and try again
				{
					save_path_to(kill_target->x, kill_target->y, world);
					make_path_movement(world, update_messages);
				}

				return update_messages;
			}

			// else the kill target is not visible

			// if have a last_known_location for kill_target
			if (kill_target_last_known_location._x != -1 &&
				kill_target_last_known_location._y != -1)
			{
				// if my location is not last_known_location
				if (kill_target_last_known_location._x != x ||
					kill_target_last_known_location._y != y ||
					kill_target_last_known_location._z != z)
				{
					// if the path is empty or going to the wrong destination
					if (path.size() == 0 || stored_path_type != Stored_Path_Type::to_kill_target)
					{
						// generate a new path
						save_path_to(kill_target_last_known_location._x, kill_target_last_known_location._y, world);
						stored_path_type = Stored_Path_Type::to_kill_target;
					}

					// make the next move
					Update_Messages update_messages("");
					if (!make_path_movement(world, update_messages)) // if the next movement fails, regenerate the path and try again
					{
						save_path_to(kill_target_last_known_location._x, kill_target_last_known_location._y, world);
						make_path_movement(world, update_messages);
					}

					return update_messages; // action was used
				}

				// else I am at the last known location and cannot see the target (this condition is handled in the next block)
			}

			// kill target is not visible and I don't have a last known location for the kill target
			kill_target_last_known_location.reset();

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

			// if the kill target is (now) visible
			if (attempt_update_kill_target_last_known_location(kill_target))
			{
				save_path_to(kill_target_last_known_location._x, kill_target_last_known_location._y, world); // save a new path
				stored_path_type = Stored_Path_Type::to_kill_target; // update the stored path type
			}

			return update_messages; // action was used
		}
	}

	// else, I do not have a kill target

	// if I am out of guard range of my protect_target
	if (U::diagonal_distance(x, y, protect_target->x, protect_target->y) > guard_radius)
	{
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

		return update_messages; // action was used
	}

	// the NPC is in range of the protect target, check for a new kill target

	// if a new kill target can be found
	if (attempt_set_new_kill_target(world, actors))
	{
		// if the path is empty or going to the wrong destination
		if (path.size() == 0 || stored_path_type != Stored_Path_Type::to_kill_target)
		{
			// generate a new path
			save_path_to(kill_target_last_known_location._x, kill_target_last_known_location._y, world);
			stored_path_type = Stored_Path_Type::to_kill_target;
		}

		// make the next movement
		Update_Messages update_messages("");
		make_path_movement(world, update_messages);

		return update_messages; // action was used
	}

	// else, idle
}

void Hostile_NPC_Bodyguard::set_protect_target(const string & set_protect_target_id)
{
	this->protect_target_id = set_protect_target_id;
}

bool Hostile_NPC_Bodyguard::attempt_set_new_kill_target(World & world, map<string, shared_ptr<Character>> & actors)
{
	vector<string> hostile_ids;

	// for each visible room
	for (int cx = x - (int)C::VIEW_DISTANCE; cx <= x + (int)C::VIEW_DISTANCE; ++cx)
	{
		for (int cy = y - (int)C::VIEW_DISTANCE; cy <= y + (int)C::VIEW_DISTANCE; ++cy)
		{
			// for each actor in the room
			for (const string & actor_ID : world.room_at(cx, cy, z)->get_actor_ids())
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
	this->kill_target_id = U::random_element_from(hostile_ids);

	// create a pointer to the player
	shared_ptr<Character> kill_target = actors.find(kill_target_id)->second;

	// save the player's current location as the player's last know location (in case they walk out of visible range)
	kill_target_last_known_location._x = kill_target->x;
	kill_target_last_known_location._y = kill_target->y;
	kill_target_last_known_location._z = kill_target->z;

	// we have a target
	return true;
}
bool Hostile_NPC_Bodyguard::attempt_update_kill_target_last_known_location(const shared_ptr<Character> & kill_target)
{
	// if the NPC can see the kill_target
	if (U::diagonal_distance(x, y, kill_target->x, kill_target->y) <= C::VIEW_DISTANCE)
	{
		// update kill_target_last_known_location
		kill_target_last_known_location._x = kill_target->x;
		kill_target_last_known_location._y = kill_target->y;
		kill_target_last_known_location._z = kill_target->z;
		return true;
	}

	// the NPC cannot see the kill target, the last_known_location was not updated
	return false;
}
