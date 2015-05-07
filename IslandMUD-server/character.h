/* Jim Viebke
Feb 14, 2015 */

#ifndef CHARACTER_H
#define CHARACTER_H

#include <map> // for inventory multimap

#include "item.h"
#include "craft.h"
#include "room.h"
#include "world.h"

using namespace std;

class Character
{
public:
	string name;
	__int64 last_action_timestamp;
	string leader_ID;
	vector<string> follower_ids;

	multimap<string, shared_ptr<Equipment>> equipment_inventory; // equipment doesn't stack
	map<string, shared_ptr<Material>> material_inventory; // materials stack

	// Item dragging_item; // a character can drag an item if they don't want to carry it.
	static Recipes recipes; // exists in memory once for all PCs and NPCs

	int x = C::DEFAULT_SPAWN_X; // location coordinates
	int y = C::DEFAULT_SPAWN_Y;
	int z = C::DEFAULT_SPAWN_Z;

	Character(const string & name) : name(name) {}
	virtual ~Character() {} // to make a polymorphic type

	string login(World & world)
	{
		// create a document to load the player's data
		xml_document user_data_xml;

		// load the player's data to user_data_xml
		user_data_xml.load_file((C::user_data_directory + "\\" + this->name + ".xml").c_str());

		// create holder values to save the coordinates from the file
		int load_x = -1, load_y = -1, load_z = -1;

		// load the three values from the node
		xml_node location_node = user_data_xml.child(C::XML_USER_LOCATION.c_str());

		// extract the attributes as well as the values for the attributes
		xml_attribute x_attribute = location_node.attribute(string("x").c_str());
		xml_attribute y_attribute = location_node.attribute(string("y").c_str());
		xml_attribute z_attribute = location_node.attribute(string("z").c_str());
		load_x = x_attribute.as_int();
		load_y = y_attribute.as_int();
		load_z = z_attribute.as_int();

		// if any of the attributes are empty or the extracted values fail bounds-checking
		if (x_attribute.empty() || y_attribute.empty() || z_attribute.empty() ||
			!R::bounds_check(load_x, load_y, load_z))
		{
			// set the player to the default spawn
			this->x = C::DEFAULT_SPAWN_X;
			this->y = C::DEFAULT_SPAWN_Y;
			this->z = C::DEFAULT_SPAWN_Z;
			world.load_view_radius_around(x, y, name);
			world.room_at(x, y, z)->add_actor(this->name);
		}
		else // valid coordinates were loaded
		{
			this->x = load_x;
			this->y = load_y;
			this->z = load_z;
			world.load_view_radius_around(x, y, name);
			world.room_at(x, y, z)->add_actor(this->name);
		}

		// for each item node of the equipment node
		for (const xml_node & equipment : user_data_xml.child(C::XML_USER_EQUIPMENT.c_str()).children())
		{
			// use the name of the node to create an equipment object and add it to the player's equipment inventory
			equipment_inventory.insert(pair<string, shared_ptr<Equipment>>(
				equipment.name(),
				R::convert_to<Equipment>(Craft::make(equipment.name()))
				));
		}

		// for each item in the material node
		for (const xml_node & material : user_data_xml.child(C::XML_USER_MATERIALS.c_str()).children())
		{
			// use the name of the material node to create a new materail object
			shared_ptr<Material> item = R::convert_to<Material>(Craft::make(material.name()));

			// extract the amount from the item's attribute
			item->amount = material.attribute(C::XML_USER_MATERIAL_COUNT.c_str()).as_uint();

			// add the item to the material inventory
			material_inventory.insert(pair<string, shared_ptr<Material>>(item->name, item));
		}

		return "You have logged in to IslandMUD!";
	}
	string logout()
	{
		// create a document to save the user's info
		xml_document user_data_xml;

		// create nodes to store user equipment and materials
		xml_node location_node = user_data_xml.append_child(C::XML_USER_LOCATION.c_str());
		xml_node equipment_node = user_data_xml.append_child(C::XML_USER_EQUIPMENT.c_str());
		xml_node material_node = user_data_xml.append_child(C::XML_USER_MATERIALS.c_str());

		// add x, y, and z attributes to the location node
		location_node.append_attribute(string("x").c_str()).set_value(this->x);
		location_node.append_attribute(string("y").c_str()).set_value(this->y);
		location_node.append_attribute(string("z").c_str()).set_value(this->z);

		// for each piece of equipment in the user's inventory
		for (multimap<string, shared_ptr<Equipment>>::const_iterator it = equipment_inventory.cbegin();
			it != equipment_inventory.cend(); ++it)
		{
			// save the equipment to a new node under the equipment node
			xml_node equipment = equipment_node.append_child(it->first.c_str());
		}

		// for each material in the user's inventory
		for (map<string, shared_ptr<Material>>::const_iterator it = material_inventory.cbegin();
			it != material_inventory.cend(); ++it)
		{
			// save the material to a new node under the material node
			xml_node material = material_node.append_child(it->first.c_str());

			// add an attribute called "count" with a value of material->count
			xml_attribute material_attribute = material.append_attribute(C::XML_USER_MATERIAL_COUNT.c_str());
			material_attribute.set_value(it->second->amount);
		}

		// save the user_data to disk
		user_data_xml.save_file((C::user_data_directory + "\\" + this->name + ".xml").c_str()); // returns an unused boolean

		return "You have logged out.";
	}

