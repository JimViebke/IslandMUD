/* Jim Viebke
Mar 19 2015 */

#include "room.h"

void Room::make_clean()
{
	dirty = false;
	make_container_clean(); // the room's container itself
	if (chest) chest->make_container_clean();
	if (table) table->make_container_clean();
}
bool Room::is_dirty() const
{
	return dirty
		|| is_container_dirty()
		|| (chest && chest->is_container_dirty())
		|| (table && table->is_container_dirty());
}

// room information
bool Room::has_wall() const // used to determine if a ceiling can be placed
{
	return room_sides[(size_t)C::surface::north] ||
		room_sides[(size_t)C::surface::east] ||
		room_sides[(size_t)C::surface::south] ||
		room_sides[(size_t)C::surface::west];
}
bool Room::has_standing_wall() const
{
	return
		(room_sides[(size_t)C::surface::north] && room_sides[(size_t)C::surface::north]->is_intact()) ||
		(room_sides[(size_t)C::surface::east] && room_sides[(size_t)C::surface::east]->is_intact()) ||
		(room_sides[(size_t)C::surface::south] && room_sides[(size_t)C::surface::south]->is_intact()) ||
		(room_sides[(size_t)C::surface::west] && room_sides[(size_t)C::surface::west]->is_intact());
}
bool Room::is_standing_wall(const std::string & surface_ID) const
{
	const C::surface surface = U::to_surface(surface_ID);

	return room_sides[(size_t)surface] && room_sides[(size_t)surface]->is_intact();
}
bool Room::has_surface(const std::string & direction_id) const
{
	return has_surface(U::to_surface(direction_id));
}
bool Room::has_surface(const C::surface surface) const
{
	return room_sides[(size_t)surface].has_value();
}
C::move_attempt Room::can_move_in_direction(const C::direction direction, const std::string & faction_ID) const
{
	// if a surface is present, it must be traversable
	if (has_surface((C::surface)direction))
	{
		return room_sides[(size_t)direction]->is_traversable(faction_ID);
	}
	else
	{
		return C::move_attempt::traversable;
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
bool Room::is_occupied_by(const character_id & actor_id) const
{
	return U::contains(actor_ids, actor_id);
}
bool Room::is_observed_by(const character_id & actor_id) const
{
	return U::contains(viewing_actor_ids, actor_id);
}
bool Room::is_water() const
{
	return water;
}
bool Room::is_forest() const
{
	return this->contains(C::TREE_ID);
}
bool Room::has_non_mineral_deposit_item() const
{
	// for each item in the room
	for (std::multimap<std::string, std::shared_ptr<Item>>::const_iterator it = contents.cbegin(); it != contents.cend(); ++it)
	{
		// if the item is not a mineral deposit
		if (it->first != C::IRON_DEPOSIT_ID && it->first != C::LIMESTONE_DEPOSIT_ID) // find a better way to do this
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
Coordinate Room::get_coordinates() const
{
	return coordinate;
}

void Room::remove(const C::surface surface)
{
	if (has_surface(surface))
	{
		room_sides[(size_t)surface].reset();
		dirty = true;
	}
}

// chests
void Room::add_chest(const std::string & set_faction_id)
{
	chest = std::make_shared<Chest>(set_faction_id);
	dirty = true;
}
bool Room::has_chest() const
{
	return chest != nullptr;
}
std::string Room::get_chest_faction_id() const
{
	// return the chest's faction, or "" if there is no chest (indicating a validation failure in the calling function)
	return (has_chest()) ? chest->get_faction_id() : "";
}
int Room::chest_health() const
{
	// return the chest's health, or 0 if there is no chest (indicating a validation failure in the calling function)
	return (has_chest()) ? chest->get_health() : 0;
}
bool Room::add_item_to_chest(const std::shared_ptr<Item> & item)
{
	dirty = true;
	return chest->insert(item);
}
Update_Messages Room::chest_contents(const std::string & faction_ID, const std::string & username) const
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

	// if the chest is empty
	if (chest->size() == 0) return Update_Messages("There is nothing in the chest.",
		username + " looks into the chest.");

	// return the contents of the chest
	return Update_Messages("In the chest there is" + chest->contents_to_string(),
		username + " looks into the chest.");
}
void Room::damage_chest()
{
	// use equipped weapon and damage tables

	dirty = true;
}
bool Room::chest_has(const std::string & item_id) const
{
	// if there is no hchest here
	if (!has_chest())
	{
		return false;
	}

	return chest->contains(item_id);
}
std::shared_ptr<Chest> Room::get_chest()
{
	return chest;
}
const std::shared_ptr<const Chest> Room::get_chest() const
{
	return chest;
}
void Room::set_chest(const std::shared_ptr<Chest> & set_chest)
{
	this->chest = set_chest;
	dirty = true;
}

// tables
void Room::add_table()
{
	this->table = std::make_shared<Table>();
	dirty = true;
}
bool Room::has_table() const
{
	return this->table != nullptr;
}
bool Room::add_item_to_table(const std::shared_ptr<Item> & item)
{
	return table->insert(item);
}
Update_Messages Room::table_contents(const std::string & username) const
{
	if (!has_table()) return Update_Messages("There is no table here.");

	if (table->size() == 0) return Update_Messages("There is nothing on the table.");

	return Update_Messages("On the table there is" + table->contents_to_string(),
		username + " looks at the table.");
}
bool Room::table_has(const std::string & item_id) const
{
	return table->contains(item_id);
}
std::shared_ptr<Item> Room::remove_from_table(const std::string & item_ID)
{
	return table->erase(item_ID);
}
std::shared_ptr<Table> Room::get_table() const
{
	return table;
}
void Room::set_table(const std::shared_ptr<Table> & set_table)
{
	this->table = set_table;
}

// bloomeries
std::string Room::add_item_to_bloomery(const std::shared_ptr<Forgeable> & item)
{
	// save an iterator
	auto bloomery_it = contents.find(C::BLOOMERY_ID);

	if (bloomery_it == contents.cend())
	{
		return "There is no bloomery here.";
	}

	if (U::is_not<Forgeable>(item))
	{
		return "You cannot put a " + item->get_name() + " in a bloomery.";
	}

	// save a new shared_ptr to the bloomery in question
	std::shared_ptr<Bloomery> bloomery = U::convert_to<Bloomery>(bloomery_it->second);

	bloomery->add_to_bloomery(item);

	return "You place the " + item->get_name() + " in a bloomery.";
}

// items
bool Room::damage_item(const std::string & item_id, const int & amount)
{
	// return a boolean indicating if the target item was destroyed

	dirty = true;

	// if the item will be destroyed
	if (contents.find(item_id)->second->get_health() - amount <= C::DEFAULT_ITEM_MIN_HEALTH)
	{
		// remove the item from the room
		erase(item_id);

		// if the removed item was a tree
		if (item_id == C::TREE_ID)
		{
			// add a log
			insert(Craft::make(C::LOG_ID));
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
void Room::add_surface(const C::surface surface, const std::string & material_ID, const int & surface_health)
{
	// create a surface with a given health (used for loading rooms from disk that may be damaged)

	// This fails silently.
	if (surface == C::surface::not_a_surface) return;

	room_sides[(size_t)surface].emplace(material_ID);
	room_sides[(size_t)surface]->set_health(surface_health);

	dirty = true;
}
void Room::add_door(const C::surface surface, const int & health, const std::string & material_ID, const std::string & faction_ID)
{
	if (!this->has_surface(surface))
	{
		std::stringstream ss;

		ss << "\nAttempted to add a door to a surface that doesn't exist."
			<< "\nsurface_ID=" << U::surface_to_string(surface)
			<< " health=" << health
			<< " material_ID=" << material_ID
			<< " faction_ID=" << faction_ID << std::endl;
		std::cout << ss.str();
		return;
	}

	if (room_sides[(size_t)surface]->has_door())
	{
		std::stringstream ss;
		ss << "\nAttempted to add a door to a surface that already has a door."
			<< "\ndirection_ID=" << U::surface_to_string(surface)
			<< " health=" << health
			<< " material_ID=" << material_ID
			<< " faction_ID=" << faction_ID << std::endl;
		std::cout << ss.str();
		return;
	}

	room_sides[(size_t)surface]->add_door(health, material_ID, faction_ID);

	dirty = true;
}

// damage surface
Update_Messages Room::damage_surface(const std::string & surface_ID, const std::shared_ptr<Item> & equipped_item, const std::string & username)
{
	const auto surface = U::to_surface(surface_ID);

	if (surface == C::surface::not_a_surface)
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
	if (room_sides[(size_t)surface]->is_rubble())
	{
		return Update_Messages("There is only rubble where a wall once was.");
	}

	// extract the ID of the attacking implement (ATTACK_COMMAND for bare-hands melee attack)
	const std::string equipped_item_id = ((equipped_item != nullptr) ? equipped_item->get_name() : C::ATTACK_COMMAND);

	// check if the player's equipped weapon exists in the damage table
	if (C::damage_tables.find(equipped_item_id) == C::damage_tables.cend())
	{
		return Update_Messages("Error occured: Damage lookup tables contain no info to attack using " + equipped_item_id + ".");
	}

	// the damage map for this implement exists, copy it here
	const std::map<std::string, int> item_damage_table = C::damage_tables.find(equipped_item_id)->second;

	// copy the surface's material_ID
	const std::string surface_material_ID = room_sides[(size_t)surface]->get_material_id();

	// check if the material of the wall being attacked exists in the damage table for the implement
	if (item_damage_table.find(surface_material_ID) == item_damage_table.cend())
	{
		// ... contains no info to use staff to damage stone."
		return Update_Messages("Error occured: Damage lookup tables contain no info to use "
			+ equipped_item_id + " to damage "
			+ surface_material_ID + " wall.");
	}

	// surface exists, inflict damage*-1
	{
		std::stringstream ss;
		ss << "surface health before attack: " << room_sides[(size_t)surface]->get_health() << std::endl;
		std::cout << ss.str();
	}
	room_sides[(size_t)surface]->change_health(item_damage_table.find(surface_material_ID)->second*-1);
	{
		std::stringstream ss;
		ss << "surface health after attack:  " << room_sides[(size_t)surface]->get_health() << std::endl;
		std::cout << ss.str();
	}

	dirty = true;

	// after applying damage, test again to see if the surface is rubble
	if (room_sides[(size_t)surface]->is_intact())
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
	room_sides[(size_t)surface]->remove_door();

	// if the collapsing surface is a ceiling
	if (surface == C::surface::ceiling)
	{
		remove(surface);

		// add a debris object to the room
		this->insert(Craft::make(C::DEBRIS_ID));
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
Update_Messages Room::damage_door(const std::string & surface_ID, const std::shared_ptr<Item> & equipped_item, const std::string & username)
{
	const auto surface = U::to_surface(surface_ID);

	if (surface == C::surface::not_a_surface)
	{
		return Update_Messages(surface_ID + " is not a valid surface.");
	}

	// test if this room has the surface
	if (!this->has_surface(surface))
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
	if (!room_sides[(size_t)surface]->has_door())
	{
		return Update_Messages("There is no door here.");
	}

	// extract the ID of the attacking implement (ATTACK_COMMAND for bare-hands melee attack)
	const std::string equipped_item_id = ((equipped_item != nullptr) ? equipped_item->get_name() : C::ATTACK_COMMAND);

	// check if the player's equipped weapon exists in the damage table
	if (C::damage_tables.find(equipped_item_id) == C::damage_tables.cend())
	{
		return Update_Messages("Error occured: Damage lookup tables contain no info to attack using " + equipped_item_id + ".");
	}

	// the damage map for this implement exists, copy it here
	const std::map<std::string, int> item_damage_table = C::damage_tables.find(equipped_item_id)->second;

	// a wall exists; check if the wall does not have an intact door
	if (!room_sides[(size_t)surface]->has_intact_door())
	{
		return Update_Messages("There is only a pile of rubble where a door once was.");
	}

	// a door exists AND it is intact

	// copy the material of the door
	const std::string door_material_ID = room_sides[(size_t)surface]->get_door()->get_material_ID();

	// check if the material of the door being attacked exists in the damage table for the attacking implement
	if (item_damage_table.find(door_material_ID) == item_damage_table.cend())
	{
		// ... contains no info to use staff to damage stone."
		return Update_Messages("Error occured: Damage lookup tables contain no info to use "
			+ equipped_item_id + " to damage "
			+ door_material_ID + " door.");
	}

	// door and damage table entry exist, inflict damage*-1
	{
		std::stringstream ss;
		ss << "door health before attack: " << room_sides[(size_t)surface]->get_door()->get_health() << std::endl; // debugging
		std::cout << ss.str();
	}
	room_sides[(size_t)surface]->get_door()->update_health_by(item_damage_table.find(door_material_ID)->second * -1);
	{
		std::stringstream ss;
		ss << "door health after attack:  " << room_sides[(size_t)surface]->get_health() << std::endl; // debugging
		std::cout << ss.str();
	}

	dirty = true;

	// if the door has 0 health
	if (room_sides[(size_t)surface]->is_rubble())
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
void Room::add_actor(const character_id & actor_id) const
{
	if (!U::contains(actor_ids, actor_id)) // if the actor is not already in the list of actors
	{
		actor_ids.push_back(actor_id); // add the actor
	}
}
void Room::remove_actor(const character_id & actor_id) const
{
	U::erase_element_from_vector(actor_ids, actor_id);
}
void Room::add_viewing_actor(const character_id & actor_id) const
{
	// if the passed actor_ID is not already able to view the room
	if (!U::contains(viewing_actor_ids, actor_id))
	{
		// add the actor ID to viewing_actor_ids
		viewing_actor_ids.push_back(actor_id);
	}
}
void Room::remove_viewing_actor(const character_id & actor_id) const
{
	// A room is unloaded when no player can see the room.
	// To this end, a list of PCs and NPCs who can see this room is maintained.

	U::erase_element_from_vector(viewing_actor_ids, actor_id); // remove the character
}

// printing
std::string Room::summary(const character_id & player_ID) const
{
	std::stringstream summary_stream;

	decltype(room_sides.cbegin()) last_surface_it;
	unsigned surface_count = 0;

	for (auto surface_it = room_sides.cbegin();
		surface_it != room_sides.cend(); ++surface_it)
	{
		if (surface_it->has_value())
		{
			last_surface_it = surface_it;
			++surface_count;
		}
	}

	// if the room has any surfaces
	if (surface_count > 0)
	{
		summary_stream << " This room consists of";

		// for each room side
		C::surface surface = (C::surface)0;
		for (auto surface_it = room_sides.cbegin();
			surface_it != room_sides.cend(); ++surface_it, surface = C::surface((size_t)surface + 1))
		{
			// skip nonexistent walls
			if (!surface_it->has_value()) continue;

			// if this is the last valid surface, print "and"
			if (surface_it == last_surface_it)
			{
				summary_stream << " and";
			}

			// if the surface is not rubble
			if ((*surface_it)->is_intact())
			{
				// "a(n) stone ceiling" or "a stone wall to the west"
				summary_stream << ' ' << U::get_article_for((*surface_it)->get_material_id()) << ' ' << (*surface_it)->get_material_id()
					<< ((surface == C::surface::ceiling || surface == C::surface::floor) ? " " : " wall to the ")
					<< U::surface_to_string(surface); // direction_ID
			}
			else // the surface is rubble
			{
				summary_stream << " a pile of rubble"
					// "on the floor" or " to your north"
					<< ((surface == C::surface::ceiling || surface == C::surface::floor) ? " on the " : " to your ")
					<< U::surface_to_string(surface);
			}

			// add text to the surface's description if it has a door
			if ((*surface_it)->has_door())
			{
				// test if the door is rubble
				if ((*surface_it)->get_door()->is_rubble())
				{
					// "a stone wall to the west..."
					summary_stream << " with a pile of rubble where a door once was";
				}
				else // the door is at least partially intact
				{
					// "a stone wall to the west" becomes
					// "a stone wall to the west with a wood door"
					summary_stream << " with a " << (*surface_it)->get_door()->get_material_ID() << " door";
				}
			}

			// if the current surface is not the last AND there are more than 2 sides, append a comma
			if (surface_it != last_surface_it && surface_count > 2)
			{
				summary_stream << ',';
			}
		}

		summary_stream << '.'; // end with a period
	}

	// report on the items in the room
	if (contents.size() > 0) // if there are items present
	{
		summary_stream << " Here there is" << this->contents_to_string();
	}

	// if the room contains a table or chest
	if (has_table() && has_chest())
	{
		summary_stream << " There is a table and a chest here.";
	}
	else if (has_table())
	{
		summary_stream << " There is a table here.";
	}
	else if (has_chest())
	{
		summary_stream << " There is a chest here.";
	}

	if (actor_ids.size() > 1) // there will always be at least one, because of the player
	{
		summary_stream << " ";
		for (const character_id& actor_ID : actor_ids)
		{
			if (actor_ID == player_ID) continue; // don't report a player's own presence to that player
			summary_stream << actor_ID << " is here.";
		}
	}

	// if there are no items or surfaces
	if (summary_stream.str().length() == 0)
	{
		summary_stream << " There is nothing of interest here.";
	}

	// end with a newline
	summary_stream << "\n";

	return summary_stream.str();
}

bool operator==(std::unique_ptr<Room> & lhs, std::unique_ptr<Room> & rhs)
{
	return lhs->coordinate == rhs->coordinate;
}
