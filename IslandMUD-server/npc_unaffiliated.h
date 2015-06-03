/* Jim Viebke
Jun 3 2015 */

#ifndef NPC_UNAFFILIATED_H
#define NPC_UNAFFILIATED_H

#include "non_player_character.h"

class NPC_Unaffiliated;

typedef NPC_Unaffiliated Neutral_NPC;

class NPC_Unaffiliated : public Non_Player_Character
{
public:
	
	NPC_Unaffiliated(const string & name, const string & faction_ID) : Non_Player_Character(name, faction_ID) {}
	
	void update(World & world, map<string, shared_ptr<Character>> & actors);
};

#endif
