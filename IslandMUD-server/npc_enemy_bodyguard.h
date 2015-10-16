
/* Jim Viebke
Aug 15 2015 */

#ifndef NPC_ENEMY_BODYGUARD_H
#define NPC_ENEMY_BODYGUARD_H

#include "npc_enemy.h"

class Hostile_NPC_Bodyguard : public Hostile_NPC
{
public:
	Hostile_NPC_Bodyguard(const string & name) : Hostile_NPC(name) {}

	void update(World & world, map<string, shared_ptr<Character>> & actors);
};

#endif
