/* Jim Viebke
Jun 3 2015 */

#include "npc_enemy.h"

void Hostile_NPC::update(World & world, map<string, shared_ptr<Character>> & actors)
{
	// every time this is called, the NPC executes one action against the world
	// any number of internal plans may be created, updated, or deleted

	// cout << "I am " << name << " and I see " << count<PC>(world, actors) << " enemy player(s) and "
	// 	<< count<Hostile_NPC>(world, actors) << " allied hostile(s).\n";

	// the next block: the NPC runs to the first player it finds

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
