
/* Jim Viebke
Jun 3 2015 */

#ifndef NPC_ENEMY_H
#define NPC_ENEMY_H

#include "non_player_character.h"

class Hostile_NPC : public Non_Player_Character
{
protected:
	Hostile_NPC(const string & name, const string & faction_ID) : Non_Player_Character(name, faction_ID) {}

	virtual void update(World & world, map<string, shared_ptr<Character>> & actors) = 0;
};

#endif
