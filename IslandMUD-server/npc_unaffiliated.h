/* Jim Viebke
Jun 3 2015 */

#ifndef NPC_UNAFFILIATED_H
#define NPC_UNAFFILIATED_H

#include "non_player_character.h"

class Neutral_NPC : public Non_Player_Character
{
public:
	
	Neutral_NPC(const string & name, const string & faction_ID, const string & ai_type) : Non_Player_Character(name, faction_ID, ai_type) {}
	
	// virtual void update(World & world, map<string, shared_ptr<Character>> & actors) = 0;
};

#endif
