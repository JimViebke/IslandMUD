/* Jim Viebke
Feb 14, 2015 */

#ifndef PLAYER_CHARACTER_H
#define PLAYER_CHARACTER_H

#include "character.h"

class Player_Character : public Character
{
public:
	Player_Character(const string & name) : Character(name) {}

	string print() const;
};

typedef Player_Character PC;

#endif
