/* Jim Viebke
Jun 3 2015 */

#ifndef NPC_NEUTRAL_H
#define NPC_NEUTRAL_H

#include "non_player_character.h"

class Neutral_NPC : public Non_Player_Character
{
public:
	
	Neutral_NPC(const std::string & name, std::unique_ptr<World> & world) : Non_Player_Character(name, C::NPC_NEUTRAL_FACTION_ID, world) {}
	
	// virtual void update(std::unique_ptr<World> & world, map<string, shared_ptr<Character>> & actors) = 0;
};

#endif
