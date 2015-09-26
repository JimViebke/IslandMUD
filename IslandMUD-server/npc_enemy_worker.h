
/* Jim Viebke
Aug 15 2015 */

#ifndef NPC_ENEMY_WORKER_H
#define NPC_ENEMY_WORKER_H

#include "npc_enemy.h"

class Hostile_NPC_Worker : public Hostile_NPC
{
private:

	class Structure_Objectives
	{
	private:
		bool doors_planned = false;

	public:
		vector<Objective> structure_surface_objectives;
		void add(const Objective & obj);
		void plan_doors(const World & world);
		bool already_planned_doors() const;
	};

	bool fortress_planned = false;
	deque<Structure_Objectives> planned_structures;

public:
	Hostile_NPC_Worker(const string & name, const string & faction_ID) :
		Hostile_NPC(name, faction_ID) {}

	void update(World & world, map<string, shared_ptr<Character>> & actors);

	void plan_fortress();

	void plan_fortress_outer_wall(const int & fortress_x, const int & fortress_y, const vector<vector<bool>> & fortress_footprint);

private:
	class Partition
	{
	public:
		int x, y, height, width;
		Partition(const int & x, const int & y, const int & height, const int & width) :
			x(x), y(y), height(height), width(width) {}
	};

	class Structure
	{
	public:
		int x, y, height, width;
		Structure(const int & x, const int & y, const int & height, const int & width) :
			x(x), y(y), height(height), width(width) {}
	};

	// used during fortress outer wall generation
	enum Area_Type { structure, fortress_interior, fortress_exterior };
};

#endif
