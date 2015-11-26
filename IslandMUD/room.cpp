/* Jim Viebke
Mar 19 2015 */

#include "room.h"

using std::cout; // fix VS ambiguity bug

// room information
bool Room::has_wall() const // used to determine if a ceiling can be placed
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
bool Room::has_standing_wall() const
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
		// if the surface is an intact wall
		if (it->second.is_intact() && (
			it->first == C::NORTH || it->first == C::EAST ||
			it->first == C::SOUTH || it->first == C::WEST))
		{
			return true;
		}
	}

	// the room does not have a wall
	return false;
}
bool Room::is_standing_wall(const string & surface_ID) const
{
	// check if there are no sides OR surface_ID is not a room side
	if (room_sides.size() == 0 ||
		!(surface_ID == C::NORTH || surface_ID == C::SOUTH || surface_ID == C::WEST || surface_ID == C::EAST))
	{
		return false;
	}

	// check if the surface exists
	if (!this->has_surface(surface_ID))
	{
		return false;
	}

	// return "is the surface intact?"
	return (room_sides.find(surface_ID)->second.is_intact());
}
bool Room::has_surface(const string & direction_id) const
{
	return room_sides.find(direction_id) != room_sides.cend();
}
string Room::can_move_in_direction(const string & direction_ID, const string & faction_ID)
{
	// if a surface is present
	if (has_surface(direction_ID))
	{
		// a player can move through the surface if it is rubble

		if (room_sides.find(direction_ID)->second.is_rubble())
		{
			return C::GOOD_SIGNAL;
		}

		return room_sides.find(direction_ID)->second.can_move_through_wall(faction_ID);
	}
	else // a surface does not exist...
	{
		// ...so the player is free to move
		return C::GOOD_SIGNAL;
	}
}
bool Room::contains_no_items() const
{
	return contents.size() == 0;
}
bool Room::is_unloadable() const
{
	return actor_ids.size() == 0 && viewing_actor_ids.size() == 0;
}
bool Room::contains_item(const string & item_id) const
{
	return contents.find(item_id) != contents.cend();
}
bool Room::contains_item(const string & item_id, const unsigned & count) const
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

	// if the item is a material, attempt to extract it
	const multimap<string, shared_ptr<Item>>::const_iterator it = contents.find(item_id);
	if (it != contents.cend()) // if the item exist
	{
		// attempt to convert the item to a material type
		if (shared_ptr<Material> material_item = U::convert_to<Material>(it->second))
		{
			// if the amount is greater than or equal to count
			if (material_item->amount >= count)
			{
				return true; // success
			}
		}
	}

	// there is not a sufficient count of items in the room
	return false;
}
bool Room::is_observed_by(const string & actor_id) const
{
	return U::contains(viewing_actor_ids, actor_id);
}
bool Room::is_water() const
{
	return water;
}
bool Room::is_forest() const
{
	return this->contains_item(C::TREE_ID);
}
bool Room::has_non_mineral_deposit_item() const
{
	// for each item in the room
	for (multimap<string, shared_ptr<Item>>::const_iterator it = contents.cbegin(); it != contents.cend(); ++it)
	{
		// if the item is not a mineral deposit
		if (it->first != C::IRON_DEPOSIT_ID && it->first != C::LIMESTONE_DEPOSIT_ID)
		{
			// an item is not a mineral deposit
			return true;
		}
	}

	// any item found was a mineral deposit
	return false;
}
bool Room::has_mineral_deposit() const
{
	// determine if a room contains an iron or limestone deposit
	return contents.find(C::IRON_DEPOSIT_ID) != contents.cend() ||
		contents.find(C::LIMESTONE_DEPOSIT_ID) != contents.cend();
}

