/* Jim Viebke
Feb 14, 2015

This contains NPC members and functionality that is common for hostile and neutral NPCs. */

#ifndef NON_PLAYER_CHARACTER_H
#define NON_PLAYER_CHARACTER_H

#include <stack>

#include "../character.h"
#include "../player_character.h"

class Non_Player_Character; // forward-declaring...

using NPC = Non_Player_Character; // ...in order to put this here

class Non_Player_Character : public Character
{
public:
	// hostile and neutral NPCs override this in their child classes
	virtual Update_Messages update(std::unique_ptr<World> & world, std::map<character_id, std::shared_ptr<Character>> & actors) = 0;

	// objective debugging
	std::string get_objectives() const;

protected:
	class Objective
	{
	public:
		// "get [] [] axe", "construct north stone surface", "construct north stone door"
		Coordinate objective_location;
		bool modifier, already_planning_to_craft = false;
		std::string verb, material, noun, purpose; // purpose is the reason this objective was added
		C::direction direction;

		Objective(const std::string & verb, const std::string & noun, const std::string & purpose);
		Objective(const std::string & verb, const std::string & noun, const Coordinate & objective_location);
		Objective(const std::string & verb, const std::string & noun, const std::string & material, const C::direction & direction, const Coordinate & objective_location, const bool & modifier);
	};

	enum class Objective_Priority { low_priority, high_priority };

	std::deque<Objective> objectives;
	std::deque<Coordinate> path;

	// this can only be instantiated by its children, hostile and neutral. No NPC of this type "NPC" exists or should be instantiated
	Non_Player_Character(const std::string & name, const std::string & faction_ID, std::unique_ptr<World> & world);

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
	bool crafting_requirements_met(const std::string & item_ID, const std::unique_ptr<World> & world) const;

	// objective planning
	void plan_to_get(const std::string & item_id);
	void plan_to_craft(const std::string & item_id);

	// used to count friends or foes:   count<Enemy_NPC>(world, actors);
	template <typename ACTOR_TYPE> unsigned count(std::unique_ptr<World> & world, std::map<std::string, std::shared_ptr<Character>> & actors) const
	{
		unsigned players_in_range = 0;

        const int x = location.get_x(), y = location.get_y();

		// for each row of rooms in view distance
		for (int cx = x - (int)C::VIEW_DISTANCE; cx <= x + (int)C::VIEW_DISTANCE; ++cx)
		{
			// for each room in the row in view distance
			for (int cy = y - (int)C::VIEW_DISTANCE; cy <= y + (int)C::VIEW_DISTANCE; ++cy)
			{
                Coordinate current(cx, cy);

				// skip this room if it is out of bounds
				if (!current.is_valid()) continue;

				// for each actor in the room
				for (const std::string & actor_ID : world.room_at(current)->get_actor_ids())
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
	bool pathfind(const Coordinate & destination, std::unique_ptr<World> & world, Update_Messages & update_messages);
	bool best_attempt_pathfind(const Coordinate & destination, std::unique_ptr<World> & world, Update_Messages & update_messages);
	bool pathfind_to_closest_item(const std::string & item_id, std::unique_ptr<World> & world, Update_Messages & update_messages);
	bool save_path_to(const Coordinate & destination, std::unique_ptr<World> & world);
	bool make_path_movement(std::unique_ptr<World> & world, Update_Messages & update_messages);

	// other pathfinding utilities
	int diagonal_movement_cost(const Coordinate & coord_1, const Coordinate & coord_2);
	bool coordinates_are_on_path(const Coordinate & find_coordinate) const;

private:

	/* F = G + H

	G : actual cost to reach a certain room
	H : estimated cost to reach destination from a certain room
	F-cost = G + H */

	class Node
	{
	public:
		Coordinate location, parent_location;
		int h = 0, g = 0, f = 0;
		C::direction direction_from_parent;

		Node();
		Node(const Coordinate & set_location, const C::direction & set_direction);

		void set_g_h_f(const int & set_g, const int & set_h);
		void set_g(const int & set_g);
	};

	// pathfinding node utilities
	Node move_and_get_lowest_f_cost(std::vector<Node> & open, std::vector<Node> & closed);
	Node move_and_get_lowest_g_cost(std::vector<Node> & open, std::vector<Node> & closed);
	bool room_in_node_list(const Coordinate & find_coord, const std::vector<Node> & node_list) const;
	Node get_node_at(const Coordinate & find_coordinate, const std::vector<Node> & node_list) const;
};

#endif
