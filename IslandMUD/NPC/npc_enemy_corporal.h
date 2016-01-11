
/* Jim Viebke
Jan 11 2015 */

#ifndef NPC_ENEMY_CORPORAL_H
#define NPC_ENEMY_CORPORAL_H

#include "npc_enemy.h"

class Hostile_NPC_Corporal : public Hostile_NPC
{
public:
	Hostile_NPC_Corporal(const std::string & name);

	Update_Messages update(World & world, std::map<std::string, std::shared_ptr<Character>> & actors);
};

#endif
