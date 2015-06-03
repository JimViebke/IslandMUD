/* Jim Viebke
Jun 3 2015 */

#ifndef NPC_ENEMY_H
#define NPC_ENEMY_H

#include "non_player_character.h"

class NPC_Enemy;

typedef NPC_Enemy Hostile_NPC;

class NPC_Enemy : public Non_Player_Character
{
public:
	NPC_Enemy(const string & name, const string & faction_ID) : Non_Player_Character(name, faction_ID) {}

	void update(World & world, map<string, shared_ptr<Character>> & actors);

};

#endif
