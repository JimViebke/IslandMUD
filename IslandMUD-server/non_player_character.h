/* Jim Viebke
Feb 14, 2015

This contains NPC members and functionality that is common for hostile and neutral NPCs. */

#ifndef NON_PLAYER_CHARACTER_H
#define NON_PLAYER_CHARACTER_H

#include <stack>

#include "character.h"
#include "player_character.h"

class Non_Player_Character; // forward-declaring...

typedef Non_Player_Character NPC; // ...in order to put this here

class Non_Player_Character : public Character
{
public:
	// hostile and neutral NPCs override this in their child classes
	virtual void update(World & world, map<string, shared_ptr<Character>> & actors) = 0;

	// objective debugging
	string get_objectives() const;

protected:
	class Objective
	{
	public:
		// "get [] [] axe", "construct north stone surface", "construct north stone door"
		int objective_x, objective_y, objective_z;
		bool modifier, already_planning_to_craft = false;
		string verb, direction, material, noun, purpose; // "sword" (the reason this objective was added

		Objective(const string & verb, const string & noun, const string & purpose) :
			verb(verb), noun(noun), purpose(purpose) {}
		Objective(const string & verb, const string & noun, const int & objective_x, const int & objective_y, const int & objective_z) :
			verb(verb), noun(noun), objective_x(objective_x), objective_y(objective_y), objective_z(objective_z) {}
		Objective(const string & verb, const string & noun, const string & material, const string & direction, const int & objective_x, const int & objective_y, const int & objective_z, const bool & modifier) :
			verb(verb), noun(noun), material(material), direction(direction), objective_x(objective_x), objective_y(objective_y), objective_z(objective_z), modifier(modifier) {}
	};

	enum Objective_Priority { low_priority, high_priority };

	string ai_type;
	deque<Objective> objectives;

	// this can only be instantiated by its children, hostile and neutral. No NPC of this type "NPC" exists or should be instantiated
	Non_Player_Character(const string & name, const string & faction_ID, const string & set_ai_type) : Character(name, faction_ID)
	{
		if (set_ai_type == C::AI_TYPE_BLACKSMITH ||
			set_ai_type == C::AI_TYPE_FIGHTER ||
			set_ai_type == C::AI_TYPE_MINER ||
			set_ai_type == C::AI_TYPE_PATROL_GUARD ||
			set_ai_type == C::AI_TYPE_WATCH_GUARD ||
			set_ai_type == C::AI_TYPE_WORKER)
		{
			this->ai_type = set_ai_type;
		}
		else
		{
			cout << "\nERROR: [" << set_ai_type << "] is not a known AI type.\n";
		}
	}

	// objective creating and deletion
	void add_objective(const Objective_Priority & priority, const string & verb, const string & noun, const string & purpose);
	void add_objective(const Objective_Priority & priority, const string & verb, const string & noun, const int & objective_x, const int & objective_y, const int & objective_z);
	void erase_objective(const deque<Objective>::iterator & objective_iterator);
	void erase_objectives_matching_purpose(const string purpose);
	void erase_goto_objective_matching(const string & purpose);
	void erase_acquire_objective_matching(const string & noun);

	// objective information
	bool one_can_craft(const string & item_id) const;
	bool i_have(const string & item_id) const;
	bool i_dont_have(const string & item_id) const;
	bool im_planning_to_acquire(const string & item_ID) const;
	bool i_have_all_ingredients_to_craft(const string & item_ID) const;

	// objective planning
	void plan_to_get(const string & item_id);
	void plan_to_craft(const string & item_id);

	// used to count friends or foes:   count<Enemy_NPC>(world, actors);
	template <typename ACTOR_TYPE> unsigned count(World & world, map<string, shared_ptr<Character>> & actors) const
	{
		unsigned players_in_range = 0;

		// for each row of rooms in view distance
		for (int cx = x - (int)C::VIEW_DISTANCE; cx <= x + (int)C::VIEW_DISTANCE; ++cx)
		{
			// for each room in the row in view distance
			for (int cy = y - (int)C::VIEW_DISTANCE; cy <= y + (int)C::VIEW_DISTANCE; ++cy)
			{
				// skip this room if it is out of bounds
				if (!R::bounds_check(cx, cy)) { continue; }

				// for each actor in the room
				for (const string & actor_ID : world.room_at(cx, cy, z)->get_actor_ids())
				{
					// if the character is the type of character we're looking for
					if (R::is<ACTOR_TYPE>(actors.find(actor_ID)->second))
					{
						// count one more
						++players_in_range;
					}
				}
			}
		}

		// relay the count
		return players_in_range;
	}

	// returns true if successful
	bool pathfind(const int & x_dest, const int & y_dest, World & world);
	bool pathfind_to_closest_item(const string & item_id, World & world);

private:

	/* F = G + H

	G : actual cost to reach a certain room
	H : estimated cost to reach destination from a certain room
	F-cost = G + H */

	class Node
	{
	public:
		int x = -1, y = -1, z = -1,
			parent_x = -1, parent_y = -1,
			h = 0, g = 0, f = 0;
		string direction_from_parent;

		Node() {}
		Node(const int & x, const int & y, const string & dir) : x(x), y(y), direction_from_parent(dir) {}

		// Node member setter
		void set_g_h_f(const int & set_g, const int & set_h);
		void set_g(const int & set_g);
	};

	// pathfinding node utilities
	Node move_and_get_lowest_f_cost(vector<Node> & open, vector<Node> & closed);
	Node move_and_get_lowest_g_cost(vector<Node> & open, vector<Node> & closed);
	bool room_in_node_list(const int & find_x, const int & find_y, const vector<Node> & node_list) const;
	Node get_node_at(const int & find_x, const int & find_y, const vector<Node> & node_list) const;
};

#endif
