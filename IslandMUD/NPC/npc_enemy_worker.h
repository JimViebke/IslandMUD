
/* Jim Viebke
Aug 15 2015 */

#ifndef NPC_ENEMY_WORKER_H
#define NPC_ENEMY_WORKER_H

#include "npc_enemy.h"

class Hostile_NPC_Worker : public Hostile_NPC
{
public:
	Hostile_NPC_Worker(const std::string & name) : Hostile_NPC(name) {}

	Update_Messages update(World & world, std::map<std::string, std::shared_ptr<Character>> & actors);

	void plan_fortress();

	void plan_fortress_outer_wall(const int & fortress_x, const int & fortress_y, const std::vector<std::vector<bool>> & fortress_footprint);

private:

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

	class Partition
	{
	public:
		int _x, _y, height, width;
		Partition(const int & set_x, const int & set_y, const int & set_height, const int & set_width) :
			_x(set_x), _y(set_y), height(set_height), width(set_width) {}
	};

	typedef Partition Structure;

	/*class Structure
	{
	public:
		int _x, _y, height, width;
		Structure(const int & x, const int & y, const int & height, const int & width) :
			_x(x), _y(y), height(height), width(width) {}
	};*/

	// used during fortress outer wall generation
	enum class Area_Type { structure, fortress_interior, fortress_exterior };
};

#endif
