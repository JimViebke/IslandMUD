/* Jim Viebke
Jeb 16 2015 */

#include <assert.h>

#include "character.h"
#include "world.h"

Recipes Character::recipes; // constructor calls populate()

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
			equipment_inventory.erase(item_id);
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
	string validate_movement = this->validate_movement(direction_ID, dx, dy, dz, world);

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
			// give the player a new instance of the item
			this->add(Craft::make(it->first));
		}

		// "You now have a(n) sword. " OR "You now have a(n) sword (x5). "
		response += "You now have a(n) " + it->first + string((it->second > 1)
			? (" (x " + it->second + string(")"))
			: "")
			+ ". ";
	}

	return response;
}
string Character::take(const string & take_item_id, World & world)
{
	if (world.room_at(x, y, z)->contains_item(take_item_id)) // if the item is in the player's vicinity
	{
		// if the item is not takable
		if (!world.room_at(x, y, z)->get_contents().find(take_item_id)->second->is_takable)
		{
			// return failure
			return "You can't take the " + take_item_id + ".";
		}

		// the item is takable
		this->add(world.room_at(x, y, z)->get_contents().find(take_item_id)->second); // copy the item to the player
		world.room_at(x, y, z)->remove_item(take_item_id); // remove the item from the world
		return "You take the " + take_item_id + ".";
	}

	if (!Character::recipes.has_recipe_for(take_item_id)) // if the recipe does not exist
	{
		return "There is no " + take_item_id + " nearby."; // report with a message
	}

	// get the recipe
	const Recipe recipe = Character::recipes.get_recipe(take_item_id);

	// verify the conditions for taking an item are present (yielding, required tools, etc)
	for (map<string, int>::const_iterator it = recipe.inventory_need.cbegin(); it != recipe.inventory_need.cend(); ++it)
	{
		if (it->first != "" && this->does_not_have(it->first, it->second)) { return "To take a(n) " + take_item_id + " you need " + ((it->second == 1) ? "a(n)" : R::to_string(it->second)) + " " + it->first + "."; }
	}
	for (map<string, int>::const_iterator it = recipe.inventory_remove.cbegin(); it != recipe.inventory_remove.cend(); ++it)
	{
		if (it->first != "" && this->does_not_have(it->first, it->second)) { return "To take a(n) " + take_item_id + " you need " + ((it->second == 1) ? "a(n)" : R::to_string(it->second)) + " " + it->first + "."; }
	}
	for (map<string, int>::const_iterator it = recipe.local_need.cbegin(); it != recipe.local_need.cend(); ++it)
	{
		if (it->first != "" && !world.room_at(x, y, z)->contains_item(it->first, it->second)) { return "To take a(n) " + take_item_id + " you need " + ((it->second == 1) ? "a" : R::to_string(it->second)) + " nearby " + it->first + "."; }
	}
	for (map<string, int>::const_iterator it = recipe.local_remove.cbegin(); it != recipe.local_remove.cend(); ++it)
	{
		if (it->first != "" && !world.room_at(x, y, z)->contains_item(it->first, it->second)) { return "To take a(n) " + take_item_id + " you need " + ((it->second == 1) ? "a" : R::to_string(it->second)) + " nearby " + it->first + "."; }
	}

	// "take" conditions met, remove items
	for (map<string, int>::const_iterator it = recipe.inventory_remove.cbegin(); it != recipe.inventory_remove.cend(); ++it)
	{
		this->remove(it->first, it->second);
	}
	for (map<string, int>::const_iterator it = recipe.local_remove.cbegin(); it != recipe.local_remove.cend(); ++it)
	{
		world.room_at(x, y, z)->remove_item(it->first, it->second);
	}

	this->add(Craft::make(take_item_id)); // create the specified item and give it to the player
	return "You now have a(n) " + take_item_id + ".";
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
		return ((surface_id == C::CEILING || surface_id == C::FLOOR) ?
			"A " + surface_id + " already exists here." : // ceiling or floor
			"A(n) " + surface_id + " wall already exists here."); // any wall
	}

	// check that the surface to construct is a wall, ceiling, or floor
	if (!R::contains(R::surface_ids, surface_id))
	{
		return "Construct a wall, ceiling or floor.";
	}

	// if the surface is a cailing, check that any wall exists
	if (surface_id == C::CEILING && // the user is construction a ceiling
		!world.room_at(x, y, z)->has_wall()) // the room does not have a wall
	{
		return "You need at least one wall to support a ceiling.";
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
string Character::attack_surface(const string & surface_ID, World & world)
{
	// surface_ID is valid, but may not exist in this room
	
	// if the surface does not exist in this room
	if (!world.room_at(x, y, z)->has_surface(surface_ID))
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

	// the surface exists in this room, damage it




	return "[Character::attack_surface()]";
}