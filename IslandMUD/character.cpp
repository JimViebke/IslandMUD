/* Jim Viebke
Jeb 16 2015 */

#include "constants.h"
#include "character.h"
#include "world.h"

Recipes Character::recipes;

// Character constructor
Character::Character(const string & name, const string & set_faction_ID) : name(name)
{
	// if the faction is valid
	if (set_faction_ID == C::PC_FACTION_ID ||
		set_faction_ID == C::NPC_NEUTRAL_FACTION_ID ||
		set_faction_ID == C::NPC_HOSTILE_FACTION_ID)
	{
		// set the actor's faction
		this->faction_ID = set_faction_ID;
	}
	else // the faction is not valid
	{
		// Raise an error in the console
		cout << "ERROR: attempted to create character with invalid faction: [" << set_faction_ID << "]\n";
	}
}

string Character::login(World & world)
{
	// create a document to load the player's data
	xml_document user_data_xml;

	// load the player's data to user_data_xml
	user_data_xml.load_file((C::user_data_directory + "/" + this->name + ".xml").c_str());



	// create holder values to save the coordinates from the file
	int loaded_x = -1, loaded_y = -1, loaded_z = -1;

	// load the three values from the node
	const xml_node location_node = user_data_xml.child(C::XML_USER_LOCATION.c_str());

	// extract the attributes as well as the values for the attributes
	const xml_attribute x_attribute = location_node.attribute(string("x").c_str());
	const xml_attribute y_attribute = location_node.attribute(string("y").c_str());
	const xml_attribute z_attribute = location_node.attribute(string("z").c_str());
	loaded_x = x_attribute.as_int();
	loaded_y = y_attribute.as_int();
	loaded_z = z_attribute.as_int();

	// if any of the attributes are empty or the extracted values fail bounds-checking
	if (x_attribute.empty() || y_attribute.empty() || z_attribute.empty() ||
		!U::bounds_check(loaded_x, loaded_y, loaded_z))
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

	// select the level node
	const xml_node level_node = user_data_xml.child(C::XML_USER_LEVELS.c_str());

	// select each level attribute
	const xml_attribute swordsmanship_level_attribute = level_node.attribute(C::XML_LEVEL_SWORDSMANSHIP.c_str());
	const xml_attribute archery_level_attribute = level_node.attribute(C::XML_LEVEL_ARCHERY.c_str());
	const xml_attribute forest_visibility_level_attribute = level_node.attribute(C::XML_LEVEL_FOREST_VISIBILITY.c_str());
	const xml_attribute full_health_level_attribute = level_node.attribute(C::XML_LEVEL_HEALTH_MAX.c_str());

	// if an attribute is non-empty, load its level value
	if (!swordsmanship_level_attribute.empty())
	{
		this->set_swordsmanship_level(swordsmanship_level_attribute.as_int());
	}
	if (!archery_level_attribute.empty())
	{
		this->set_archery_level(archery_level_attribute.as_int());
	}
	if (!forest_visibility_level_attribute.empty())
	{
		this->set_forest_visibilty_level(forest_visibility_level_attribute.as_int());
	}
	if (!full_health_level_attribute.empty())
	{
		this->set_health_max(full_health_level_attribute.as_int());
	}

	// select status node (just holds current_health at this time)
	const xml_node status_node = user_data_xml.child(C::XML_USER_STATUS.c_str());

	// if current_health is non-empty, set current health
	const xml_attribute current_health_attribute = status_node.attribute(C::XML_USER_STATUS_CURRENT_HEALTH.c_str());
	if (!current_health_attribute.empty())
	{
		this->set_current_health(current_health_attribute.as_int());
	}
	else // if current_health is empty, explicitly set current health to base maximum
	{
		this->set_current_health(C::FULL_HEALTH_MIN);
	}


	// for each item node of the equipment node
	for (const xml_node & equipment_node : user_data_xml.child(C::XML_USER_EQUIPMENT.c_str()).children())
	{
		// create the item
		shared_ptr<Equipment> equipment = U::convert_to<Equipment>(Craft::make(equipment_node.name()));

		// extract the value of the health attribute and use it to set the item's health
		equipment->set_health(equipment_node.attribute(C::XML_ITEM_HEALTH.c_str()).as_int());

		// add the item to the player's inventory
		equipment_inventory.insert(pair<string, shared_ptr<Equipment>>(equipment->name, equipment));
	}

	// for each item in the material node
	for (const xml_node & material : user_data_xml.child(C::XML_USER_MATERIALS.c_str()).children())
	{
		// use the name of the material node to create a new materail object
		shared_ptr<Material> item = U::convert_to<Material>(Craft::make(material.name()));

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
	// if an item is equipped, move it back to the player's inventory
	this->unequip();

	// create a document to save the user's info
	xml_document user_data_xml;

	// create nodes to store user equipment and materials
	xml_node status_node = user_data_xml.append_child(C::XML_USER_STATUS.c_str());
	xml_node location_node = user_data_xml.append_child(C::XML_USER_LOCATION.c_str());
	xml_node level_node = user_data_xml.append_child(C::XML_USER_LEVELS.c_str());
	xml_node equipment_node = user_data_xml.append_child(C::XML_USER_EQUIPMENT.c_str());
	xml_node material_node = user_data_xml.append_child(C::XML_USER_MATERIALS.c_str());

	/// add health attribute to status node
	status_node.append_attribute(C::XML_USER_STATUS_CURRENT_HEALTH.c_str()).set_value(this->current_health);

	// add x, y, and z attributes to the location node
	location_node.append_attribute(string("x").c_str()).set_value(this->x);
	location_node.append_attribute(string("y").c_str()).set_value(this->y);
	location_node.append_attribute(string("z").c_str()).set_value(this->z);

	// add each level to the location node
	level_node.append_attribute(C::XML_LEVEL_SWORDSMANSHIP.c_str()).set_value(this->swordsmanship_level);
	level_node.append_attribute(C::XML_LEVEL_ARCHERY.c_str()).set_value(this->archery_level);
	level_node.append_attribute(C::XML_LEVEL_FOREST_VISIBILITY.c_str()).set_value(this->forest_visibility_level);
	level_node.append_attribute(C::XML_LEVEL_HEALTH_MAX.c_str()).set_value(this->max_health);

	// for each piece of equipment in the user's inventory
	for (multimap<string, shared_ptr<Equipment>>::const_iterator it = equipment_inventory.cbegin();
		it != equipment_inventory.cend(); ++it)
	{
		// save the equipment to a new node under the equipment node
		xml_node equipment = equipment_node.append_child(it->first.c_str());

		// append a health attribute to the equipment node and set its value to the health of the equipment
		equipment.append_attribute(C::XML_ITEM_HEALTH.c_str()).set_value(it->second->get_health());
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
	user_data_xml.save_file((C::user_data_directory + "/" + this->name + ".xml").c_str()); // returns an unused boolean

	return "You have logged out.";
}

// levels
void Character::set_swordsmanship_level(const int & level_value)
{
	if (level_value > C::SWORDSMANSHIP_LEVEL_MAX)
	{
		swordsmanship_level = C::SWORDSMANSHIP_LEVEL_MAX;
	}
	else if (level_value < C::SWORDSMANSHIP_LEVEL_MIN)
	{
		swordsmanship_level = C::SWORDSMANSHIP_LEVEL_MIN;
	}
	else
	{
		swordsmanship_level = level_value;
	}
}
void Character::set_archery_level(const int & level_value)
{
	if (level_value > C::ARCHERY_LEVEL_MAX)
	{
		archery_level = C::ARCHERY_LEVEL_MAX;
	}
	else if (level_value < C::ARCHERY_LEVEL_MIN)
	{
		archery_level = C::ARCHERY_LEVEL_MIN;
	}
	else
	{
		archery_level = level_value;
	}
}
void Character::set_forest_visibilty_level(const int & level_value)
{
	if (level_value > C::FOREST_VISIBILITY_LEVEL_MAX)
	{
		forest_visibility_level = C::FOREST_VISIBILITY_LEVEL_MAX;
	}
	else if (level_value < C::FOREST_VISIBILITY_LEVEL_MIN)
	{
		forest_visibility_level = C::FOREST_VISIBILITY_LEVEL_MIN;
	}
	else
	{
		forest_visibility_level = level_value;
	}
}
void Character::set_health_max(const int & level_value)
{
	if (level_value > C::FULL_HEALTH_MAX)
	{
		max_health = C::FULL_HEALTH_MAX;
	}
	else if (level_value < C::FULL_HEALTH_MIN)
	{
		max_health = C::FULL_HEALTH_MIN;
	}
	else
	{
		max_health = level_value;
	}
}

// setters
void Character::set_current_health(const int & health_value)
{
	// don't call this until after the player's max_health has been set
	// otherwise the max_health field will still be at the default game constant

	if (health_value > max_health)
	{
		current_health = max_health;
	}
	else if (health_value <= C::HEALTH_MIN) // we can't set the user's health level to "die", so just use max
	{
		current_health = max_health;
	}
	else
	{
		current_health = health_value;
	}
}

// inventory information
bool Character::has(const string & item_name, const unsigned & item_count) const
{
	if (item_count == 1) // only one instance is required
	{
		return
			((equipped_item != nullptr) ? equipped_item->name : "") == item_name ||
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
string Character::get_inventory() const // debugging
{
	stringstream contents;
	for (multimap<string, shared_ptr<Equipment>>::const_iterator it = equipment_inventory.begin();
		it != equipment_inventory.end(); ++it)
	{
		contents << it->second->name << " ";
	}
	for (map<string, shared_ptr<Material>>::const_iterator it = material_inventory.begin();
		it != material_inventory.end(); ++it)
	{
		contents << it->second->name << ":" << it->second->amount << " ";
	}
	return contents.str();
}

// inventory manipulation
void Character::add(const shared_ptr<Item> & item)
{
	// if the item is a material and is therefore stackable
	if (U::is<Material>(item))
	{
		// check if the player already has an instance of the item in their material inventory
		if (this->material_inventory.find(item->name) != this->material_inventory.cend())
		{
			// if so, increment the count
			this->material_inventory[item->name]->amount++;
		}
		else
		{
			// if not, give the player a new instance of the item
			this->material_inventory.insert(pair<string, shared_ptr<Material>>(item->name, U::convert_to<Material>(Craft::make(item->name))));
		}
	}
	else // the item is not a material and is therefore an Equipment type
	{
		// insert the new item
		this->equipment_inventory.insert(pair<string, shared_ptr<Equipment>>(item->name, U::convert_to<Equipment>(item)));
	}
}
void Character::remove(const string & item_id, const unsigned & count)
{
	// WARNING - for materials this assumes the player has [count] instances

	// if the player is holding the item
	if (this->equipped_item != nullptr && this->equipped_item->name == item_id)
	{
		this->equipped_item = nullptr; // erase the item
	}
	// remove or reduce the item in the player's inventory
	else if (equipment_inventory.find(item_id) != equipment_inventory.cend())
	{
		for (unsigned i = 0; i < count; ++i)
		{
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
		// the player does not have the item
	}
}
Update_Messages Character::equip(const string & item_ID)
{
	/*
	You ready your [item_id].
	You replace your [item_id] with a(n) [item_id];
	*/

	// if the player does not have the item specified
	if (!this->has(item_ID))
	{
		return Update_Messages("You do not have " + U::get_article_for(item_ID) + " " + item_ID + " to equip.");
	}

	// the player has at least one instance of the item, check if the player does not have another one to equip
	if (equipment_inventory.find(item_ID) == equipment_inventory.cend() &&
		material_inventory.find(item_ID) == material_inventory.cend())
	{
		return Update_Messages("You are holding " + U::get_article_for(item_ID) + " " + item_ID + " and don't have another one to equip.");
	}

	// create a stringstream to accumulate feedback
	stringstream user_update;
	stringstream room_update;

	// the player does have the item to equip, test if an item is already equipped
	if (this->equipped_item != nullptr)
	{
		user_update << "You replace your " << equipped_item->name << " with ";
		room_update << this->name << " replaces " << U::get_article_for(equipped_item->name) << " " << equipped_item->name << " with ";

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
	if (equipment_inventory.find(item_ID) != equipment_inventory.cend())
	{
		equipment_inventory.erase(equipment_inventory.find(item_ID));
	}
	else if (material_inventory.find(item_ID) != material_inventory.cend()) // the item is present in the material inventory
	{
		material_inventory.find(item_ID)->second->amount--; // decrement the material count in the player's inventory
		if (material_inventory.find(item_ID)->second->amount < 1)
		{
			material_inventory.erase(material_inventory.find(item_ID));
		}
	}

	// if the stringstream is empty (no item was previously equipped)
	if (user_update.str().length() == 0)
	{
		return Update_Messages(
			"You equip your " + item_ID + ".",
			this->name + " equips " + U::get_article_for(item_ID) + " " + item_ID + ".");
	}
	else
	{
		// complete and return the response message
		user_update << U::get_article_for(equipped_item->name) << " " << equipped_item->name << ".";
		room_update << U::get_article_for(equipped_item->name) << " " << equipped_item->name << ".";

		return Update_Messages(user_update.str(), room_update.str());
	}

}
Update_Messages Character::unequip()
{
	// test if no item is equipped
	if (this->equipped_item == nullptr)
	{
		return Update_Messages("You aren't holding anything.");
	}

	// save the ID of the currently equipped item
	const string item_ID = equipped_item->name;

	// save the existing the item to the player's inventory
	this->add(equipped_item);

	// reset the currently equipped item
	equipped_item = nullptr;

	return Update_Messages("You put your " + item_ID + " away.", this->name + " lowers the " + item_ID + ".");
}

// actions
string Character::move(const string & direction_ID, World & world)
{
	// movement deltas
	int dx = 0, dy = 0, dz = 0;

	U::assign_movement_deltas(direction_ID, dx, dy, dz);

	// validate movement deltas
	if (!U::bounds_check(x + dx, y + dy, z + dz))
	{
		return "You can't go there.";
	}

	// copy the destination from disk to memory
	// world.load_room_to_world(x + dx, y + dy, z + dz);

	// test if the environment (structures) allow the player to move in a given direction
	const string validate_movement = this->validate_movement(x, y, z, direction_ID, dx, dy, dz, world);

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
		const int ry = (direction_ID == C::WEST) ? y + C::VIEW_DISTANCE : y - C::VIEW_DISTANCE;
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
			const int rx = x + C::VIEW_DISTANCE;
			for (int ry = y - C::VIEW_DISTANCE; ry <= y + C::VIEW_DISTANCE; ++ry)
			{
				world.remove_viewer_and_attempt_unload(rx, ry, C::GROUND_INDEX, this->name);
			}
		}

		if (direction_ID == C::NORTH_EAST || direction_ID == C::SOUTH_EAST) // moving east, parse west row
		{
			const int ry = y - C::VIEW_DISTANCE;
			for (int rx = x - C::VIEW_DISTANCE; rx <= x + C::VIEW_DISTANCE; ++rx)
			{
				world.remove_viewer_and_attempt_unload(rx, ry, C::GROUND_INDEX, this->name);
			}
		}

		if (direction_ID == C::SOUTH_EAST || direction_ID == C::SOUTH_WEST) // moving south, parse north row
		{
			const int rx = x - C::VIEW_DISTANCE;
			for (int ry = y - C::VIEW_DISTANCE; ry <= y + C::VIEW_DISTANCE; ++ry)
			{
				world.remove_viewer_and_attempt_unload(rx, ry, C::GROUND_INDEX, this->name);
			}
		}

		if (direction_ID == C::SOUTH_WEST || direction_ID == C::NORTH_WEST) // moving west, parse east row
		{
			const int ry = y + C::VIEW_DISTANCE;
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
	// check for special case
	if (craft_item_id == C::CHEST_ID)
	{
		if (world.room_at(x, y, z)->has_chest())
		{
			return "There is already a chest here.";
		}
	}

	// return if the recipe does not exist
	if (!Character::recipes.has_recipe_for(craft_item_id)) { return "There is no way to craft " + U::get_article_for(craft_item_id) + " " + craft_item_id + "."; }

	// get the recipe
	const Recipe recipe = Character::recipes.get_recipe(craft_item_id);

	// verify the conditions for the recipe are present
	for (map<string, int>::const_iterator it = recipe.inventory_need.cbegin(); it != recipe.inventory_need.cend(); ++it)
	{
		if (it->first != "" && !this->has(it->first, it->second)) { return U::get_article_for(craft_item_id) + " " + craft_item_id + " requires " + ((it->second == 1) ? U::get_article_for(it->first) : U::to_string(it->second)) + " " + it->first; }
	}
	for (map<string, int>::const_iterator it = recipe.inventory_remove.cbegin(); it != recipe.inventory_remove.cend(); ++it)
	{
		if (it->first != "" && !this->has(it->first, it->second)) { return U::get_article_for(craft_item_id) + " " + craft_item_id + " uses " + ((it->second == 1) ? U::get_article_for(it->first) : U::to_string(it->second)) + " " + it->first; }
	}
	for (map<string, int>::const_iterator it = recipe.local_need.cbegin(); it != recipe.local_need.cend(); ++it)
	{
		if (it->first != "" && !world.room_at(x, y, z)->contains_item(it->first)) { return U::get_article_for(craft_item_id) + " " + craft_item_id + " requires " + ((it->second == 1) ? "a" : U::to_string(it->second)) + " nearby " + it->first; }
	}
	for (map<string, int>::const_iterator it = recipe.local_remove.cbegin(); it != recipe.local_remove.cend(); ++it)
	{
		if (it->first != "" && !world.room_at(x, y, z)->contains_item(it->first)) { return U::get_article_for(craft_item_id) + " " + craft_item_id + " uses " + ((it->second == 1) ? "a" : U::to_string(it->second)) + " nearby " + it->first; }
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

	// special test cast for chests
	if (craft_item_id == C::CHEST_ID)
	{
		// add a chest to the room
		world.room_at(x, y, z)->add_chest(this->faction_ID);
		return "You craft a chest.";
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
		response += "You now have " + U::get_article_for(it->first) + " " + it->first
			+ string((it->second > 1)
			? (" (x" + U::to_string(it->second) + ")") : "")
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
	// if the player is holding the item specified
	if (this->equipped_item != nullptr && this->equipped_item->name == drop_item_id)
	{
		// add the item to the world
		world.room_at(x, y, z)->add_item(this->equipped_item);
	}
	else
	{
		if (!this->has(drop_item_id)) // if the player does not have the item specified
		{
			// the item does not exist in the player's inventory
			return "You don't have " + U::get_article_for(drop_item_id) + " " + drop_item_id + " to drop.";
		}

		// add the item to the world
		world.room_at(x, y, z)->add_item(
			(equipment_inventory.find(drop_item_id) != equipment_inventory.end()) ? U::convert_to<Item>( // determine where to get the item from
			equipment_inventory.find(drop_item_id)->second) : // upcast one of the items to an <Item> type
			material_inventory.find(drop_item_id)->second
			);
	}

	/// remove item
	this->remove(drop_item_id);

	// success reply
	return "You drop " + U::get_article_for(drop_item_id) + " " + drop_item_id + ".";
}
string Character::add_to_chest(const string & insert_item_id, World & world)
{
	// if this room does not have a chest
	if (!world.room_at(x, y, z)->has_chest())
	{
		return "There is no chest here.";
	}

	// if the chest was crafted by another faction
	if (world.room_at(x, y, z)->get_chest_faction_id() != this->faction_ID)
	{
		return "This chest has an unfamiliar lock.";
	}

	// if the player doesn't have the item
	if (!this->has(insert_item_id))
	{
		return "You don't have " + U::get_article_for(insert_item_id) + " " + insert_item_id + ".";
	}

	// if the item is equipped
	if (equipped_item != nullptr && equipped_item->name == insert_item_id)
	{
		world.room_at(x, y, z)->add_item_to_chest(equipped_item);
	}
	// if the item is a piece of equipment
	else if (equipment_inventory.find(insert_item_id) != equipment_inventory.cend())
	{
		// add the item
		world.room_at(x, y, z)->add_item_to_chest(equipment_inventory.find(insert_item_id)->second);
	}
	else // the item is a material that the user may have 1 or more
	{
		// create a new instance to add the the chest
		world.room_at(x, y, z)->add_item_to_chest(Craft::make(insert_item_id));
	}

	// remove it from the player's inventory (this works for materials too)
	this->remove(insert_item_id);

	// You place the sword into the chest.
	return "You place the " + insert_item_id + " into the chest.";
}
string Character::take_from_chest(const string & take_item_id, World & world)
{
	// if this room does not have a chest
	if (!world.room_at(x, y, z)->has_chest())
	{
		return "There is no chest here.";
	}

	// if the chest was crafted by another faction
	if (world.room_at(x, y, z)->get_chest_faction_id() != this->faction_ID)
	{
		return "This chest has an unfamiliar lock.";
	}

	// if the player doesn't have the item
	if (!world.room_at(x, y, z)->chest_has(take_item_id))
	{
		return "The chest does not contain " + U::get_article_for(take_item_id) + " " + take_item_id + ".";
	}

	this->add(world.room_at(x, y, z)->remove_from_chest(take_item_id));

	// You place the sword into the chest.
	return "You take the " + take_item_id + " from the chest.";
}
string Character::look_inside_chest(const World & world) const
{
	// validation within
	return world.room_at(x, y, z)->chest_contents(faction_ID);
}
string Character::construct_surface(const string & material_id, const string & surface_id, World & world)
{
	if (world.room_at(x, y, z)->is_forest())
	{
		return "You are in a forest and cannot build a structure here.";
	}

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
				U::capitalize(U::get_article_for(surface_id)) + " " + surface_id + " wall already exists here."); // any wall
		}
	}

	// check that the surface to construct is a wall, ceiling, or floor
	if (!U::contains(C::surface_ids, surface_id))
	{
		return "Construct a wall, ceiling or floor.";
	}

	// if the surface is a ceiling, check that any intact wall exists
	if (surface_id == C::CEILING && // the user is constructing a ceiling
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
		return "You need " + U::to_string(C::SURFACE_REQUIREMENTS.find(material_id)->second) + " " + material_id + " to continue construction.";
	}

	// remove the number of materials from the player's inventory
	this->remove(material_id, C::SURFACE_REQUIREMENTS.find(material_id)->second);

	// create a Room_Side and add it to Room::room_side using the surface ID
	world.room_at(x, y, z)->add_surface(surface_id, material_id);

	// "You construct a stone floor/ceiling." OR "You construct a stone wall to your north."
	return "You construct a " + material_id + // you construct a [material]
		((surface_id != C::CEILING && surface_id != C::FLOOR) ?
		" wall to your " + surface_id : // wall to your [direction]
		" " + surface_id); // ceiling/floor
}
string Character::construct_surface_with_door(const string & surface_material_id, const string & surface_id, const string & door_material_id, World & world)
{
	// Part 1: validate that a surface can be constructed



	if (world.room_at(x, y, z)->is_forest())
	{
		return "You are in a forest and cannot build a structure here.";
	}

	// make sure the material can be used to construct a surface
	if (C::SURFACE_REQUIREMENTS.find(surface_material_id) == C::SURFACE_REQUIREMENTS.end())
	{
		return "You can't build a structure's surface out of " + surface_material_id + ".";
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
				U::capitalize(U::get_article_for(surface_id)) + " " + surface_id + " wall already exists here."); // any wall
		}
	}

	// check that the surface to construct is a wall, ceiling, or floor
	if (!U::contains(C::surface_ids, surface_id))
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
	if (this->material_inventory.find(surface_material_id) == material_inventory.end())
	{
		return "You don't have " + surface_material_id + ".";
	}

	// check that the player has enough of the item to construct
	if (this->material_inventory.find(surface_material_id)->second->amount < C::SURFACE_REQUIREMENTS.find(surface_material_id)->second)
	{
		// "You need 5 wood to continue construction of the wall."
		return "You need " + U::to_string(C::SURFACE_REQUIREMENTS.find(surface_material_id)->second) + " " + surface_material_id + " to continue construction of the wall.";
	}



	// Part 2: Validate that a door can be constructed



	// check that there exist requirements for making a door of the specified type
	if (C::DOOR_REQUIREMENTS.find(door_material_id) == C::DOOR_REQUIREMENTS.cend())
	{
		return "You cannot construct a door using " + door_material_id + ".";
	}

	// extract the amount of materials required to make a door of the specified type
	const unsigned DOOR_MATERIAL_COUNT_REQUIRED = C::DOOR_REQUIREMENTS.find(door_material_id)->second;

	// check that the player has the required materials
	if (!this->has(door_material_id, DOOR_MATERIAL_COUNT_REQUIRED))
	{
		// "A stone door requires 5 stone."
		return "A " + door_material_id + " door requires " + U::to_string(DOOR_MATERIAL_COUNT_REQUIRED) + " " + door_material_id + ".";
	}


	// Part 3: Build the surface



	// remove the materials to construct the surface
	this->remove(surface_material_id, C::SURFACE_REQUIREMENTS.find(surface_material_id)->second);

	// add the surface to the room
	world.room_at(x, y, z)->add_surface(surface_id, surface_material_id);



	// Part 4: Add the door to the surface



	// remove the materials to construct the door
	this->remove(door_material_id, C::DOOR_REQUIREMENTS.find(door_material_id)->second);

	// add a door to the surface in the room
	world.room_at(x, y, z)->add_door(surface_id, C::MAX_SURFACE_HEALTH, door_material_id, this->faction_ID);



	// Part 5: the response



	// "You construct a stone floor with a stone hatch." OR "You construct a stone wall to your north with a branch door."
	return "You construct a " + surface_material_id + // you construct a [material]
		((surface_id != C::CEILING && surface_id != C::FLOOR) ?
		" wall to your " + surface_id + " with a " + door_material_id + " door." : // wall to your [direction]
		" " + surface_id + " with a " + door_material_id + " hatch."); // ceiling/floor
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
		U::assign_movement_deltas(surface_ID, new_x, new_y, new_z);
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
		U::assign_movement_deltas(surface_ID, new_x, new_y, new_z);
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
string Character::attack_item(const string & target_ID, World & world)
{
	// if the target isn't here
	if (!world.room_at(x, y, z)->contains_item(target_ID))
	{
		return "There is no " + target_ID + " here.";
	}

	// if the user has an item equipped
	if (equipped_item != nullptr)
	{
		// if the attacking implement is not in the damage tables
		if (C::damage_tables.find(equipped_item->name) == C::damage_tables.cend())
		{
			return "You can't do that with " + U::get_article_for(equipped_item->name) + " " + equipped_item->name + ".";
		}

		// extract the damage table for the attacking implement
		const map<string, int> damage_table = C::damage_tables.find(equipped_item->name)->second;

		// if the damage table does not have an entry for the target item ID
		if (damage_table.find(target_ID) == damage_table.cend())
		{
			return "You can't do that to a " + target_ID + ".";
		}

		// damage the item, return a different message depending of if the item was destroyed or damaged
		if (world.room_at(x, y, z)->damage_item(target_ID, damage_table.find(target_ID)->second))
		{
			return "You destroy the " + target_ID + " using your " + equipped_item->name + ".";
		}
		else
		{
			return "You damage the " + target_ID + " using your " + equipped_item->name + ".";
		}
	}
	else // the user does not have an item equipped, do a barehanded attack
	{
		// extract the damage table for a bare-handed attack
		const map<string, int> damage_table = C::damage_tables.find(C::ATTACK_COMMAND)->second;

		// if the damage table does not contain an entry for the target
		if (damage_table.find(target_ID) == damage_table.cend())
		{
			return "You can't do that to a " + target_ID + ".";
		}

		// the damage table does contain an entry for the target
		if (world.room_at(x, y, z)->damage_item(target_ID, damage_table.find(target_ID)->second))
		{
			return "You destroy the " + target_ID + " using your bare hands.";
		}
		else
		{
			return "You damage the " + target_ID + " using your bare hands.";
		}
	}

}

// movement info
string Character::validate_movement(const int & cx, const int & cy, const int & cz, const string & direction_ID, const int & dx, const int & dy, const int & dz, const World & world) const
{
	// determine if a character can move in a given direction (8 compass points, up, or down)

	// validate direction
	if (!U::contains(C::direction_ids, direction_ID)) { return direction_ID + " is not a direction."; }

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
	// none of four possible walls obstruct path AND an obstructing corner is not formed by adjacent rooms
	else if (
		direction_ID == C::NORTH_WEST || direction_ID == C::NORTH_EAST ||
		direction_ID == C::SOUTH_EAST || direction_ID == C::SOUTH_WEST)
	{
		const unique_ptr<Room>::pointer current_room = world.room_at(cx, cy, cz);
		const unique_ptr<Room>::pointer destination_room = world.room_at(cx + dx, cy + dy, cz);

		if (direction_ID == C::NORTH_WEST)
		{
			if (current_room->has_surface(C::NORTH) || current_room->has_surface(C::WEST) ||
				destination_room->has_surface(C::SOUTH) || destination_room->has_surface(C::EAST) ||
				(world.room_has_surface(cx - 1, cy, cz, C::WEST) && world.room_has_surface(cx, cy - 1, cz, C::NORTH)) ||
				(world.room_has_surface(cx - 1, cy, cz, C::SOUTH) && world.room_has_surface(cx, cy - 1, cz, C::EAST)))
			{
				return "There are walls in your way to the " + direction_ID + ".";
			}
		}
		else if (direction_ID == C::NORTH_EAST)
		{
			if (current_room->has_surface(C::NORTH) || current_room->has_surface(C::EAST) ||
				destination_room->has_surface(C::SOUTH) || destination_room->has_surface(C::WEST) ||
				(world.room_has_surface(cx - 1, cy, cz, C::EAST) && world.room_has_surface(cx, cy + 1, cz, C::NORTH)) ||
				(world.room_has_surface(cx - 1, cy, cz, C::SOUTH) && world.room_has_surface(cx, cy + 1, cz, C::WEST)))
			{
				return "There are walls in your way to the " + direction_ID + ".";
			}
		}
		else if (direction_ID == C::SOUTH_EAST)
		{
			if (current_room->has_surface(C::SOUTH) || current_room->has_surface(C::EAST) ||
				destination_room->has_surface(C::NORTH) || destination_room->has_surface(C::WEST) ||
				(world.room_has_surface(cx + 1, cy, cz, C::EAST) && world.room_has_surface(cx, cy + 1, cz, C::SOUTH)) ||
				(world.room_has_surface(cx + 1, cy, cz, C::NORTH) && world.room_has_surface(cx, cy + 1, cz, C::WEST)))
			{
				return "There are walls in your way to the " + direction_ID + ".";
			}
		}
		else if (direction_ID == C::SOUTH_WEST)
		{
			if (current_room->has_surface(C::SOUTH) || current_room->has_surface(C::WEST) ||
				destination_room->has_surface(C::NORTH) || destination_room->has_surface(C::EAST) ||
				(world.room_has_surface(cx + 1, cy, cz, C::WEST) && world.room_has_surface(cx, cy - 1, cz, C::SOUTH)) ||
				(world.room_has_surface(cx + 1, cy, cz, C::NORTH) && world.room_has_surface(cx, cy - 1, cz, C::EAST)))
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