	// inventory manipulation
	void add(const shared_ptr<Item> & item);
	void remove(const string & item_id, const unsigned & count = 1);

	// inventory information
	bool has(const string & item_name, const unsigned & item_count = 1) const;
	bool does_not_have(const string & item_name, const unsigned & item_count = 1) const;

	// actions
	string craft(const string & craft_item_id, World & world);
	string move(const string & direction, World & world);
	string take(const string & item_id, World & world);
	string drop(const string & drop_item_id, World & world);
	string construct_surface(const string & material_id, const string & surface_id, World & world);

	// movement info
	string validate_movement(const string & direction_ID, const int & dx, const int & dy, const int & dz, const World & world) const
	{
		// determine if a character can move in a given direction (8 compass points, up, or down)

		// validate direction
		if (!R::contains(R::direction_ids, direction_ID)) { return direction_ID + " is not a direction."; }

		// if the player wants to move in a primary direction (n/e/s/w), condition is
		// no wall OR wall with opening (currently the opening is not taken into account)
		if (direction_ID == C::NORTH || direction_ID == C::EAST ||
			direction_ID == C::SOUTH || direction_ID == C::WEST)
		{
			// if the current room has a wall in the way
			// OR
			// the destination room has an opposite wall in the way
			if (world.room_at(x, y, z)->has_surface(direction_ID)
				||
				world.room_at(x + dx, y + dy, z)->has_surface(R::opposite_surface_id.find(direction_ID)->second))
			{
				return "There is a wall in your way to the " + direction_ID + ".";
			}
		}
		// if the player wants to move in a secondary direction (nw/ne/se/sw), condition is
		// one of two direction walls absent AND one of two result room opposite walls absent
		else if (
			direction_ID == C::NORTH_WEST || direction_ID == C::NORTH_EAST ||
			direction_ID == C::SOUTH_EAST || direction_ID == C::SOUTH_WEST)
		{
			const shared_ptr<Room> current_room = world.room_at(x, y, z);
			const shared_ptr<Room> destination_room = world.room_at(x + dx, y + dy, z);

			if (direction_ID == C::NORTH_WEST)
			{
				if (current_room->has_surface(C::NORTH) || current_room->has_surface(C::WEST) ||
					destination_room->has_surface(C::SOUTH) || destination_room->has_surface(C::EAST))
				{
					return "There are walls in your way to the " + direction_ID + ".";
				}
			}
			else if (direction_ID == C::NORTH_EAST)
			{
				if (current_room->has_surface(C::NORTH) || current_room->has_surface(C::EAST) ||
					destination_room->has_surface(C::SOUTH) || destination_room->has_surface(C::WEST))
				{
					return "There are walls in your way to the " + direction_ID + ".";
				}
			}
			else if (direction_ID == C::SOUTH_EAST)
			{
				if (current_room->has_surface(C::SOUTH) || current_room->has_surface(C::EAST) ||
					destination_room->has_surface(C::NORTH) || destination_room->has_surface(C::WEST))
				{
					return "There are walls in your way to the " + direction_ID + ".";
				}
			}
			else if (direction_ID == C::SOUTH_WEST)
			{
				if (current_room->has_surface(C::SOUTH) || current_room->has_surface(C::WEST) ||
					destination_room->has_surface(C::NORTH) || destination_room->has_surface(C::EAST))
				{
					return "There are walls in your way to the " + direction_ID + ".";
				}
			}
		}
		// condition for up is (opening AND ladder/stair/ramp)
		/*else if (direction_ID == C::UP || )
		{
		// HERE
		if ()
		{
		return "You [walk]/[climb] up to the [...]." //... ground level, second level, ...
		}
		}
		// condition for down is (ceiling) AND (ceiling has opening)
		else if (direction_ID == C::DOWN)
		{
		// HERE
		if ()
		{
		return "You drop down."; // ... to [ground level]/[the second level]
		}
		}*/

		// no issues were detected
		return C::GOOD_SIGNAL;
	}

};

#endif
