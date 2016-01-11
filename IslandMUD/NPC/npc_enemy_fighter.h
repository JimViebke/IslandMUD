
/* Jim Viebke
Aug 15 2015 */

#ifndef NPC_ENEMY_FIGHTER_H
#define NPC_ENEMY_FIGHTER_H

#include "npc_enemy.h"

class Hostile_NPC_Fighter : public Hostile_NPC
{
public:
	Hostile_NPC_Fighter(const std::string & name) : Hostile_NPC(name) {}

	Update_Messages update(World & world, std::map<std::string, std::shared_ptr<Character>> & actors);
};

#endif