// chests
void Room::add_chest(const string & set_faction_id)
{
	updated = true;

	chest = make_shared<Chest>(set_faction_id);
}
bool Room::has_chest() const
{
	return chest != nullptr;
}
string Room::get_chest_faction_id() const
{
	// return the chest's faction, or "" if there is no chest (indicating a validation failure in the calling function)
	return (has_chest()) ? chest->get_faction_id() : "";
}
int Room::chest_health() const
{
	// return the chest's health, or 0 if there is no chest (indicating a validation failure in the calling function)
	return (has_chest()) ? chest->get_health() : 0;
}
void Room::add_item_to_chest(const shared_ptr<Item> & item)
{
	updated = true;

	chest->add(item);
}
Update_Messages Room::chest_contents(const string & faction_ID, const string & username) const
{
	// if no chest exists in this room
	if (!has_chest())
	{
		return Update_Messages("There is no chest here.");
	}

	// if the chest was crafted by another faction
	if (chest->get_faction_id() != faction_ID)
	{
		return Update_Messages("This chest has an unfamiliar lock.",
			username + " tries the chest's lock.");
	}

	// return the contents of the chest
	return Update_Messages(chest->contents(), username + " looks into the chest.");
}
void Room::damage_chest()
{
	updated = true;

	// use equipped weapon and damage tables


}
bool Room::chest_has(const string & item_id) const
{
	// if there is no hchest here
	if (!has_chest())
	{
		return false;
	}

	return chest->has(item_id);
}
shared_ptr<Item> Room::remove_from_chest(const string & item_id)
{
	updated = true;

	// manifest a stone if a chest does not exist
	// (this would indicate an validation failure in the calling function)
	if (!has_chest())
	{
		return Craft::make(C::STONE_ID);
	}

	return chest->take(item_id);
}
shared_ptr<Chest> Room::get_chest() const
{
	return chest;
}
void Room::set_chest(const shared_ptr<Chest> & set_chest)
{
	// only used at load time, so the "update" flag will not be set to true

	this->chest = set_chest;
}

// items
void Room::add_item(const shared_ptr<Item> item) // pass a copy rather than a reference
{
	/* This doesn't stack materials.
	This could easily be fixed, but room saving/unloading doesn't use item counts.
	Alternatively, treat the symptoms and just fix the printout
	*/

	contents.insert(pair<string, shared_ptr<Item>>(item->name, item));
	updated = true;
}
void Room::remove_item(const string & item_id, const int & count)
{
	for (int i = 0; i < count; ++i) // for as many items as are to be removed
	{
		contents.erase(contents.find(item_id)); // remove the item
	}

	updated = true;
}
bool Room::damage_item(const string & item_id, const int & amount)
{
	// return a boolean indicating if the target item was destroyed

	// if the item will be destroyed
	if (contents.find(item_id)->second->get_health() - amount <= C::DEFAULT_ITEM_MIN_HEALTH)
	{
		// remove the item from the room
		remove_item(item_id);

		// if the removed item was a tree
		if (item_id == C::TREE_ID)
		{
			// add a log
			add_item(Craft::make(C::LOG_ID));
		}

		return true;
	}
	else // the item will not be destroyed, just reduce its health
	{
		contents.find(item_id)->second->update_health(amount);

		return false;
	}
}

// add surfaces and doors
void Room::add_surface(const string & surface_ID, const string & material_ID)
{
	// if the surface ID is valid
	if (U::contains(C::surface_ids, surface_ID))
	{
		// create a new Room_Side and add it to room_sides
		room_sides.insert(pair<string, Room_Side>(surface_ID, Room_Side(material_ID)));
		updated = true;
	}
}
void Room::add_surface(const string & surface_ID, const string & material_ID, const int & surface_health)
{
	// create a surface with a given health (used for loading rooms from disk that may be damaged)

	// if the surface ID is valid
	if (U::contains(C::surface_ids, surface_ID))
	{
		// create a new Room_Side and add it to room_sides
		room_sides.insert(pair<string, Room_Side>(surface_ID, Room_Side(material_ID)));

		// select the surface and set its health to the passed value
		room_sides.find(surface_ID)->second.set_health(surface_health);

		updated = true;
	}
}
void Room::add_door(const string & directon_ID, const int & health, const string & material_ID, const string & faction_ID)
{
	if (!this->has_surface(directon_ID))
	{
		cout << "\nAttempted to add a door to a surface that doesn't exist.";
		cout << "\ndirection_ID=" << directon_ID
			<< " health=" << health
			<< " material_ID=" << material_ID
			<< " faction_ID=" << faction_ID << endl;
		return;
	}

	if (room_sides.find(directon_ID)->second.get_door() != nullptr)
	{
		cout << "\nAttempted to add a door to a surface that already has a door.";
		cout << "\ndirection_ID=" << directon_ID
			<< " health=" << health
			<< " material_ID=" << material_ID
			<< " faction_ID=" << faction_ID << endl;
		return;
	}

	// delegate the work to the room's surface
	room_sides.find(directon_ID)->second.add_door(health, material_ID, faction_ID);

	updated = true;
}

