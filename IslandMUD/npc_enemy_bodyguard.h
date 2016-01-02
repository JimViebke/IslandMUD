
/* Jim Viebke
Aug 15 2015 */

#ifndef NPC_ENEMY_BODYGUARD_H
#define NPC_ENEMY_BODYGUARD_H

#include "npc_enemy.h"

class Hostile_NPC_Bodyguard : public Hostile_NPC
{
public:
	Hostile_NPC_Bodyguard(const std::string & name, const std::string & set_protect_target_id) : Hostile_NPC(name),
		protect_target_id(set_protect_target_id), kill_target_last_known_location(-1, -1), guard_radius(5) {}

	Update_Messages update(World & world, std::map<std::string, std::shared_ptr<Character>> & actors);

	void set_protect_target(const std::string & set_protect_target_id);

private:
	enum class Stored_Path_Type { to_kill_target, to_protect_target };

	// NPC bodyguards will stay within guard_radius rooms of the NPC they are protecting
	int guard_radius; // measured in diagonal/Chebyshev distance, not Euclidian distance
	Stored_Path_Type stored_path_type;
	Coordinate kill_target_last_known_location;
	std::string kill_target_id = "", protect_target_id = "";

	// returns a boolean indicating if a new target was found and set
	bool attempt_set_new_kill_target(World & world, std::map<std::string, std::shared_ptr<Character>> & actors);
	// returns a boolean indicating if the kill target's last known location was updated
	bool attempt_update_kill_target_last_known_location(const std::shared_ptr<Character> & kill_target);

	// AI subroutines

	Update_Messages hunt_target(std::shared_ptr<Character> & kill_target, std::shared_ptr<Character> & protect_target,
		World & world, std::map<std::string, std::shared_ptr<Character>> & actors);

	Update_Messages move_toward_protect_target(std::shared_ptr<Character> & protect_target,
		World & world, std::map<std::string, std::shared_ptr<Character>> & actors);

	Update_Messages approach_new_kill_target(World & world);
};

#endif
