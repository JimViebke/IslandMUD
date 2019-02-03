
/* Jim Viebke
Jan 11 2015 */

#ifndef NPC_ENEMY_CORPORAL_H
#define NPC_ENEMY_CORPORAL_H

#include "npc_enemy.h"

class Hostile_NPC_Corporal : public Hostile_NPC
{
public:
	Hostile_NPC_Corporal(const std::string & name, std::observer_ptr<Game> game);

	Update_Messages update();

	// AI subroutines

	void acquire_new_hunt_target();
	Update_Messages wander();
	bool hunt(Update_Messages & update_messages);

private:

	void generate_new_wander_location();

	enum class Stored_Path_Type { to_hunt_target, to_hunt_target_last_known_location, to_wander_target };

	// The NPC selects a random x,y coordinate within the world to wander to.
	// The NPC will stop wandering and engage any Player Characters it sees.
	// The NPC fights to the death. If the Player Character dies, the NPC attempts to
	// pick another target.
	// If no target is found, the NPC will resume wandering
	Stored_Path_Type stored_path_type;
	Coordinate wander_location;
	std::shared_ptr<Coordinate> destination;
	character_id hunt_target_id = -1;
};

#endif