// damage surface
Update_Messages Room::damage_surface(const string & surface_ID, const shared_ptr<Item> & equipped_item, const string & username)
{
	// test if the surface is valid
	if (!U::contains(C::surface_ids, surface_ID))
	{
		return Update_Messages(surface_ID + " is not a valid surface.");
	}

	// test if this room has the surface
	if (!this->has_surface(surface_ID))
	{
		if (surface_ID == C::CEILING || surface_ID == C::FLOOR)
		{
			return Update_Messages("There is no " + surface_ID + ".");
		}
		else
		{
			return Update_Messages("There is no " + surface_ID + " wall here.");
		}
	}

	// the surface exists, test if the surface is rubble
	if (room_sides.find(surface_ID)->second.is_rubble())
	{
		return Update_Messages("There is only rubble where a wall once was.");
	}

	// extract the ID of the attacking implement (ATTACK_COMMAND for bare-hands melee attack)
	const string equipped_item_id = ((equipped_item != nullptr) ? equipped_item->name : C::ATTACK_COMMAND);

	// check if the player's equipped weapon exists in the damage table
	if (C::damage_tables.find(equipped_item_id) == C::damage_tables.cend())
	{
		return Update_Messages("Error occured: Damage lookup tables contain no info to attack using " + equipped_item_id + ".");
	}

	// the damage map for this implement exists, copy it here
	const map<string, int> item_damage_table = C::damage_tables.find(equipped_item_id)->second;

	// copy the surface's material_ID
	const string surface_material_ID = this->room_sides.find(surface_ID)->second.get_material_id();

	// check if the material of the wall being attacked exists in the damage table for the implement
	if (item_damage_table.find(surface_material_ID) == item_damage_table.cend())
	{
		// ... contains no info to use staff to damage stone."
		return Update_Messages("Error occured: Damage lookup tables contain no info to use "
			+ equipped_item_id + " to damage "
			+ surface_material_ID + " wall.");
	}

	// surface exists, inflict damage*-1
	cout << "surface health before attack: " << room_sides.find(surface_ID)->second.get_health() << endl;
	room_sides.find(surface_ID)->second.change_health(item_damage_table.find(surface_material_ID)->second*-1);
	cout << "surface health after attack:  " << room_sides.find(surface_ID)->second.get_health() << endl;

	updated = true; // the room has been modified since it was loaded

	// after applying damage, test again to see if the surface is rubble
	if (room_sides.find(surface_ID)->second.is_intact())
	{
		// the surface holds

		return Update_Messages("You damage the " + // You damage the
			// ceiling / wall to your west
			((surface_ID == C::CEILING || surface_ID == C::FLOOR) ? surface_ID : "wall to your " + surface_ID)
			// using your sword. / using your bare hands.
			+ " using your " +
			((equipped_item_id == C::ATTACK_COMMAND) ? "bare hands." : (equipped_item_id + ".")),

			// Bob uses
			username + " uses " +
			((equipped_item_id == C::ATTACK_COMMAND)
			// bare hands / an axe
			? "bare hands" : U::get_article_for(equipped_item_id) + " " + equipped_item_id) +
			// to damage the
			" to damage the " +
			// ceiling. / wall to your west.
			((surface_ID == C::CEILING || surface_ID == C::FLOOR) ? surface_ID : "wall to your " + surface_ID) + ".");
	}

	// the surface collapses

	// ensure the surface no longer has a door
	room_sides.find(surface_ID)->second.remove_door();

	// if the collapsing surface is a ceiling
	if (surface_ID == C::CEILING)
	{
		// remove the ceiling object completely
		room_sides.erase(surface_ID);

		// add a debris object to the room
		this->add_item(Craft::make(C::DEBRIS_ID));
	}

	return Update_Messages(
		(surface_ID == C::CEILING || surface_ID == C::FLOOR)
		? "The " + surface_ID + " collapses."
		: "The wall collapses.",

		// Bob collapses the
		username + " collapses the " +
		((surface_ID == C::CEILING || surface_ID == C::FLOOR)
		// ceiling/floor
		? surface_ID + "."
		// wall to your west
		: "wall to your " + surface_ID + "."),
		true);
}
Update_Messages Room::damage_door(const string & surface_ID, const shared_ptr<Item> & equipped_item, const string & username)
{
	// test if the surface is valid
	if (!U::contains(C::surface_ids, surface_ID))
	{
		return Update_Messages(surface_ID + " is not a valid surface.");
	}

	// test if this room has the surface
	if (!this->has_surface(surface_ID))
	{
		if (surface_ID == C::CEILING || surface_ID == C::FLOOR)
		{
			return Update_Messages("There is no " + surface_ID + ".");
		}
		else
		{
			return Update_Messages("There is no " + surface_ID + " wall here.");
		}
	}

	// test if the surface has the door
	if (!this->room_sides.find(surface_ID)->second.has_door())
	{
		return Update_Messages("There is no door here.");
	}

	// extract the ID of the attacking implement (ATTACK_COMMAND for bare-hands melee attack)
	const string equipped_item_id = ((equipped_item != nullptr) ? equipped_item->name : C::ATTACK_COMMAND);

	// check if the player's equipped weapon exists in the damage table
	if (C::damage_tables.find(equipped_item_id) == C::damage_tables.cend())
	{
		return Update_Messages("Error occured: Damage lookup tables contain no info to attack using " + equipped_item_id + ".");
	}

	// the damage map for this implement exists, copy it here
	const map<string, int> item_damage_table = C::damage_tables.find(equipped_item_id)->second;

	// a wall exists; check if the wall does not have an intact door
	if (!this->room_sides.find(surface_ID)->second.has_intact_door())
	{
		return Update_Messages("There is only a pile of rubble where a door once was.");
	}

	// a door exists AND it is intact

	// copy the material of the door
	const string door_material_ID = this->room_sides.find(surface_ID)->second.get_door()->get_material_ID();

	// check if the material of the door being attacked exists in the damage table for the attacking implement
	if (item_damage_table.find(door_material_ID) == item_damage_table.cend())
	{
		// ... contains no info to use staff to damage stone."
		return Update_Messages("Error occured: Damage lookup tables contain no info to use "
			+ equipped_item_id + " to damage "
			+ door_material_ID + " door.");
	}

	// door and damage table entry exist, inflict damage*-1
	cout << "door health before attack: " << room_sides.find(surface_ID)->second.get_door()->get_health() << endl; // debugging
	room_sides.find(surface_ID)->second.get_door()->update_health_by(item_damage_table.find(door_material_ID)->second * -1);
	cout << "door health after attack:  " << room_sides.find(surface_ID)->second.get_door()->get_health() << endl; // debugging

	updated = true; // the room has now been modified since it was loaded

	// if the door has 0 health
	if (room_sides.find(surface_ID)->second.get_door()->is_rubble())
	{
		// the door collapses
		return Update_Messages("The door breaks and collapses.", username + " breaks the door.", true);
	}
	else
	{
		// the door holds
		return Update_Messages("You damage the " + surface_ID + " door using your " +
			((equipped_item_id == C::ATTACK_COMMAND) ? "bare hands." : (equipped_item_id + ".")),

			// Bob damages the west door with
			username + " damages the " + surface_ID + " door with " +
			((equipped_item_id == C::ATTACK_COMMAND)
			// bare hands. / an axe.
			? "bare hands." : U::get_article_for(equipped_item_id) + " " + equipped_item_id + "."),

			true);
	}
}

