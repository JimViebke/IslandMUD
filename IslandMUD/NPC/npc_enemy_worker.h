
/* Jim Viebke
Aug 15 2015 */

#ifndef NPC_ENEMY_WORKER_H
#define NPC_ENEMY_WORKER_H

#include "npc_enemy.h"

class Hostile_NPC_Worker : public Hostile_NPC
{
public:
	Hostile_NPC_Worker(const std::string & name, World & world) : Hostile_NPC(name, world) {}

	Update_Messages update(World & world, std::map<std::string, std::shared_ptr<Character>> & actors);

	void plan_fortress();

	void plan_fortress_outer_wall(const int & fortress_x, const int & fortress_y, const std::vector<std::vector<bool>> & fortress_footprint);

private:

	// Contains a vector of Objectives. All objectives belong to the same structure
	class Structure_Objectives
	{
	private:
		bool doors_planned = false;

	public:
		std::vector<Objective> structure_surface_objectives;
		void add(const Objective & obj);
		void plan_doors(const World & world);
		bool already_planned_doors() const;
	};

	bool fortress_planned = false;
	std::deque<Structure_Objectives> planned_structures;

	// used during fortress outer wall generation
	enum class Area_Type { structure, fortress_interior, fortress_exterior };
};

#endif
