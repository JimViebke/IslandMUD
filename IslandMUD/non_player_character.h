/* Jim Viebke
Feb 14, 2015

This contains NPC members and functionality that is common for hostile and neutral NPCs. */

#ifndef NON_PLAYER_CHARACTER_H
#define NON_PLAYER_CHARACTER_H

#include <stack>

#include "character.h"
#include "player_character.h"

class Non_Player_Character; // forward-declaring...

using NPC = Non_Player_Character; // ...in order to put this here

class Non_Player_Character : public Character
{
public:
	// hostile and neutral NPCs override this in their child classes
	virtual Update_Messages update(World & world, std::map<std::string, std::shared_ptr<Character>> & actors) = 0;

	// objective debugging
	std::string get_objectives() const;

protected:
	class Objective
	{
	public:
		// "get [] [] axe", "construct north stone surface", "construct north stone door"
		int objective_x, objective_y, objective_z;
		bool modifier, already_planning_to_craft = false;
		std::string verb, direction, material, noun, purpose; // purpose is the reason this objective was added

		Objective(const std::string & verb, const std::string & noun, const std::string & purpose);
		Objective(const std::string & verb, const std::string & noun, const int & objective_x, const int & objective_y, const int & objective_z);
		Objective(const std::string & verb, const std::string & noun, const std::string & material, const std::string & direction, const int & objective_x, const int & objective_y, const int & objective_z, const bool & modifier);
	};

	enum class Objective_Priority { low_priority, high_priority };

	class Coordinate
	{
	public:
		int _x, _y, _z;
		Coordinate(const int & set_x, const int & set_y, const int & set_z = -1);
		void reset() { _x = _y = _z = -1; }
	};

	std::deque<Objective> objectives;
	std::deque<Coordinate> path;

	// this can only be instantiated by its children, hostile and neutral. No NPC of this type "NPC" exists or should be instantiated
	Non_Player_Character(const std::string & name, const std::string & faction_ID);

	// objective creating and deletion
	void add_objective(const Objective_Priority & priority, const std::string & verb, const std::string & noun, const std::string & purpose);
	void add_objective(const Objective_Priority & priority, const std::string & verb, const std::string & noun, const int & objective_x, const int & objective_y, const int & objective_z);
	void erase_objective(const std::deque<Objective>::iterator & objective_iterator);
	void erase_objectives_matching_purpose(const std::string purpose);
	void erase_goto_objective_matching(const std::string & purpose);
	void erase_acquire_objective_matching(const std::string & noun);

	// objective information
	bool one_can_craft(const std::string & item_id) const;
	bool i_have(const std::string & item_id) const;
	bool i_dont_have(const std::string & item_id) const;
	bool im_planning_to_acquire(const std::string & item_ID) const;
	bool crafting_requirements_met(const std::string & item_ID, const World & world) const;

	// objective planning
	void plan_to_get(const std::string & item_id);
	void plan_to_craft(const std::string & item_id);

	// used to count friends or foes:   count<Enemy_NPC>(world, actors);
	template <typename ACTOR_TYPE> unsigned count(World & world, std::map<std::string, std::shared_ptr<Character>> & actors) const
	{
		unsigned players_in_range = 0;

		// for each row of rooms in view distance
		for (int cx = x - (int)C::VIEW_DISTANCE; cx <= x + (int)C::VIEW_DISTANCE; ++cx)
		{
			// for each room in the row in view distance
			for (int cy = y - (int)C::VIEW_DISTANCE; cy <= y + (int)C::VIEW_DISTANCE; ++cy)
			{
				// skip this room if it is out of bounds
				if (!U::bounds_check(cx, cy)) { continue; }

				// for each actor in the room
				for (const std::string & actor_ID : world.room_at(cx, cy, z)->get_actor_ids())
				{
					// if the character is the type of character we're looking for
					if (U::is<ACTOR_TYPE>(actors.find(actor_ID)->second))
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
	bool pathfind(const int & x_dest, const int & y_dest, World & world, Update_Messages & update_messages);
	bool pathfind_to_closest_item(const std::string & item_id, World & world, Update_Messages & update_messages);
	bool save_path_to(const int & x_dest, const int & y_dest, World & world);
	bool make_path_movement(World & world, Update_Messages & update_messages);

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
		std::string direction_from_parent;

		Node();
		Node(const int & set_x, const int & set_y, const std::string & dir);

		void set_g_h_f(const int & set_g, const int & set_h);
		void set_g(const int & set_g);
	};

	// pathfinding node utilities
	Node move_and_get_lowest_f_cost(std::vector<Node> & open, std::vector<Node> & closed);
	Node move_and_get_lowest_g_cost(std::vector<Node> & open, std::vector<Node> & closed);
	bool room_in_node_list(const int & find_x, const int & find_y, const std::vector<Node> & node_list) const;
	Node get_node_at(const int & find_x, const int & find_y, const std::vector<Node> & node_list) const;
};

#endif