// add and remove actors
void Room::add_actor(const string & actor_id)
{
	if (!U::contains(actor_ids, actor_id)) // if the actor is not already in the list of actors
	{
		actor_ids.push_back(actor_id); // add the actor
	}
}
void Room::remove_actor(const string & actor_id)
{
	if (U::contains(actor_ids, actor_id)) // if the character exists here
	{
		U::erase_element_from_vector(actor_ids, actor_id);
	}
}
void Room::add_viewing_actor(const string & actor_id)
{
	// if the passed actor_ID is not already able to view the room
	if (!U::contains(viewing_actor_ids, actor_id))
	{
		// add the actor ID to viewing_actor_ids
		viewing_actor_ids.push_back(actor_id);
	}
}
void Room::remove_viewing_actor(const string & actor_id)
{
	// A room is unloaded when no player can see the room.
	// To this end, a list of PCs and NPCs who can see this room is maintained.

	if (U::contains(viewing_actor_ids, actor_id)) // if the character can see this room
	{
		U::erase_element_from_vector(viewing_actor_ids, actor_id); // remove the character
	}
}

// printing
string Room::summary(const string & player_ID) const
{
	stringstream summary_stream;

	// report on the sides of the room (walls, ceiling, floor)
	if (room_sides.size() > 0)
	{
		summary_stream << "\n\nThis room consists of";

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

			// if the surface is not rubble
			if (!it->second.is_rubble())
			{
				// "a(n) stone ceiling" or "a stone wall to the west"
				summary_stream << " " << U::get_article_for(it->second.get_material_id()) << " " << it->second.get_material_id()
					<< ((it->first == C::CEILING || it->first == C::FLOOR) ? " " : " wall to the ")
					<< it->first; // direction_ID
			}
			else // the surface is rubble
			{
				summary_stream << " a pile of rubble"
					// "on the floor" or " to your north"
					<< ((it->first == C::CEILING || it->first == C::FLOOR) ? " on the " : " to your ")
					<< it->first;
			}

			// add text to the surface's description if it has a door
			if (it->second.has_door())
			{
				// test if the door is rubble
				if (it->second.get_door()->is_rubble())
				{
					// "a stone wall to the west..."
					summary_stream << " with a pile of rubble where a door once was";
				}
				else // the door is at least partially intact
				{
					// "a stone wall to the west" becomes
					// "a stone wall to the west with a wood door"
					summary_stream << " with a " << it->second.get_door()->get_material_ID() << " door";
				}
			}

			// if the current surface is not the last AND there are more than 2 sides, append a comma
			if (it != last_side_it && room_sides.size() > 2)
			{
				summary_stream << ",";
			}
		}

		summary_stream << "."; // end with a period
	}

	// report on the items in the room
	if (contents.size() > 0) // if there are items present
	{
		// create a map of <item id, item count>
		map<string, int> stacked_contents;
		for (multimap<string, shared_ptr<Item>>::const_iterator it = contents.cbegin();
			it != contents.cend(); ++it)
		{
			stacked_contents[it->first]++;
		}

		// save an iterator to the last item
		const map<string, int>::const_iterator last_item_it = --stacked_contents.cend();

		summary_stream << "\n\nHere there is";
		// for each item
		for (map<string, int>::const_iterator item_it = stacked_contents.cbegin();
			item_it != stacked_contents.cend(); ++item_it)
		{
			// if there is more than one instance of the item here
			if (item_it->second > 1)
			{
				// append the item_id followed by the count
				summary_stream << " " << item_it->first << " (x" << item_it->second << ")";
			}
			else
			{
				// don't append the count
				summary_stream << " " << U::get_article_for(item_it->first) << " " << item_it->first;
			}

			// conditionally append a comma
			summary_stream << ((item_it == last_item_it) ? "" : ",");
		}

		summary_stream << ".";
	}

	// if the room contains a chest
	if (has_chest())
	{
		// append a sentence to the current paragraph
		summary_stream << " There is a chest here.";
	}

	if (actor_ids.size() > 1)
	{
		summary_stream << "\n\n";
		for (const string & actor_ID : actor_ids)
		{
			if (actor_ID == player_ID) { continue; } // skip the player themself.
			summary_stream << actor_ID << " is here. ";
		}
	}

	// if there are no items or surfaces
	if (summary_stream.str().length() == 0)
	{
		summary_stream << "\n\nThere is nothing of interest here.";
	}

	return summary_stream.str();
}
