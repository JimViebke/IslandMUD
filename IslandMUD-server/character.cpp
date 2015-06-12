/* Jim Viebke
Jeb 16 2015 */

#include "character.h"
#include "world.h"

Recipes Character::recipes; // constructor calls populate()

string Character::login(World & world)
{
	// create a document to load the player's data
	xml_document user_data_xml;

	// load the player's data to user_data_xml
	user_data_xml.load_file((C::user_data_directory + "\\" + this->name + ".xml").c_str());



	// create holder values to save the coordinates from the file
	int loaded_x = -1, loaded_y = -1, loaded_z = -1;

	// load the three values from the node
	xml_node location_node = user_data_xml.child(C::XML_USER_LOCATION.c_str());

	// extract the attributes as well as the values for the attributes
	xml_attribute x_attribute = location_node.attribute(string("x").c_str());
	xml_attribute y_attribute = location_node.attribute(string("y").c_str());
	xml_attribute z_attribute = location_node.attribute(string("z").c_str());
	loaded_x = x_attribute.as_int();
	loaded_y = y_attribute.as_int();
	loaded_z = z_attribute.as_int();

	// if any of the attributes are empty or the extracted values fail bounds-checking
	if (x_attribute.empty() || y_attribute.empty() || z_attribute.empty() ||
		!R::bounds_check(loaded_x, loaded_y, loaded_z))
	{
		// set the player to the default spawn
		this->x = C::DEFAULT_SPAWN_X;
		this->y = C::DEFAULT_SPAWN_Y;
		this->z = C::DEFAULT_SPAWN_Z;
	}
	else
	{
		// set the player to the valid loaded coordinates
		this->x = loaded_x;
		this->y = loaded_y;
		this->z = loaded_z;
	}

	// load the rooms around the player's spawn
	world.load_view_radius_around(x, y, name);

	// spawn in the player
	world.room_at(x, y, z)->add_actor(this->name);



	// select the skill node
	xml_node skill_node = user_data_xml.child(C::XML_USER_SKILLS.c_str());

	// select each skill attribute
	xml_attribute swordsmanship_skill_attribute = skill_node.attribute(C::XML_SKILL_SWORDSMANSHIP.c_str());
	xml_attribute archery_skill_attribute = skill_node.attribute(C::XML_SKILL_ARCHERY.c_str());
	xml_attribute forest_visibility_skill_attribute = skill_node.attribute(C::XML_SKILL_FOREST_VISIBILITY.c_str());

	// if an attribute is non-empty, load its skill value
	if (!swordsmanship_skill_attribute.empty())
	{
		this->set_swordsmanship_skill(swordsmanship_skill_attribute.as_int());
	}
	if (!archery_skill_attribute.empty())
	{
		this->set_archery_skill(archery_skill_attribute.as_int());
	}
	if (!forest_visibility_skill_attribute.empty())
	{
		this->set_forest_visibilty_skill(forest_visibility_skill_attribute.as_int());
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



	// notify success
	return "You have logged in to IslandMUD!";
}
string Character::logout()
{
	// create a document to save the user's info
	xml_document user_data_xml;

	// create nodes to store user equipment and materials
	xml_node location_node = user_data_xml.append_child(C::XML_USER_LOCATION.c_str());
	xml_node skill_node = user_data_xml.append_child(C::XML_USER_SKILLS.c_str());
	xml_node equipment_node = user_data_xml.append_child(C::XML_USER_EQUIPMENT.c_str());
	xml_node material_node = user_data_xml.append_child(C::XML_USER_MATERIALS.c_str());

	// add x, y, and z attributes to the location node
	location_node.append_attribute(string("x").c_str()).set_value(this->x);
	location_node.append_attribute(string("y").c_str()).set_value(this->y);
	location_node.append_attribute(string("z").c_str()).set_value(this->z);

	// add each skill to the location node
	skill_node.append_attribute(C::XML_SKILL_SWORDSMANSHIP.c_str()).set_value(this->swordsmanship_skill);
	skill_node.append_attribute(C::XML_SKILL_ARCHERY.c_str()).set_value(this->archery_skill);
	skill_node.append_attribute(C::XML_SKILL_FOREST_VISIBILITY.c_str()).set_value(this->forest_visibility_skill);

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

// skills
void Character::set_swordsmanship_skill(const int & skill_value)
{
	if (skill_value > C::SWORDSMANSHIP_SKILL_MAX)
	{
		swordsmanship_skill = C::SWORDSMANSHIP_SKILL_MAX;
	}
	else if (skill_value < C::SWORDSMANSHIP_SKILL_MIN)
	{
		swordsmanship_skill = C::SWORDSMANSHIP_SKILL_MIN;
	}
	else
	{
		swordsmanship_skill = skill_value;
	}
}
void Character::set_archery_skill(const int & skill_value)
{
	if (skill_value > C::ARCHERY_SKILL_MAX)
	{
		archery_skill = C::ARCHERY_SKILL_MAX;
	}
	else if (skill_value < C::ARCHERY_SKILL_MIN)
	{
		archery_skill = C::ARCHERY_SKILL_MIN;
	}
	else
	{
		archery_skill = skill_value;
	}
}
void Character::set_forest_visibilty_skill(const int & skill_value)
{
	if (skill_value > C::FOREST_VISIBILITY_SKILL_MAX)
	{
		forest_visibility_skill = C::FOREST_VISIBILITY_SKILL_MAX;
	}
	else if (skill_value < C::FOREST_VISIBILITY_SKILL_MIN)
	{
		forest_visibility_skill = C::FOREST_VISIBILITY_SKILL_MIN;
	}
	else
	{
		forest_visibility_skill = skill_value;
	}
}

// inventory information
bool Character::has(const string & item_name, const unsigned & item_count) const
{
	if (item_count == 1) // only one instance is required
	{
		return
			equipment_inventory.find(item_name) != equipment_inventory.cend() ||
			material_inventory.find(item_name) != material_inventory.cend();
	}
	else // more than one item is required
	{
		return (equipment_inventory.count(item_name) >= item_count) // the equipment inventory contains item_count of the required item
			||
			(material_inventory.find(item_name) != material_inventory.cend() && // the material invenory contains the required item
			material_inventory.find(item_name)->second->amount >= item_count); // AND the material exists in sufficient quantity
	}
}
bool Character::does_not_have(const string & item_name, const unsigned & item_count) const
{
	return !this->has(item_name, item_count);
}

// inventory manipulation
void Character::add(const shared_ptr<Item> & item)
{
	// if the item is a material and is therefore stackable
	if (R::is<Material>(item))
	{
		// check if the player already has an instance of the item
		if (this->has(item->name))
		{
			// if so, increment the count
			this->material_inventory[item->name]->amount++;
		}
		else
		{
			// if not, give the player a new instance of the item
			this->material_inventory.insert(pair<string, shared_ptr<Material>>(item->name, R::convert_to<Material>(Craft::make(item->name))));
		}
	}
	else // the item is not a material and is therefore an Equipment type
	{
		// insert the new item
		this->equipment_inventory.insert(pair<string, shared_ptr<Equipment>>(item->name, R::convert_to<Equipment>(item)));
	}
}
void Character::remove(const string & item_id, const unsigned & count)
{
	// WARNING - for materials this assumes the player has [count] instances

	// remove or reduce the item in the player's inventory
	if (equipment_inventory.find(item_id) != equipment_inventory.cend())
	{
		for (unsigned i = 0; i < count; ++i)
		{
			// this deals with erase([key]) erroneously removing all instances of the item
			// multimap<string, shared_ptr<Equipment>>::iterator equipment_it = equipment_inventory.find(item_id);

			equipment_inventory.erase(equipment_inventory.find(item_id));
		}
	}
	else if (material_inventory.find(item_id) != material_inventory.cend()) // the item is present in the material inventory
	{
		material_inventory.find(item_id)->second->amount -= count; // decrement the material count in the player's inventory
		if (material_inventory.find(item_id)->second->amount < 1)
		{
			material_inventory.erase(material_inventory.find(item_id));
		}
	}
	else
	{
		// The player does not have the item
	}
}
string Character::equip(const string & item_ID)
{
	/*
	You ready your [item_id].
	You replace your [item_id] with a(n) [item_id];
	*/

	// if the player does not have the item specified
	if (!this->has(item_ID))
	{
		return string("You do not have a(n) ") + item_ID + " to equip.";
	}

	// create a stringstream to accumulate feedback
	stringstream output;

	// the player does have the item to equip, test if an item is already equipped
	if (this->equipped_item != nullptr)
	{
		output << "You replace your " << equipped_item->name << " with a(n) ";

		// save the existing the item to the player's inventory
		this->add(equipped_item);
		// erase the existing item
		this->equipped_item = nullptr;
	}

	// set the equipped item to the specified item from whichever inventory we find it in
	if (this->equipment_inventory.find(item_ID) != equipment_inventory.cend())
	{
		equipped_item = equipment_inventory.find(item_ID)->second;
	}
	else
	{
		equipped_item = material_inventory.find(item_ID)->second;
	}

	// remove or reduce the item in the player's inventory
	this->remove(item_ID);

	// if the stringstream is empty (no item was previously equipped)
	if (output.str().length() == 0)
	{
		return "You equip your " + item_ID + ".";
	}
	else
	{
		// complete and return the stringstream
		output << equipped_item->name << ".";
		return output.str();
	}

}
string Character::unequip(const string & item_ID)
{
	// test if no item is equipped
	if (this->equipped_item == nullptr)
	{
		return "You aren't holding anything.";
	}

	// save the existing the item to the player's inventory
	this->add(equipped_item);

	// reset the currently equipped item
	equipped_item = nullptr;

	return "You put your " + item_ID + " away.";
}

// actions
string Character::move(const string & direction_ID, World & world)
{
	// movement deltas
	int dx = 0, dy = 0, dz = 0;

	R::assign_movement_deltas(direction_ID, dx, dy, dz);

	// validate movement deltas
	if (!R::bounds_check(x + dx, y + dy, z + dz))
	{
		return "You can't go there.";
	}

	// copy the destination from disk to memory
	// world.load_room_to_world(x + dx, y + dy, z + dz);

	// test if the environment (structures) allow the player to move in a given direction
	string validate_movement = this->validate_movement(x, y, z, direction_ID, dx, dy, dz, world);

	// if the validation failed for any reason
	if (validate_movement != C::GOOD_SIGNAL)
	{
		// return that reason movement validation failed
		return validate_movement;
	}

	// the movement validated, load the radius for the destination
	world.load_view_radius_around(x + dx, y + dy, this->name);

	// remove viewing ID from rooms leaving view
	if (direction_ID == C::NORTH || direction_ID == C::SOUTH)
	{
		// if the character is moving north, add the view distance to x to get the x of the row being removed
		// otherwise (moving south) remove the distance from x
		int rx = (direction_ID == C::NORTH) ? x + C::VIEW_DISTANCE : x - C::VIEW_DISTANCE;
		// each room to try unload from from x,(y-view) to (x,y+view)
		for (int ry = y - C::VIEW_DISTANCE; ry <= y + C::VIEW_DISTANCE; ++ry)
		{
			// remove the character from the room's viewer list, trying to unload the room in the process
			world.remove_viewer_and_attempt_unload(rx, ry, C::GROUND_INDEX, this->name); // bounds checking takes place in here
		}
	}
	else if (direction_ID == C::WEST || direction_ID == C::EAST)
	{
		// logic is the same as above, but in rotated axes (axes is plural of axis)
		int ry = (direction_ID == C::WEST) ? y + C::VIEW_DISTANCE : y - C::VIEW_DISTANCE;
		for (int rx = x - C::VIEW_DISTANCE; rx <= x + C::VIEW_DISTANCE; ++rx)
		{
			world.remove_viewer_and_attempt_unload(rx, ry, C::GROUND_INDEX, this->name);
		}
	}
	else if (direction_ID == C::UP) { return "[moving up not available yet]"; }
	else if (direction_ID == C::DOWN) { return "[moving down not available yet]"; }
	else
	{
		/* The direction is a secondary compass direction.
		This means execution will alwways enter two of the four below blocks.
		Functionality here is the same as above. For documentation, scroll up. */

		if (direction_ID == C::NORTH_WEST || direction_ID == C::NORTH_EAST) // moving north, parse south row
		{
			int rx = x + C::VIEW_DISTANCE;
			for (int ry = y - C::VIEW_DISTANCE; ry <= y + C::VIEW_DISTANCE; ++ry)
			{
				world.remove_viewer_and_attempt_unload(rx, ry, C::GROUND_INDEX, this->name);
			}
		}

		if (direction_ID == C::NORTH_EAST || direction_ID == C::SOUTH_EAST) // moving east, parse west row
		{
			int ry = y - C::VIEW_DISTANCE;
			for (int rx = x - C::VIEW_DISTANCE; rx <= x + C::VIEW_DISTANCE; ++rx)
			{
				world.remove_viewer_and_attempt_unload(rx, ry, C::GROUND_INDEX, this->name);
			}
		}

		if (direction_ID == C::SOUTH_EAST || direction_ID == C::SOUTH_WEST) // moving south, parse north row
		{
			int rx = x - C::VIEW_DISTANCE;
			for (int ry = y - C::VIEW_DISTANCE; ry <= y + C::VIEW_DISTANCE; ++ry)
			{
				world.remove_viewer_and_attempt_unload(rx, ry, C::GROUND_INDEX, this->name);
			}
		}

		if (direction_ID == C::SOUTH_WEST || direction_ID == C::NORTH_WEST) // moving west, parse east row
		{
			int ry = y + C::VIEW_DISTANCE;
			for (int rx = x - C::VIEW_DISTANCE; rx <= x + C::VIEW_DISTANCE; ++rx)
			{
				world.remove_viewer_and_attempt_unload(rx, ry, C::GROUND_INDEX, this->name);
			}
		}
	}

	// the movement validated, remove character id from area
	world.room_at(x, y, z)->remove_actor(this->name);

	// test if the room can be unloaded
	if (world.is_unloadable(x, y, z)) // Why are we trying to unload the room we're moving out of? The player will still be able to see this.
	{
		world.unload_room(x, y, z);
	}
	// consider moving the above block to world.remove_character(x, y, z, id)
	// the check to unload could be in one place

	// update character internal coordinates
	x += dx;
	y += dy;
	z += dz;

	// add character id to new area using the new x and y coordinates
	world.room_at(x, y, z)->add_actor(this->name);

	// reply success
	return "You move " + direction_ID + ".";
}
string Character::craft(const string & craft_item_id, World & world)
{
	// finish crafting if the recipe does not exist
	if (!Character::recipes.has_recipe_for(craft_item_id)) { return "Can't craft " + craft_item_id; }

	// get the recipe
	const Recipe recipe = Character::recipes.get_recipe(craft_item_id);

	// verify the conditions for the recipe are present
	for (map<string, int>::const_iterator it = recipe.inventory_need.cbegin(); it != recipe.inventory_need.cend(); ++it)
	{
		if (it->first != "" && this->does_not_have(it->first)) { return craft_item_id + " requires " + ((it->second > 0) ? "a(n)" : R::to_string(it->second)) + " " + it->first; }
	}
	for (map<string, int>::const_iterator it = recipe.inventory_remove.cbegin(); it != recipe.inventory_remove.cend(); ++it)
	{
		if (it->first != "" && this->does_not_have(it->first)) { return craft_item_id + " uses " + ((it->second > 0) ? "a(n)" : R::to_string(it->second)) + " " + it->first; }
	}
	for (map<string, int>::const_iterator it = recipe.local_need.cbegin(); it != recipe.local_need.cend(); ++it)
	{
		if (it->first != "" && !world.room_at(x, y, z)->contains_item(it->first)) { return craft_item_id + " requires " + ((it->second > 0) ? "a" : R::to_string(it->second)) + " nearby " + it->first; }
	}
	for (map<string, int>::const_iterator it = recipe.local_remove.cbegin(); it != recipe.local_remove.cend(); ++it)
	{
		if (it->first != "" && !world.room_at(x, y, z)->contains_item(it->first)) { return craft_item_id + " uses " + ((it->second > 0) ? "a" : R::to_string(it->second)) + " nearby " + it->first; }
	}

	// remove ingredients from inventory
	for (map<string, int>::const_iterator it = recipe.inventory_remove.cbegin(); it != recipe.inventory_remove.cend(); ++it)
	{
		this->remove(it->first, it->second); // ID, count
	}

	// remove ingredients from area
	for (map<string, int>::const_iterator it = recipe.local_remove.cbegin(); it != recipe.local_remove.cend(); ++it)
	{
		this->remove(it->first, it->second); // ID, count
	}

	// for each item to be given to the player
	string response = "";
	for (map<string, int>::const_iterator it = recipe.yields.cbegin(); it != recipe.yields.cend(); ++it)
	{
		// for as many times as the item is to be given to the player
		for (int i = 0; i < it->second; ++i)
		{
			// craft the item
			shared_ptr<Item> item = Craft::make(it->first);

			// if the item can be carried
			if (item->is_takable())
			{
				// add the item to the player's inventory
				this->add(item);
			}
			else // the item can not be taken
			{
				// add the item to the room
				world.room_at(x, y, z)->add_item(item);
			}
		}

		// "You now have a(n) sword. " OR "You now have a(n) sword (x5). "
		response += "You now have a(n) " + it->first
			+ string((it->second > 1)
			? (" (x" + R::to_string(it->second) + ")") : "")
			+ ". ";
	}

	return response;
}
string Character::take(const string & take_item_id, World & world)
{
	// check if the item is not in the player's vicinity
	if (!world.room_at(x, y, z)->contains_item(take_item_id))
	{
		return "There is no " + take_item_id + " here.";
	}

	// check if the item is not takable
	if (!world.room_at(x, y, z)->get_contents().find(take_item_id)->second->is_takable())
	{
		// return failure
		return "You can't take the " + take_item_id + ".";
	}

	// the item is takable
	this->add(world.room_at(x, y, z)->get_contents().find(take_item_id)->second); // copy the item to the player
	world.room_at(x, y, z)->remove_item(take_item_id); // remove the item from the world

	return "You take the " + take_item_id + ".";
}
string Character::drop(const string & drop_item_id, World & world)
{
	if (!this->has(drop_item_id)) // if the player does not have the item specified
	{
		// the item does not exist in the player's inventory
		return "You don't have a(n) " + drop_item_id + " to drop.";
	}

	// add the item to the world
	world.room_at(x, y, z)->add_item(
		(equipment_inventory.find(drop_item_id) != equipment_inventory.end()) ? R::convert_to<Item>( // determine where to get the item from
		equipment_inventory.find(drop_item_id)->second) : // upcast one of the items to an <Item> type
		material_inventory.find(drop_item_id)->second
		);

	/// remove item
	this->remove(drop_item_id);

	// success reply
	return "You drop a(n) " + drop_item_id + ".";
}
string Character::construct_surface(const string & material_id, const string & surface_id, World & world)
{
	// make sure the material can be used to construct a surface
	if (C::SURFACE_REQUIREMENTS.find(material_id) == C::SURFACE_REQUIREMENTS.end())
	{
		return "You can't build a structure's surface out of " + material_id + ".";
	}

	// check if the surface already exists
	if (world.room_at(x, y, z)->has_surface(surface_id)) // bounds checking not necissary because the player is standing here
	{
		// test if construction is prevented by an intact wall or a pile of rubble
		if (world.room_at(x, y, z)->get_room_sides().find(surface_id)->second.is_rubble())
		{
			return "A pile of rubble prevents construction.";
		}
		else // the surface is intact
		{
			return ((surface_id == C::CEILING || surface_id == C::FLOOR) ?
				"A " + surface_id + " already exists here." : // ceiling or floor
				"A(n) " + surface_id + " wall already exists here."); // any wall
		}
	}

	// check that the surface to construct is a wall, ceiling, or floor
	if (!R::contains(C::surface_ids, surface_id))
	{
		return "Construct a wall, ceiling or floor.";
	}

	// if the surface is a ceiling, check that any intact wall exists
	if (surface_id == C::CEILING && // the user is construction a ceiling
		!world.room_at(x, y, z)->has_standing_wall()) // the room does not have a wall
	{
		return "You need at least one standing wall to support a ceiling.";
	}

	// check that the player has the item
	if (this->material_inventory.find(material_id) == material_inventory.end())
	{
		return "You don't have " + material_id + ".";
	}

	// check that the player has enough of the item to construct
	if (this->material_inventory.find(material_id)->second->amount < C::SURFACE_REQUIREMENTS.find(material_id)->second)
	{
		// "You need 5 wood to continue construction."
		return "You need " + R::to_string(C::SURFACE_REQUIREMENTS.find(material_id)->second) + " " + material_id + " to continue construction.";
	}

	// decrement the required supplies from the player's inventory
	this->material_inventory.find(material_id)->second->amount -= C::SURFACE_REQUIREMENTS.find(material_id)->second;

	// if the player has less that one of the material remaining
	if (this->material_inventory.find(material_id)->second->amount < 1)
	{
		// remove the object from the player's inventory
		this->material_inventory.erase(material_id);
	}

	// create a Room_Side and add it to Room::room_side using the surface ID
	world.room_at(x, y, z)->add_surface(surface_id, material_id);

	// "You construct a stone floor/ceiling." OR "You construct a stone wall to your north."
	return "You construct a " + material_id + // you construct a [material]
		((surface_id != C::CEILING && surface_id != C::FLOOR) ?
		" wall to your " + surface_id : // wall to your [direction]
		" " + surface_id); // ceiling/floor
}
string Character::construct_door(const string & material_ID, const string & surface_ID, World & world)
{
	// check that the surface is valid
	if (!R::contains(C::surface_ids, surface_ID))
	{
		return surface_ID + " is not a surface.";
	}

	// check that this room has the surface specified
	if (!world.room_at(x, y, z)->has_surface(surface_ID))
	{
		//  no ceiling here / no west wall here
		return "There is no " + surface_ID + ((surface_ID == C::CEILING || surface_ID == C::FLOOR) ? "" : " wall") + " here to have a door.";
	}

	// check if the wall is standing
	if (!world.room_at(x, y, z)->is_standing_wall(surface_ID))
	{
		return "A pile of rubble prevents construction.";
	}

	// check if a door already exists
	if (world.room_at(x, y, z)->get_room_sides().find(surface_ID)->second.has_door())
	{
		// test if the door is rubble or intact
		if (world.room_at(x, y, z)->get_room_sides().find(surface_ID)->second.get_door()->is_rubble())
		{
			return "A pile of rubble in the doorway prevents construction.";
		}
		else // the door is intact
		{
			// ...in the ceiling / ...in the east wall
			return "There is already a door in the " + surface_ID + ((surface_ID == C::CEILING || surface_ID == C::FLOOR) ? "." : " wall.");
		}
	}

	// check that there exist requirements for making a door of the specified type
	if (C::DOOR_REQUIREMENTS.find(material_ID) == C::DOOR_REQUIREMENTS.cend())
	{
		return "ERROR: No material requirements available to construct door using " + material_ID;
	}

	// extract the amount of materials required to make a door of the specified type
	const unsigned MATERIAL_COUNT_REQUIRED = C::DOOR_REQUIREMENTS.find(material_ID)->second;

	// check that the player has the required materials
	if (!this->has(material_ID, MATERIAL_COUNT_REQUIRED))
	{
		// "A stone door requires 5 stone."
		return "A " + material_ID + " door requires " + R::to_string(MATERIAL_COUNT_REQUIRED) + " " + material_ID + ".";
	}

	// add a door to the surface in the room
	world.room_at(x, y, z)->add_door(surface_ID, C::MAX_SURFACE_HEALTH, material_ID, this->faction_ID);

	// remove the consumed materials from the actor's inventory
	this->remove(material_ID, MATERIAL_COUNT_REQUIRED);

	// "...door in the ceiling." or "...door in the west wall."
	return "You construct a " + material_ID + " door in the " + surface_ID + ((surface_ID == C::CEILING || surface_ID == C::FLOOR) ? "." : " wall.");
}
string Character::attack_surface(const string & surface_ID, World & world)
{
	// get this check out of the way
	if (surface_ID == C::CEILING || surface_ID == C::FLOOR)
	{
		return "Damaging a surface in a room above or below you is not supported yet.";
	}

	// verify we are working with a primary compass point
	if (surface_ID != C::NORTH && surface_ID != C::EAST &&
		surface_ID != C::SOUTH && surface_ID != C::WEST)
	{
		return "Only n/s/e/w surfaces can be damaged at this time.";
	}

	// if the current room has an intact surface
	if (world.room_at(x, y, z)->is_standing_wall(surface_ID))
	{
		// apply damage to the surface
		return world.room_at(x, y, z)->damage_surface(surface_ID, this->equipped_item);
	}

	// this room does not have an intact surface, the neighboring room might

	// find coordinates of neighboring room
	int new_x = x, new_y = y;
	{
		int new_z = 0;
		R::assign_movement_deltas(surface_ID, new_x, new_y, new_z);
	} // dz falls out of scope to prevent accidental use - we're only working in two dimensions right now

	// if the neighboring room has the opposite surface intact (our west wall borders next room's east wall)
	if (world.room_at(new_x, new_y, z)->is_standing_wall(C::opposite_surface_id.find(surface_ID)->second)) // deliberately using just "z" throughout this block
	{
		// inflict damage upon the surface
		return world.room_at(new_x, new_y, z)->damage_surface(C::opposite_surface_id.find(surface_ID)->second, this->equipped_item);
	}

	// neither room has an intact surface

	// test if both walls do not exist
	if (!world.room_at(x, y, z)->has_surface(surface_ID) &&
		!world.room_at(new_x, new_y, z)->has_surface(C::opposite_surface_id.find(surface_ID)->second))
	{
		return "There is no " + surface_ID + " wall here.";
	}
	else
	{
		// any surface that does exist is rubble, and at least one surface exists
		return "There is only rubble where a wall once was.";
	}
}
string Character::attack_door(const string & surface_ID, World & world)
{
	// get this check out of the way
	if (surface_ID == C::CEILING || surface_ID == C::FLOOR)
	{
		return "Damaging above or below you is not supported yet.";
	}

	// verify we are working with a primary compass point
	if (surface_ID != C::NORTH && surface_ID != C::EAST &&
		surface_ID != C::SOUTH && surface_ID != C::WEST)
	{
		return "Only doors in n/s/e/w surfaces can be damaged at this time.";
	}

	// if the current room has an intact surface with an intact door in it
	if (world.room_at(x, y, z)->has_surface(surface_ID) && world.room_at(x, y, z)->get_room_sides().find(surface_ID)->second.has_intact_door())
	{
		// applied damage to the door
		return world.room_at(x, y, z)->damage_door(surface_ID, this->equipped_item);
	}

	// the current room does not have an intact door in the specified direction,
	// test if the next room has an intact door facing us.

	// find coordinates of target room
	int new_x = x, new_y = y;
	{
		int new_z = 0;
		R::assign_movement_deltas(surface_ID, new_x, new_y, new_z);
	} // new_z falls out of scope to prevent accidental use - we're only working in two dimensions right now

	// if the neighboring room has the opposite surface intact
	if (world.room_at(new_x, new_y, z)->is_standing_wall(C::opposite_surface_id.find(surface_ID)->second)) // deliberately using just "z" throughout this block
	{
		// inflict damaage upon the surface or door
		return world.room_at(new_x, new_y, z)->damage_door(C::opposite_surface_id.find(surface_ID)->second, this->equipped_item);
	}

	// this feedback might not be correct for all cases
	return "There is no door to your " + surface_ID;
}

// movement info
string Character::validate_movement(const int & cx, const int & cy, const int & cz, const string & direction_ID, const int & dx, const int & dy, const int & dz, const World & world) const
{
	// determine if a character can move in a given direction (8 compass points, up, or down)

	// validate direction
	if (!R::contains(C::direction_ids, direction_ID)) { return direction_ID + " is not a direction."; }

	// if the player wants to move in a primary direction (n/e/s/w)
	if (direction_ID == C::NORTH || direction_ID == C::EAST ||
		direction_ID == C::SOUTH || direction_ID == C::WEST)
	{
		// save the value of an attempt to move out of the current room
		string move_attempt = world.room_at(cx, cy, cz)->can_move_in_direction(direction_ID, faction_ID);

		if (move_attempt != C::GOOD_SIGNAL)
		{
			// the player can't move out of the current room
			return move_attempt;
		}

		// save the value of an attempt to move into the destination room
		move_attempt = world.room_at(cx + dx, cy + dy, cz)->can_move_in_direction(C::opposite_surface_id.find(direction_ID)->second, faction_ID);

		if (move_attempt != C::GOOD_SIGNAL)
		{
			// the player can't move into the destination
			return move_attempt;
		}
	}
	// if the player wants to move in a secondary direction (nw/ne/se/sw), condition is
	// one of two direction walls absent AND one of two result room opposite walls absent
	else if (
		direction_ID == C::NORTH_WEST || direction_ID == C::NORTH_EAST ||
		direction_ID == C::SOUTH_EAST || direction_ID == C::SOUTH_WEST)
	{
		const shared_ptr<Room> current_room = world.room_at(cx, cy, cz);
		const shared_ptr<Room> destination_room = world.room_at(cx + dx, cy + dy, cz);

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
	if ()
	{
	return "You [walk]/[climb] up to the [...]." //... ground level, second level, ...
	}
	}
	// condition for down is (ceiling) AND (ceiling has opening)
	else if (direction_ID == C::DOWN)
	{
	if ()
	{
	return "You drop down."; // ... to [ground level]/[the second level]
	}
	}*/

	// no issues were detected
	return C::GOOD_SIGNAL;
}
