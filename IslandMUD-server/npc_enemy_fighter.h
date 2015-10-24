
/* Jim Viebke
Aug 15 2015 */

#ifndef NPC_ENEMY_FIGHTER_H
#define NPC_ENEMY_FIGHTER_H

#include "npc_enemy.h"

class Hostile_NPC_Fighter : public Hostile_NPC
{
public:
	Hostile_NPC_Fighter(const string & name) : Hostile_NPC(name) {}

	void update(World & world, map<string, shared_ptr<Character>> & actors);
};

#endif
