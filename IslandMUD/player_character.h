/* Jim Viebke
Feb 14, 2015 */

#ifndef PLAYER_CHARACTER_H
#define PLAYER_CHARACTER_H

#include "character.h"

class Player_Character;

using PC = Player_Character;

class Player_Character : public Character
{
public:
	Player_Character(const string & name) : Character(name, C::PC_FACTION_ID) {}

	string print() const;

	string get_equipped_item_id() const;

	string generate_area_map(const World & world, const map<string, shared_ptr<Character>> & actors) const;
};

#endif
