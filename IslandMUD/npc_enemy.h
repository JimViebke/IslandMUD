
/* Jim Viebke
Jun 3 2015 */

#ifndef NPC_ENEMY_H
#define NPC_ENEMY_H

#include "non_player_character.h"

class Hostile_NPC : public Non_Player_Character
{
protected:
	Hostile_NPC(const string & name) : Non_Player_Character(name, C::NPC_HOSTILE_FACTION_ID) {}

	virtual Update_Messages update(World & world, map<string, shared_ptr<Character>> & actors) = 0;
};

#endif
