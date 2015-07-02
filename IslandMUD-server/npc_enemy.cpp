/* Jim Viebke
Jun 3 2015 */

#include "npc_enemy.h"

void Hostile_NPC::update(World & world, map<string, shared_ptr<Character>> & actors)
{
	// every time this is called, the NPC executes one action against the world then returns
	// any number of internal objectives may be created, updated, or deleted

	if (i_dont_have(C::AXE_ID) && !im_planning_to_acquire(C::AXE_ID))
	{
		plan_to_get(C::AXE_ID);
	}

	if (i_dont_have(C::SWORD_ID) && !im_planning_to_acquire(C::SWORD_ID))
	{
		plan_to_get(C::SWORD_ID);
	}

	for (deque<Objective>::iterator objective_iterator = objectives.begin();
		objective_iterator != objectives.end();)
	{

		if (objective_iterator->verb == C::AI_OBJECTIVE_ACQUIRE)
		{
			// if the item is here, take it, remove the current objective, and return
			if (world.room_at(x, y, z)->contains_item(objective_iterator->noun))
			{
				take(objective_iterator->noun, world);

				if (objective_iterator->noun == objective_iterator->purpose)
				{
					// this item is an end goal
					erase_objectives_matching_purpose(objective_iterator->purpose);
				}
				else
				{
					// this item is a means to an end
					erase_objective(objective_iterator);
				}

				return;
			}

			// see if the item is reachable
			for (int cx = x - (int)C::VIEW_DISTANCE; cx <= x + (int)C::VIEW_DISTANCE; ++cx)
			{
				for (int cy = y - (int)C::VIEW_DISTANCE; cy <= y + (int)C::VIEW_DISTANCE; ++cy)
				{
					if (!R::bounds_check(cx, cy)) { continue; } // skip if out of bounds

					if (world.room_at(cx, cy, z)->contains_item(objective_iterator->noun))
					{
						if (pathfind(cx, cy, world))
						{
							return;
						}
					}
				}
			}

			// a path could not be found to the item, plan to craft it if it is craftable and the NPC isn't planning to already

			// if i'm not already planning on crafting the item
			// AND the item is craftable
			if (!objective_iterator->already_planning_to_craft
				&& one_can_craft(objective_iterator->noun))
			{
				objective_iterator->already_planning_to_craft = true;
				plan_to_craft(objective_iterator->noun);
				objective_iterator = objectives.begin(); // obligatory reset
				continue; // jump to next iteration
			}
		}

		++objective_iterator;
	}

	for (deque<Objective>::iterator objective_iterator = objectives.begin();
		objective_iterator != objectives.end(); ++objective_iterator)
	{
		// try to craft the item
		const string craft_attempt = craft(objective_iterator->noun, world);
		if (craft_attempt.find("You now have") != string::npos)
		{
			// if successful, clear completed objectives

			if (objective_iterator->noun == objective_iterator->purpose)
			{
				// this item is an end goal
				erase_objectives_matching_purpose(objective_iterator->purpose);
			}
			else
			{
				// this item is a means to an end
				erase_objective(objective_iterator);
			}

			return;
		}
	}

	// the next block: the NPC runs to the first player it finds IF
	// - it is armed with a sword AND
	// - it has no other objectives
	if (i_have(C::SWORD_ID) && objectives.size() == 0) // hardcoding this bit for now
	{
		// for each row in view distance
		for (int cx = x - (int)C::VIEW_DISTANCE; cx <= x + (int)C::VIEW_DISTANCE; ++cx)
		{
			// for each room in the row in view distance
			for (int cy = y - (int)C::VIEW_DISTANCE; cy <= y + (int)C::VIEW_DISTANCE; ++cy)
			{
				// skip this room if it is out of bounds
				if (!R::bounds_check(cx, cy)) { continue; }

				// for each actor in the room
				for (const string & actor_ID : world.room_at(cx, cy, z)->get_actor_ids())
				{
					// if the character is the type of character we're looking for
					if (R::is<PC>(actors.find(actor_ID)->second))
					{
						// cout << endl << actor_ID << " is a player character at " << cx << "," << cy << ". [exterminate! exterminate!]\n";
						// [target acquired]
						pathfind(cx, cy, world);
						return;
					}
				}
			}
		}
	}

}
