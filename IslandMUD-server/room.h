/* Jim Viebke
Feb 14, 2015 */

#ifndef ROOM_H
#define ROOM_H

#include <map> // for room contents multimap
#include <set> // playerlist

#include "resources.h"
#include "constants.h"
#include "item.h"
#include "room_side.h" // walls, floor, or ceiling


class Room
{
private:
	bool updated = false; // has the room been updated since it was loaded?
	map<string, Room_Side> room_sides = {}; // the floor, walls, and ceiling in the room (no key for absent surfaces)
	multimap<string, shared_ptr<Item>> contents = {}; // the items in a room
	vector<string> viewing_actor_ids = {}; // the PCs and NPCs who can see this room
	vector<string> actor_ids = {}; // the PCs and NPCs in a room
public:

	Room() {}

	// room information
	const multimap<string, shared_ptr<Item>> get_contents() const
	{
		return contents;
	}
	const map<string, Room_Side> get_room_sides() const
	{
		return room_sides;
	}
	bool has_wall() const // used to determine if a ceiling can be placed
	{
		// first do an easy check to see if there are any surfaces
		if (room_sides.size() == 0)
		{
			return false;
		}

		// for each surface
		for (map<string, Room_Side>::const_iterator it = room_sides.begin();
			it != room_sides.cend(); ++it)
		{
			// if the surface is a wall
			if (it->first == C::NORTH || it->first == C::EAST ||
				it->first == C::SOUTH || it->first == C::WEST)
			{
				return true;
			}
		}
		// the room does not have a wall
		return false;
	}
	bool has_surface(const string & direction_id) const
	{
		return room_sides.find(direction_id) != room_sides.cend();
	}
	bool contains_no_items() const
	{
		return contents.size() == 0;
	}
	bool is_unloadable() const
	{
		return actor_ids.size() == 0 && viewing_actor_ids.size() == 0;
	}
	bool contains_item(const string & item_id) const
	{
		return contents.find(item_id) != contents.cend();
	}
	bool contains_item(const string & item_id, const unsigned & count) const
	{
		// test if a room has a specified quantity for crafting

		if (count == 1) // if only one item is required
		{
			return contains_item(item_id); // defer
		}

		// more than one is required
		if (contents.count(item_id) >= count) // if there are a sufficient number of matching items
		{
			return true; // return success
		}

		// for each item in the room
		for (multimap<string, shared_ptr<Item>>::const_iterator it = contents.cbegin();
			it != contents.cend(); ++it)
		{
			// if the item is a material
			if (shared_ptr<Material> material_item = R::convert_to<Material>(it->second))
			{
				// if the amount greater than or equal to count
				if (material_item->amount >= count)
				{
					// success
					return true;
				}
			}
		}

		// there is not a sufficient count of items in the room
		return false;
	}

	// add and remove items
	void add_item(const shared_ptr<Item> item) // pass a copy rather than a reference
	{
		/* This doesn't stack materials.
		This could easily be fixed, but room saving/unloading doesn't use item counts.
		Alternatively, treat the symptoms and just fix the printout
		*/

		contents.insert(pair<string, shared_ptr<Item>>(item->name, item));
		updated = true;
	}
	void remove_item(const string & item_id, const int & count = 1)
	{
		for (int i = 0; i < count; ++i) // for as many items as are to be removed
		{
			contents.erase(contents.find(item_id)); // remove the item
		}

		updated = true;
	}

	// add surfaces
	void add_surface(const string & surface_ID, const string & material_ID)
	{
		// if the surface ID is valid
		if (R::contains(R::surface_ids, surface_ID))
		{
			// create a new Room_Side and add it to room_sides
			room_sides.insert(pair<string, Room_Side>(surface_ID, Room_Side(material_ID)));
			updated = true;
		}
	}
	void add_surface(const string & surface_ID, const string & material_ID, const int & surface_health)
	{
		// create a surface with a given health (used for loading rooms from disk that may be damaged)

		// if the surface ID is valid
		if (R::contains(R::surface_ids, surface_ID))
		{
			// create a new Room_Side and add it to room_sides
			room_sides.insert(pair<string, Room_Side>(surface_ID, Room_Side(material_ID)));

			// select the surface and set its health to the passed value
			room_sides.find(surface_ID)->second.set_health(surface_health);

			updated = true;
		}
	}

