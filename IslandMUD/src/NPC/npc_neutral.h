/* Jim Viebke
Jun 3 2015 */

#ifndef NPC_NEUTRAL_H
#define NPC_NEUTRAL_H

#include "non_player_character.h"

class Neutral_NPC : public Non_Player_Character
{
public:
	Neutral_NPC(const std::string & name, std::observer_ptr<Game> game) : Non_Player_Character(name, C::NPC_NEUTRAL_FACTION_ID, game) {}
};

#endif
