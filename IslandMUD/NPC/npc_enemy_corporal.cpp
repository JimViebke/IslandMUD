
/* Jim Viebke
Jan 11 2016 */

#include "npc_enemy_corporal.h"

Hostile_NPC_Corporal::Hostile_NPC_Corporal(const std::string & name) : Hostile_NPC(name) {}

Update_Messages Hostile_NPC_Corporal::update(World & world, std::map<std::string, std::shared_ptr<Character>> & actors)
{
	return Update_Messages("");
}
