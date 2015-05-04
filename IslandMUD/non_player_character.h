/* Jim Viebke
Feb 14, 2015 */

#ifndef NON_PLAYER_CHARACTER_H
#define NON_PLAYER_CHARACTER_H

#include <stack>

#include "character.h"

class Non_Player_Character : public Character
{
public:
	stack<string> objectives; // structure highly subject to change

	Non_Player_Character() : Character()
	{

	}
};

typedef Non_Player_Character NPC;

#endif