	// damage surface
	string damage_surface(const string & surface_ID)
	{
		if (!this->has_surface(surface_ID))
		{
			if (surface_ID == C::UP)
			{
				return "There is no ceiling above you that you can damage.";
			}
			else if (surface_ID == C::DOWN)
			{
				return "There is no floor below you to damage.";
			}
			else
			{
				return "There is no wall to your " + surface_ID + " to damage.";
			}
		}

		// surface exists, inflict damage
		this->room_sides.find(surface_ID)->second.change_health(-1); // hard coded until we have damage tables

		// if the surface has 0 health
		if (room_sides.find(surface_ID)->second.is_rubble())
		{
			// remove the surface
			room_sides.erase(surface_ID);

			// the surface collapses
			return (surface_ID == C::CEILING || surface_ID == C::FLOOR) ?
				"The " + surface_ID + " collapses." :
				"The wall collapses.";
		}
		else
		{
			// the surface holds
			return "You damage the " +
				((surface_ID == C::CEILING || surface_ID == C::FLOOR) ? surface_ID : "wall")
				+ ".";
		}
	}

	// add and remove actors
	void add_actor(const string & actor_id)
	{
		if (!R::contains(actor_ids, actor_id)) // if the actor is not already in the list of actors
		{
			actor_ids.push_back(actor_id); // add the actor
		}
	}
	void remove_actor(const string & actor_id)
	{
		if (R::contains(actor_ids, actor_id)) // if the character exists here
		{
			R::erase_element_from_vector(actor_ids, actor_id);
		}
	}
	void add_viewing_actor(const string & actor_id)
	{
		// if the passed actor_ID is not already able to view the room
		if (!R::contains(viewing_actor_ids, actor_id))
		{
			// add the actor ID to viewing_actor_ids
			viewing_actor_ids.push_back(actor_id);
		}
	}
	void remove_viewing_actor(const string & actor_id)
	{
		if (R::contains(viewing_actor_ids, actor_id)) // if the character can see this room
		{
			R::erase_element_from_vector(viewing_actor_ids, actor_id); // remove the character
		}
	}

	// printing
	string summary() const
	{
		stringstream summary_stream;

		// report on the sides of the room (walls, ceiling, floor)
		if (room_sides.size() > 0)
		{
			summary_stream << "\nThis room consists of";

			// create a pointer pointing to the last side in the room
			map<string, Room_Side>::const_iterator last_side_it = room_sides.cend(); // one past the actual last item
			--last_side_it; // the last item

			// for each room side
			for (map<string, Room_Side>::const_iterator it = room_sides.cbegin();
				it != room_sides.cend(); ++it)
			{
				// if there are more than one sides, append " and"
				if (it == last_side_it && room_sides.size() > 1) // "and" precedes last surface
				{
					summary_stream << " and";
				}

				summary_stream << " a(n) " << it->second.material_id // a stone
					<< ((it->first == C::CEILING || it->first == C::FLOOR) ? " " : " wall to the ") // conditionally " wall to the " 
					<< it->first; // direction ID

				// if the current surface is not the last AND there are more than 2 sides, append a comma
				if (it != last_side_it && room_sides.size() > 2)
				{
					summary_stream << ",";
				}
			}

			summary_stream << "." << endl; // always end with a period
		}

		if (contents.size() > 0) // if there are items present
		{
			summary_stream << "\nYou look around and notice ";
			// for each item
			for (multimap<string, shared_ptr<Item>>::const_iterator it = contents.cbegin();
				it != contents.cend(); ++it)
			{
				// append the id (?) of the item
				summary_stream << it->first << " ";
			}
		}
		else // empty room
		{
			summary_stream << "\nThere is nothing of interest here.";
		}

		return summary_stream.str();
	}

};

#endif
