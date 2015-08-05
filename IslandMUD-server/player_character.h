/* Jim Viebke
Feb 14, 2015 */

#ifndef PLAYER_CHARACTER_H
#define PLAYER_CHARACTER_H

#include "character.h"

class Player_Character;

typedef Player_Character PC;

class Player_Character : public Character
{
public:
	Player_Character(const string & name, const string & faction_ID) : Character(name, faction_ID) {}

	string print() const;

	string get_equipped_item_id() const;

	string generate_area_map(const World & world, const map<string, shared_ptr<Character>> & actors) const;
};

#endif
