
/* Jim Viebke
Aug 15 2015 */

#ifndef NPC_ENEMY_FIGHTER_H
#define NPC_ENEMY_FIGHTER_H

#include "npc_enemy.h"

class Hostile_NPC_Fighter : public Hostile_NPC
{
public:
	Hostile_NPC_Fighter(const std::string & name, std::unique_ptr<World> & world) : Hostile_NPC(name, world) {}

	Update_Messages update(std::unique_ptr<World> & world, std::map<character_id, std::shared_ptr<Character>> & actors);
};

#endif
