
/* Jim Viebke
Aug 15 2015 */

#ifndef NPC_ENEMY_WORKER_H
#define NPC_ENEMY_WORKER_H

#include "npc_enemy.h"

class Hostile_NPC_Worker : public Hostile_NPC
{
private:
	bool fortress_planned = false;

public:
	Hostile_NPC_Worker(const string & name, const string & faction_ID) :
		Hostile_NPC(name, faction_ID, C::AI_TYPE_WORKER) {}

	void update(World & world, map<string, shared_ptr<Character>> & actors);

	void plan_fortress();

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
};

#endif
