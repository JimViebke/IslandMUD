
/* Jim Viebke
Jun 3 2015 */

#ifndef NPC_ENEMY_H
#define NPC_ENEMY_H

#include "non_player_character.h"

class Hostile_NPC : public Non_Player_Character
{
protected:
	Hostile_NPC(const std::string & name, std::observer_ptr<Game> game);

	virtual Update_Messages update() = 0;

	character_id get_new_hostile_id() const;

	class Structure
	{
	public:
		int _x, _y, height, width;
		Structure(const int & set_x, const int & set_y, const int & set_height, const int & set_width) :
			_x(set_x), _y(set_y), height(set_height), width(set_width) {}
	};

	// The structure class also serves as partitions during fortress planning
	typedef Structure Partition;

	class Fortress
	{
	private:
		std::map<std::string, Structure> buildings;
	};
};

#endif
