
/* Jim Viebke
Aug 15 2015 */

#include "npc_enemy_fighter.h"

Update_Messages Hostile_NPC_Fighter::update(std::unique_ptr<World> & world, std::map<std::string, std::shared_ptr<Character>> & actors)
{
	if (i_dont_have(C::AXE_ID) && !im_planning_to_acquire(C::AXE_ID))
	{
		plan_to_get(C::AXE_ID);
	}

	if (i_dont_have(C::SWORD_ID) && !im_planning_to_acquire(C::SWORD_ID))
	{
		plan_to_get(C::SWORD_ID);
	}

	// in this block: take the item if it's here, move to the item if it is visible and reachable,
	for (std::deque<Objective>::iterator objective_iterator = objectives.begin();
		objective_iterator != objectives.end();)
	{

		if (objective_iterator->verb == C::AI_OBJECTIVE_ACQUIRE)
		{
			// if the item is here, take it, remove the current objective, and return
			if (world->room_at(location)->contains(objective_iterator->noun))
			{
				Update_Messages update_messages = take(objective_iterator->noun, world);

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
			Update_Messages update_messages("");
			if (pathfind_to_closest_item(objective_iterator->noun, world, update_messages))
			{
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

		// if I am planning on moving to an instance 
		if (objective_iterator->verb == C::AI_OBJECTIVE_GOTO)
		{
			if (one_can_craft(objective_iterator->purpose) && crafting_requirements_met(objective_iterator->purpose, world))
			{
				Update_Messages update_messages("");
				if (pathfind_to_closest_item(objective_iterator->noun, world, update_messages))
				{
					// delete extra objectives here
					// or maybe not; perhaps the objective should be cleared when the item is taken/crafted

					return update_messages;
				}
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
		const Update_Messages update_messages = craft(((objective_iterator->verb == C::AI_OBJECTIVE_GOTO) ? objective_iterator->purpose : objective_iterator->noun), world);
		
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

			return update_messages; // this shouldn't compile
		}
	}

	// the next block: the NPC runs to the first player it finds IF
	// - it has completed crafting the sword and the axe and has completed all other objectives
	if (i_have(C::SWORD_ID) && i_have(C::AXE_ID) && objectives.size() == 0)
	{
		const int x = location.get_x(), y = location.get_y();

		// for each row in view distance
		for (int cx = x - (int)C::VIEW_DISTANCE; cx <= x + (int)C::VIEW_DISTANCE; ++cx)
		{
			// for each room in the row in view distance
			for (int cy = y - (int)C::VIEW_DISTANCE; cy <= y + (int)C::VIEW_DISTANCE; ++cy)
			{
				const Coordinate current(cx, cy);

				// skip this room if it is out of bounds
				if (!current.is_valid()) continue;

				// for each actor in the room
				for (const std::string & actor_ID : world->room_at(current)->get_actor_ids())
				{
					// if the character is a player character
					if (U::is<PC>(actors.find(actor_ID)->second))
					{
						// [target acquired]
						Update_Messages update_messages("");
						pathfind(current, world, update_messages);
						return update_messages;
					}
				}
			}
		}
	}

	return Update_Messages("");
}
