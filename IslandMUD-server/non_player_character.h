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
	virtual void update(World & world, map<string, shared_ptr<Character>> & actors) = 0;

protected:
	stack<string> objectives; // structure highly subject to change

	// this can only be instantiaed by its children, hostile and neutral. No NPC of this type "NPC" exists or should be instantiated
	Non_Player_Character(const string & name, const string & faction_ID) : Character(name, faction_ID) {}

	// used to count friends & foes:   count<Enemy_NPC>(world, actors);
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

	void a_star_pathfind(const int & x_dest, const int & y_dest, World & world);

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

		void set_g_h_f(const int & set_g, const int & set_h);
	};

	// pathfinding node utilities
	Node move_and_get_lowest_f_cost(vector<Node> & open, vector<Node> & closed);
	bool room_in_node_list(const int & find_x, const int & find_y, const vector<Node> & node_list) const;
	Node get_node_at(const int & find_x, const int & find_y, const vector<Node> & node_list) const;
};

#endif
