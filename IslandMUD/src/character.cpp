/* Jim Viebke
Jeb 16 2015 */

#include <algorithm> // for std::max

#include "constants.h"
#include "character.h"
#include "game.h"
#include "world.h"

std::unique_ptr<Recipes> Character::recipes;

Character::Character(const std::string & name, const std::string & set_faction_ID, std::observer_ptr<Game> game) : name(name), location(C::DEFAULT_SPAWN_X, C::DEFAULT_SPAWN_Y), game(game), world(game->world.get()), actors(&game->actors)
{
	if (Character::recipes == nullptr)
	{
		Character::recipes = std::make_unique<Recipes>();
	}

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
		std::stringstream ss;
		ss << "ERROR: attempted to create character with invalid faction: [" << set_faction_ID << "]\n";
		std::cout << ss.str();
	}

	id = U::next_int();

	login();

	std::stringstream ss;
	ss << "Signed in " << name << " with ID=" << get_ID() << std::endl;
	std::cout << ss.str();
}
Character::~Character() {}

void Character::login()
{
	// create a document to load the player's data
	pugi::xml_document user_data_xml;

	// load the player's data to user_data_xml
	user_data_xml.load_file((C::user_data_directory + "/" + this->name + ".xml").c_str());



	// create holder values to save the coordinates from the file
	int loaded_x = -1, loaded_y = -1;

	// load the three values from the node
	const pugi::xml_node location_node = user_data_xml.child(C::XML_USER_LOCATION.c_str());

	// extract the attributes as well as the values for the attributes
	const pugi::xml_attribute x_attribute = location_node.attribute(std::string("x").c_str());
	const pugi::xml_attribute y_attribute = location_node.attribute(std::string("y").c_str());

	Coordinate loaded_cooardinates(x_attribute.as_int(), y_attribute.as_int());

	// if any of the attributes are empty or the extracted values fail bounds-checking
	if (x_attribute.empty() || y_attribute.empty() ||
		!loaded_cooardinates.is_valid())
	{
		// set the player to the default spawn
		location = Coordinate(C::DEFAULT_SPAWN_X, C::DEFAULT_SPAWN_Y);
	}
	else
	{
		// set the player to the valid loaded coordinates
		location = loaded_cooardinates;
	}

	// load the rooms around the player's spawn
	world->load_view_radius_around(location, id);

	// spawn in the player
	world->room_at(location)->add_actor(id);

	// select the level node
	const pugi::xml_node level_node = user_data_xml.child(C::XML_USER_LEVELS.c_str());

	// select each level attribute
	const pugi::xml_attribute swordsmanship_level_attribute = level_node.attribute(C::XML_LEVEL_SWORDSMANSHIP.c_str());
	const pugi::xml_attribute archery_level_attribute = level_node.attribute(C::XML_LEVEL_ARCHERY.c_str());
	const pugi::xml_attribute forest_visibility_level_attribute = level_node.attribute(C::XML_LEVEL_FOREST_VISIBILITY.c_str());
	const pugi::xml_attribute full_health_level_attribute = level_node.attribute(C::XML_LEVEL_HEALTH_MAX.c_str());

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
	const pugi::xml_node status_node = user_data_xml.child(C::XML_USER_STATUS.c_str());

	// if current_health is non-empty, set current health
	const pugi::xml_attribute current_health_attribute = status_node.attribute(C::XML_USER_STATUS_CURRENT_HEALTH.c_str());
	if (!current_health_attribute.empty())
	{
		this->set_current_health(current_health_attribute.as_int());
	}
	else // if current_health is empty, explicitly set current health to base maximum
	{
		this->set_current_health(C::FULL_HEALTH_MIN);
	}


	// for each item node of the items node
	for (const pugi::xml_node & item_node : user_data_xml.child(C::XML_ITEM.c_str()).children())
	{
		// create the item
		std::shared_ptr<Item> item = (Craft::make(item_node.name()));

		// extract the value of the health attribute and use it to set the item's health
		item->set_health(item_node.attribute(C::XML_ITEM_HEALTH.c_str()).as_int());

		// attempt to extract the count attribute (for stackable items)
		pugi::xml_attribute count_attribute = item_node.attribute(C::XML_ITEM_COUNT.c_str());
		if (!count_attribute.empty())
		{
			if (std::shared_ptr<Stackable> stackable = U::convert_to<Stackable>(item))
			{
				stackable->amount = std::max(count_attribute.as_uint(), 1u);
			}
		}

		// add the item to the player's inventory
		this->insert(item);
	}
}

Character::ID Character::get_ID() const
{
	return id;
}

Update_Messages Character::save()
{
	// if an item is equipped, move it back to the player's inventory
	this->unequip();

	// load the existing document to save the user's info
	pugi::xml_document user_data_xml;
	user_data_xml.load_file((C::user_data_directory + "/" + this->name + ".xml").c_str());

	// erase the nodes we want to overwrite
	user_data_xml.remove_child(C::XML_USER_STATUS.c_str());
	user_data_xml.remove_child(C::XML_USER_LOCATION.c_str());
	user_data_xml.remove_child(C::XML_USER_LEVELS.c_str());
	user_data_xml.remove_child(C::XML_ITEM.c_str());

	// create nodes to store user equipment and materials
	pugi::xml_node status_node = user_data_xml.append_child(C::XML_USER_STATUS.c_str());
	pugi::xml_node location_node = user_data_xml.append_child(C::XML_USER_LOCATION.c_str());
	pugi::xml_node level_node = user_data_xml.append_child(C::XML_USER_LEVELS.c_str());
	pugi::xml_node items_node = user_data_xml.append_child(C::XML_ITEM.c_str());

	// add health attribute to status node
	status_node.append_attribute(C::XML_USER_STATUS_CURRENT_HEALTH.c_str()).set_value(this->current_health);

	// add and y attributes to the location node
	location_node.append_attribute(std::string("x").c_str()).set_value(location.get_x());
	location_node.append_attribute(std::string("y").c_str()).set_value(location.get_y());

	// add each level to the location node
	level_node.append_attribute(C::XML_LEVEL_SWORDSMANSHIP.c_str()).set_value(this->swordsmanship_level);
	level_node.append_attribute(C::XML_LEVEL_ARCHERY.c_str()).set_value(this->archery_level);
	level_node.append_attribute(C::XML_LEVEL_FOREST_VISIBILITY.c_str()).set_value(this->forest_visibility_level);
	level_node.append_attribute(C::XML_LEVEL_HEALTH_MAX.c_str()).set_value(this->max_health);

	// for each item in the user's inventory
	for (std::multimap<std::string, std::shared_ptr<Item>>::const_iterator it = contents.cbegin();
		it != contents.cend(); ++it)
	{
		// save the item to a new node under the items node
		pugi::xml_node item_node = items_node.append_child(it->first.c_str());

		// if the item is stackable
		if (std::shared_ptr<Stackable> stackable = U::convert_to<Stackable>(it->second))
		{
			// add count="[amount]" to the node's attributes
			item_node.append_attribute(C::XML_ITEM_COUNT.c_str()).set_value(stackable->amount);
		}

		// append a health attribute to the item node and set its value to the health of the item
		item_node.append_attribute(C::XML_ITEM_HEALTH.c_str()).set_value(it->second->get_health());
	}

	// save the user_data to disk
	user_data_xml.save_file((C::user_data_directory + "/" + this->name + ".xml").c_str()); // returns an unused boolean

	return Update_Messages("Player info saved to disk.");
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
void Character::reset_health()
{
	current_health = max_health;
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
bool Character::does_not_have(const std::string & item_name, const unsigned & item_count) const
{
	return !this->contains(item_name, item_count);
}
std::string Character::get_inventory() const // debugging
{
	return contents_to_string();
}

// actions
Update_Messages Character::move(const std::string & direction)
{
	return move(U::to_direction(direction));
}
Update_Messages Character::move(const C::direction & direction)
{
	const Coordinate destination = location.get_after_move(direction);

	const int x = location.get_x(), y = location.get_y();

	// validate movement deltas
	if (!destination.is_valid())
	{
		return Update_Messages("You can't go there.");
	}

	// test if the environment (structures) allow the player to move in a given direction
	const C::move_attempt validate_movement = this->validate_movement(location, direction, destination);

	// if the validation failed for any reason
	if (validate_movement != C::move_attempt::traversable)
	{
		switch (validate_movement)
		{
		case C::move_attempt::not_a_direction:
			return Update_Messages(U::direction_to_string(direction) + " is not a direction.");
		case C::move_attempt::unfamiliar_lock:
			return Update_Messages("This door has an unfamiliar lock.");
		case C::move_attempt::blocked_by_wall:
			return Update_Messages("There is a wall in the way to your " + U::direction_to_string(direction) + ".");
		case C::move_attempt::indirectly_blocked_by_wall:
			return Update_Messages("There are walls in the way to your " + U::direction_to_string(direction) + ".");
		default:
			return Update_Messages("Could not move " + U::direction_to_string(direction) + " for an unkown reason.");
		}
	}

	// the movement validated, load the radius for the destination
	world->load_view_radius_around(destination, id);

	// maintain a list of users that are falling out of view that will also need a map update
	std::vector<character_id> additional_users_to_notify;

	// remove viewing ID from rooms leaving view
	if (direction == C::direction::north || direction == C::direction::south)
	{
		// if the character is moving north, add the view distance to x to get the x of the row being removed
		// otherwise (moving south) remove the distance from x
		int rx = (direction == C::direction::north) ? x + C::VIEW_DISTANCE : x - C::VIEW_DISTANCE;
		// each room to try unload from from x,(y-view) to (x,y+view)
		for (int ry = y - C::VIEW_DISTANCE; ry <= y + C::VIEW_DISTANCE; ++ry)
		{
			Coordinate unload_location(rx, ry);

			// Skip this room if it is not loaded. This occurs when a player moves diagonally, and both room unload passes overlap at the corner of the map.
			if (!unload_location.is_valid() || world->room_at(unload_location) == nullptr) continue;

			U::append_b_to_a(additional_users_to_notify, world->room_at(unload_location)->get_actor_ids()); // save any users in the room

			// remove the character from the room's viewer list, trying to unload the room in the process
			world->remove_viewer_and_attempt_unload(unload_location, id); // bounds checking takes place in here
		}
	}
	else if (direction == C::direction::west || direction == C::direction::east)
	{
		// logic is the same as above, but in rotated axes (axes is plural of axis (themoreyouknow.gif))
		const int ry = (direction == C::direction::west) ? y + C::VIEW_DISTANCE : y - C::VIEW_DISTANCE;
		for (int rx = x - C::VIEW_DISTANCE; rx <= x + C::VIEW_DISTANCE; ++rx)
		{
			Coordinate unload_location(rx, ry);
			if (!unload_location.is_valid() || world->room_at(unload_location) == nullptr) continue;
			U::append_b_to_a(additional_users_to_notify, world->room_at(unload_location)->get_actor_ids());
			world->remove_viewer_and_attempt_unload(unload_location, id);
		}
	}
	else
	{
		/* The direction is a secondary compass direction.
		This means execution will alwways enter two of the four below blocks.
		Functionality here is the same as above. For documentation, scroll up. */

		if (direction == C::direction::north_west || direction == C::direction::north_east) // moving north, parse south row
		{
			const int rx = x + C::VIEW_DISTANCE;
			for (int ry = y - C::VIEW_DISTANCE; ry <= y + C::VIEW_DISTANCE; ++ry)
			{
				Coordinate unload_location(rx, ry);
				if (!unload_location.is_valid() || world->room_at(unload_location) == nullptr) continue;
				U::append_b_to_a(additional_users_to_notify, world->room_at(unload_location)->get_actor_ids());
				world->remove_viewer_and_attempt_unload(unload_location, id);
			}
		}

		if (direction == C::direction::north_east || direction == C::direction::south_east) // moving east, parse west row
		{
			const int ry = y - C::VIEW_DISTANCE;
			for (int rx = x - C::VIEW_DISTANCE; rx <= x + C::VIEW_DISTANCE; ++rx)
			{
				Coordinate unload_location(rx, ry);
				if (!unload_location.is_valid() || world->room_at(unload_location) == nullptr) continue;
				U::append_b_to_a(additional_users_to_notify, world->room_at(unload_location)->get_actor_ids());
				world->remove_viewer_and_attempt_unload(unload_location, id);
			}
		}

		if (direction == C::direction::south_east || direction == C::direction::south_west) // moving south, parse north row
		{
			const int rx = x - C::VIEW_DISTANCE;
			for (int ry = y - C::VIEW_DISTANCE; ry <= y + C::VIEW_DISTANCE; ++ry)
			{
				Coordinate unload_location(rx, ry);
				if (!unload_location.is_valid() || world->room_at(unload_location) == nullptr) continue;
				U::append_b_to_a(additional_users_to_notify, world->room_at(unload_location)->get_actor_ids());
				world->remove_viewer_and_attempt_unload(unload_location, id);
			}
		}

		if (direction == C::direction::south_west || direction == C::direction::north_west) // moving west, parse east row
		{
			const int ry = y + C::VIEW_DISTANCE;
			for (int rx = x - C::VIEW_DISTANCE; rx <= x + C::VIEW_DISTANCE; ++rx)
			{
				Coordinate unload_location(rx, ry);
				if (!unload_location.is_valid() || world->room_at(unload_location) == nullptr) continue;
				U::append_b_to_a(additional_users_to_notify, world->room_at(unload_location)->get_actor_ids());
				world->remove_viewer_and_attempt_unload(unload_location, id);
			}
		}
	}

	// the movement validated, remove character id from area
	world->room_at(location)->remove_actor(id);

	// actually move the character
	location = destination;

	// add character id to new area using the new coordinates
	world->room_at(location)->add_actor(id);

	// prepare responses
	Update_Messages updates("You move " + U::direction_to_string(direction) + ".",

		// "[name] arrives from the [direction] wielding an [item]."
		this->name + " arrives from the " + U::direction_to_string(U::opposite_direction(direction)) +
		((this->equipped_item == nullptr) ? "." : (" wielding " + U::get_article_for(equipped_item->get_name()) + " " + equipped_item->get_name() + ".")),

		true); // update required for all users in sight range

	// users that have fallen out of view won't recieve a map update unless we send one to them explicitly
	updates.additional_map_update_users = std::make_shared<std::vector<character_id>>(std::move(additional_users_to_notify));

	return updates;
}
Update_Messages Character::craft(const std::string & craft_item_id)
{
	// check for special cases
	if (craft_item_id == C::CHEST_ID)
	{
		if (world->room_at(location)->has_chest())
		{
			return Update_Messages("There is already a chest here.");
		}
	}
	else if (craft_item_id == C::TABLE_ID)
	{
		if (world->room_at(location)->has_table())
		{
			return Update_Messages("There is already a table here.");
		}
	}

	// return if the recipe does not exist
	if (!Character::recipes->has_recipe_for(craft_item_id)) { return Update_Messages("There is no way to craft " + U::get_article_for(craft_item_id) + " " + craft_item_id + "."); }

	// get the recipe
	const Recipe recipe = Character::recipes->get_recipe(craft_item_id);

	// verify the conditions for the recipe are present
	for (std::map<std::string, int>::const_iterator it = recipe.inventory_need.cbegin(); it != recipe.inventory_need.cend(); ++it)
	{
		if (it->first != "" && !this->contains(it->first, it->second)) { return Update_Messages(U::get_article_for(craft_item_id) + " " + craft_item_id + " requires " + ((it->second == 1) ? U::get_article_for(it->first) : U::to_string(it->second)) + " " + it->first); }
	}
	for (std::map<std::string, int>::const_iterator it = recipe.inventory_remove.cbegin(); it != recipe.inventory_remove.cend(); ++it)
	{
		if (it->first != "" && !this->contains(it->first, it->second)) { return Update_Messages(U::get_article_for(craft_item_id) + " " + craft_item_id + " uses " + ((it->second == 1) ? U::get_article_for(it->first) : U::to_string(it->second)) + " " + it->first); }
	}
	for (std::map<std::string, int>::const_iterator it = recipe.local_need.cbegin(); it != recipe.local_need.cend(); ++it)
	{
		if (it->first != "" && !world->room_at(location)->contains(it->first)) { return Update_Messages(U::get_article_for(craft_item_id) + " " + craft_item_id + " requires " + ((it->second == 1) ? "a" : U::to_string(it->second)) + " nearby " + it->first); }
	}
	for (std::map<std::string, int>::const_iterator it = recipe.local_remove.cbegin(); it != recipe.local_remove.cend(); ++it)
	{
		if (it->first != "" && !world->room_at(location)->contains(it->first)) { return Update_Messages(U::get_article_for(craft_item_id) + " " + craft_item_id + " uses " + ((it->second == 1) ? "a" : U::to_string(it->second)) + " nearby " + it->first); }
	}

	// remove ingredients from inventory
	for (std::map<std::string, int>::const_iterator it = recipe.inventory_remove.cbegin(); it != recipe.inventory_remove.cend(); ++it)
	{
		this->erase(it->first, it->second); // ID, count
	}

	// remove ingredients from area
	for (std::map<std::string, int>::const_iterator it = recipe.local_remove.cbegin(); it != recipe.local_remove.cend(); ++it)
	{
		this->erase(it->first, it->second); // ID, count
	}

	std::stringstream player_update, room_update;

	// for each item to be given to the player
	for (std::map<std::string, int>::const_iterator it = recipe.yields.cbegin(); it != recipe.yields.cend(); ++it)
	{
		// for as many times as the item is to be given to the player
		for (int i = 0; i < it->second; ++i)
		{
			// special test cases
			if (craft_item_id == C::CHEST_ID)
			{
				// add a chest to the room
				world->room_at(location)->add_chest(this->faction_ID);
				continue;
			}
			else if (craft_item_id == C::TABLE_ID)
			{
				// add a table to the room
				world->room_at(location)->add_table();
				continue;
			}

			// craft the item
			std::shared_ptr<Item> item = Craft::make(it->first);

			// if the item can be carried
			if (item->is_takable())
			{
				// add the item to the player's inventory
				this->insert(item);
			}
			else // the item can not be taken
			{
				// add the item to the room
				world->room_at(location)->insert(item);
			}
		}

		player_update << "You craft ";
		room_update << this->name << " crafts ";

		if (it->second > 1) // ... 3 arrowheads.
		{
			player_update << it->second << " " << U::get_plural_for(it->first) << ".";
			room_update << it->second << " " << U::get_plural_for(it->first) << ".";
		}
		else // ... an arrowhead.
		{
			player_update << U::get_article_for(it->first) << " " << it->first << ".";
			room_update << U::get_article_for(it->first) << " " << it->first << ".";
		}
	}

	return Update_Messages(player_update.str(), room_update.str());
}
Update_Messages Character::take(const std::string & take_item_id, const std::string & count)
{
	if (!Craft::make(take_item_id)->is_takable())
	{
		return Update_Messages("You cannot take " + U::get_article_for(take_item_id) + " " + take_item_id + ".");
	}

	// create a counter to determine how many items are actually acquired
	const unsigned acquire_count = Character::move_items(*world->room_at(location), *this, take_item_id, count);

	if (acquire_count == 0)
	{
		return Update_Messages("There aren't any " + U::get_plural_for(take_item_id) + " here.");
	}
	else if (acquire_count > 1) // plural result
	{
		return Update_Messages("You take " + U::to_string(acquire_count) + " " + U::get_plural_for(take_item_id) + ".",
			this->name + " takes " + U::to_string(acquire_count) + " " + U::get_plural_for(take_item_id) + ".");
	}
	else // singular result
	{
		return Update_Messages("You take " + U::get_article_for(take_item_id) + " " + take_item_id + ".",
			this->name + " takes " + U::get_article_for(take_item_id) + " " + take_item_id + ".");
	}
}
Update_Messages Character::drop(const std::string & drop_item_id, const std::string & count)
{
	// create a counter to determine how many items are actually dropped
	const unsigned drop_count = Character::move_items(*this, *world->room_at(location), drop_item_id, count);

	if (drop_count == 0)
	{
		return Update_Messages("You don't have " + U::get_article_for(drop_item_id) + " " + drop_item_id + ".");
	}
	else if (drop_count > 1) // plural result
	{
		return Update_Messages("You drop " + U::to_string(drop_count) + " " + U::get_plural_for(drop_item_id) + ".",
			this->name + " drops " + U::to_string(drop_count) + " " + U::get_plural_for(drop_item_id) + ".");
	}
	else // singular result
	{
		return Update_Messages("You drop " + U::get_article_for(drop_item_id) + " " + drop_item_id + ".",
			this->name + " drops " + U::get_article_for(drop_item_id) + " " + drop_item_id + ".");
	}
}
Update_Messages Character::equip(const std::string & item_ID)
{
	/*
	You ready your [item_id].
	You replace your [item_id] with a(n) [item_id];
	*/

	// if the player does not have the item specified
	if (!this->contains(item_ID))
	{
		// if the player is wielding an instance of the item already
		if (this->equipped_item->get_name() == item_ID)
		{
			return Update_Messages("You are holding " + U::get_article_for(item_ID) + " " + item_ID + " and don't have another one to equip.");
		}
		else // the player neither has nor is wielding the item
		{
			return Update_Messages("You do not have " + U::get_article_for(item_ID) + " " + item_ID + " to equip.");
		}
	}

	// the player can equip the item; create a stringstream to accumulate feedback
	std::stringstream user_update;
	std::stringstream room_update;

	// the player does have the item to equip, test if an item is already equipped
	if (this->equipped_item != nullptr)
	{
		user_update << "You replace your " << equipped_item->get_name() << " with ";
		room_update << this->name << " replaces " << U::get_article_for(equipped_item->get_name()) << " " << equipped_item->get_name() << " with ";

		// save the equipped item
		const std::shared_ptr<Item> item = equipped_item;

		// replace the equipped item with the item from the player's inventory
		this->equipped_item = this->erase(item_ID);

		// add the previously equipped item to the player's inventory
		this->insert(item);
	}

	// set the equipped item to the specified item
	equipped_item = this->erase(item_ID);

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
		user_update << U::get_article_for(equipped_item->get_name()) << " " << equipped_item->get_name() << ".";
		room_update << U::get_article_for(equipped_item->get_name()) << " " << equipped_item->get_name() << ".";

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
	const std::string item_ID = equipped_item->get_name();

	// save the existing the item to the player's inventory
	this->insert(equipped_item);

	// reset the currently equipped item
	equipped_item = nullptr;

	return Update_Messages("You put your " + item_ID + " away.", this->name + " lowers the " + item_ID + ".");
}
Update_Messages Character::add_to_chest(std::string insert_item_id, const std::string & count)
{
	const std::unique_ptr<Room>::pointer room = world->room_at(location);

	// if this room does not have a chest
	if (!room->has_chest())
	{
		return Update_Messages("There is no chest here.");
	}

	// if the chest was crafted by another faction
	if (room->get_chest_faction_id() != this->faction_ID)
	{
		return Update_Messages("This chest has an unfamiliar lock.");
	}

	// move the items
	const unsigned add_count = Character::move_items(*this, *room->get_chest(), insert_item_id, count);

	if (add_count == 0)
	{
		return Update_Messages("You don't have " + U::get_article_for(insert_item_id) + " " + insert_item_id + ".");
	}
	else if (add_count > 1) // plural result
	{
		return Update_Messages("You place " + U::to_string(add_count) + " " + U::get_plural_for(insert_item_id) + " into the chest.",
			this->name + " places " + U::to_string(add_count) + " " + U::get_plural_for(insert_item_id) + " into the chest.");
	}
	else // singular result
	{
		return Update_Messages("You place " + U::get_article_for(insert_item_id) + " " + insert_item_id + " into the chest.",
			this->name + " places " + U::get_article_for(insert_item_id) + " " + insert_item_id + " into the chest.");
	}
}
Update_Messages Character::take_from_chest(const std::string & take_item_id, const std::string & count)
{
	const std::unique_ptr<Room>::pointer room = world->room_at(location);

	// if this room does not have a chest
	if (!room->has_chest())
	{
		return Update_Messages("There is no chest here.");
	}

	// if the chest was crafted by another faction
	if (room->get_chest_faction_id() != this->faction_ID)
	{
		return Update_Messages("This chest has an unfamiliar lock.");
	}

	// move the items
	const unsigned acquire_count = Character::move_items(*room->get_chest(), *this, take_item_id, count);

	if (acquire_count == 0)
	{
		return Update_Messages("The chest does not contain any " + U::get_plural_for(take_item_id) + ".");
	}
	else if (acquire_count > 1) // plural result
	{
		return Update_Messages("You take " + U::to_string(acquire_count) + " " + U::get_plural_for(take_item_id) + " from the chest.",
			this->name + " takes " + U::to_string(acquire_count) + " " + U::get_plural_for(take_item_id) + " from the chest.");
	}
	else // singular result
	{
		return Update_Messages("You take " + U::get_article_for(take_item_id) + " " + take_item_id + " from the chest.",
			this->name + " takes " + U::get_article_for(take_item_id) + " " + take_item_id + " from the chest.");
	}
}
Update_Messages Character::look_inside_chest() const
{
	// validation within
	return world->room_at(location)->chest_contents(faction_ID, this->name);
}
Update_Messages Character::add_to_table(const std::string & add_item_ID, const std::string & count)
{
	const std::unique_ptr<Room>::pointer room = world->room_at(location);

	// check if there is a table in the room
	if (!room->has_table())
	{
		return Update_Messages("There is no table here.");
	}

	// create a counter to determine how many items are actually added to the table
	const unsigned add_count = Character::move_items(*this, *room->get_table(), add_item_ID, count);

	if (add_count == 0)
	{
		return Update_Messages("You don't have " + U::get_article_for(add_item_ID) + " " + add_item_ID + ".");
	}
	else if (add_count > 1) // plural result
	{
		return Update_Messages("You put " + U::to_string(add_count) + " " + U::get_plural_for(add_item_ID) + " on the table.",
			this->name + " drops " + U::to_string(add_count) + " " + U::get_plural_for(add_item_ID) + " on the table.");
	}
	else // singular result
	{
		return Update_Messages("You put " + U::get_article_for(add_item_ID) + " " + add_item_ID + " on the table.",
			this->name + " drops " + U::get_article_for(add_item_ID) + " " + add_item_ID + " on the table.");
	}
}
Update_Messages Character::take_from_table(const std::string take_item_ID, const std::string & count)
{
	const std::unique_ptr<Room>::pointer room = world->room_at(location);

	// check if there is a table in the room
	if (!room->has_table())
	{
		return Update_Messages("There is no table here.");
	}

	// move the item from the table to the player's inventory
	const unsigned acquire_count = Character::move_items(*room->get_table(), *this, take_item_ID, count);

	if (acquire_count == 0)
	{
		return Update_Messages("The table does not contain any " + U::get_plural_for(take_item_ID) + ".");
	}
	else if (acquire_count > 1) // plural result
	{
		return Update_Messages("You take " + U::to_string(acquire_count) + " " + U::get_plural_for(take_item_ID) + " from the table.",
			this->name + " takes " + U::to_string(acquire_count) + " " + U::get_plural_for(take_item_ID) + " from the table.");
	}
	else // singular result
	{
		return Update_Messages("You take " + U::get_article_for(take_item_ID) + " " + take_item_ID + " from the table.",
			this->name + " takes " + U::get_article_for(take_item_ID) + " " + take_item_ID + " from the table.");
	}
}
Update_Messages Character::look_at_table() const
{
	// validation within
	return world->room_at(location)->table_contents(this->name);
}
Update_Messages Character::construct_surface(const std::string & material_id, const C::surface & surface)
{
	if (world->room_at(location)->is_forest())
	{
		return Update_Messages("You are in a forest and cannot build a structure here.");
	}

	// make sure the material can be used to construct a surface
	if (C::SURFACE_REQUIREMENTS.find(material_id) == C::SURFACE_REQUIREMENTS.end())
	{
		return Update_Messages("You can't build a structure's surface out of " + material_id + ".");
	}

	// check if the surface already exists
	if (world->room_at(location)->has_surface(surface)) // bounds checking not necissary because the player is standing here
	{
		// test if construction is prevented by an intact wall or a pile of rubble
		if (world->room_at(location)->get_room_sides()[(size_t)surface]->is_rubble())
		{
			return Update_Messages("A pile of rubble prevents construction.");
		}
		else // the surface is intact
		{
			return Update_Messages((surface == C::surface::ceiling || surface == C::surface::floor) ?
				"A " + U::surface_to_string(surface) + " already exists here." : // ceiling or floor
				U::capitalize(U::get_article_for(U::surface_to_string(surface))) + " " + U::surface_to_string(surface) + " wall already exists here."); // any wall
		}
	}

	// check that the surface to construct is a wall, ceiling, or floor
	if (surface == C::surface::not_a_surface)
	{
		return Update_Messages("Construct a wall, ceiling or floor.");
	}

	// if the surface is a ceiling, check that any intact wall exists
	if (surface == C::surface::ceiling && // the user is constructing a ceiling
		!world->room_at(location)->has_standing_wall()) // the room does not have a wall
	{
		return Update_Messages("You need at least one standing wall to support a ceiling.");
	}

	// check that the player has the item
	if (!this->contains(material_id))
	{
		return Update_Messages("You don't have " + material_id + ".");
	}

	// check that the player has enough of the item to construct
	if (!this->contains(material_id, C::SURFACE_REQUIREMENTS.find(material_id)->second))
	{
		// "You need 5 wood to continue construction."
		return Update_Messages("You need " + U::to_string(C::SURFACE_REQUIREMENTS.find(material_id)->second) + " " + material_id + " to continue construction.");
	}

	// remove the number of materials from the player's inventory
	this->erase(material_id, C::SURFACE_REQUIREMENTS.find(material_id)->second);

	// create a Room_Side and add it to Room::room_side using the surface ID
	world->room_at(location)->add_surface(surface, material_id);

	// "You construct a stone floor/ceiling." OR "You construct a stone wall to your north."
	return Update_Messages("You construct a " + material_id + // you construct a [material]
		((surface != C::surface::ceiling && surface != C::surface::floor) ?
			" wall to your " + U::surface_to_string(surface) : // wall to your [direction]
			" " + U::surface_to_string(surface)), // ceiling/floor

		this->name + " constructs a " + material_id + // [name] constructs a [material]
		((surface != C::surface::ceiling && surface != C::surface::floor) ?
			" wall to your " + U::surface_to_string(surface) : // wall to your [direction]
			" " + U::surface_to_string(surface)), // ceiling/floor

		true);
}
Update_Messages Character::construct_surface_with_door(const std::string & surface_material_id, const C::surface & surface, const std::string & door_material_id)
{
	// Part 1: validate that a surface can be constructed



	if (world->room_at(location)->is_forest())
	{
		return Update_Messages("You are in a forest and cannot build a structure here.");
	}

	// make sure the material can be used to construct a surface
	if (C::SURFACE_REQUIREMENTS.find(surface_material_id) == C::SURFACE_REQUIREMENTS.end())
	{
		return Update_Messages("You can't build a structure's surface out of " + surface_material_id + ".");
	}

	// check if the surface already exists
	if (world->room_at(location)->has_surface(surface)) // bounds checking not necessary because the player is standing here
	{
		// test if construction is prevented by an intact wall or a pile of rubble
		if (world->room_at(location)->get_room_sides()[(size_t)surface]->is_rubble())

		{
			return Update_Messages("A pile of rubble prevents construction.");
		}
		else // the surface is intact
		{
			return Update_Messages((surface == C::surface::ceiling || surface == C::surface::floor) ?
				"A " + U::surface_to_string(surface) + " already exists here." : // ceiling or floor
				U::capitalize(U::get_article_for(U::surface_to_string(surface))) + " " + U::surface_to_string(surface) + " wall already exists here."); // any wall
		}
	}

	// check that the surface to construct is a wall, ceiling, or floor
	if (surface == C::surface::not_a_surface)
	{
		return Update_Messages("Construct a wall, ceiling or floor.");
	}

	// if the surface is a ceiling, check that any intact wall exists
	if (surface == C::surface::ceiling && // the user is construction a ceiling
		!world->room_at(location)->has_standing_wall()) // the room does not have a wall
	{
		return Update_Messages("You need at least one standing wall to support a ceiling.");
	}

	// check that the player has the item
	if (!this->contains(surface_material_id))
	{
		return Update_Messages("You don't have any " + U::get_plural_for(surface_material_id) + ".");
	}

	// check that the player has enough of the item to construct
	if (!this->contains(surface_material_id, C::SURFACE_REQUIREMENTS.find(surface_material_id)->second))
	{
		// "You need 5 wood to continue construction of the wall."
		return Update_Messages("You need " + U::to_string(C::SURFACE_REQUIREMENTS.find(surface_material_id)->second) + " " + U::get_plural_for(surface_material_id) + " to continue construction of the wall.");
	}



	// Part 2: Validate that a door can be constructed



	// check that there exist requirements for making a door of the specified type
	if (C::DOOR_REQUIREMENTS.find(door_material_id) == C::DOOR_REQUIREMENTS.cend())
	{
		return Update_Messages("You cannot construct a door using " + door_material_id + ".");
	}

	// extract the amount of materials required to make a door of the specified type
	const unsigned DOOR_MATERIAL_COUNT_REQUIRED = C::DOOR_REQUIREMENTS.find(door_material_id)->second;

	// check that the player has the required material
	if (!this->contains(door_material_id))
	{
		return Update_Messages("You don't have any " + U::get_plural_for(surface_material_id) + ".");
	}

	// check that the player has enough of the required material
	if (!this->contains(door_material_id, DOOR_MATERIAL_COUNT_REQUIRED))
	{
		// "A stone door requires 5 stones."
		return Update_Messages("A " + door_material_id + " door requires " + U::to_string(DOOR_MATERIAL_COUNT_REQUIRED) + " " + U::get_plural_for(door_material_id) + ".");
	}



	// Part 3: Build the surface



	// remove the materials to construct the surface
	this->erase(surface_material_id, C::SURFACE_REQUIREMENTS.find(surface_material_id)->second);

	// add the surface to the room
	world->room_at(location)->add_surface(surface, surface_material_id);



	// Part 4: Add the door to the surface



	// remove the materials to construct the door
	this->erase(door_material_id, C::DOOR_REQUIREMENTS.find(door_material_id)->second);

	// add a door to the surface in the room
	world->room_at(location)->add_door(surface, C::MAX_SURFACE_HEALTH, door_material_id, this->faction_ID);



	// Part 5: the responses

	std::stringstream to_player, to_room;

	to_player << "You construct a " << surface_material_id; // you construct a [material]
	to_room << this->name << " constructs a " << surface_material_id; // you construct a [material]

	if (surface == C::surface::ceiling || surface == C::surface::floor) // the player constructed a ceiling or floor
	{
		to_player << " " << U::surface_to_string(surface) << " with a " << door_material_id << " hatch.";
		to_room << " " << U::surface_to_string(surface) << " with a " << door_material_id << " hatch.";
	}
	else // constructing a wall rather than a ceiling or floor
	{
		to_player << " wall to your " << U::surface_to_string(surface) << " with a " << door_material_id << " door.";
		to_room << " wall to your " << U::surface_to_string(surface) << " with a " << door_material_id << " door.";
	}

	// "You construct a stone floor with a stone hatch." OR "You construct a stone wall to your north with a branch door."
	return Update_Messages(to_player.str(), to_room.str(), true); // send messages to user and room, and require map update for players in view distance
}
Update_Messages Character::attack_surface(const std::string & surface_ID)
{
	// get this check out of the way
	if (surface_ID == C::CEILING || surface_ID == C::FLOOR)
	{
		return Update_Messages("Damaging a surface in a room above or below you is not supported yet.");
	}

	// verify we are working with a primary compass point
	if (surface_ID != C::NORTH && surface_ID != C::EAST &&
		surface_ID != C::SOUTH && surface_ID != C::WEST)
	{
		return Update_Messages("Only n/s/e/w surfaces can be damaged at this time.");
	}

	// if the current room has an intact surface
	if (world->room_at(location)->is_standing_wall(surface_ID))
	{
		// apply damage to the surface
		return world->room_at(location)->damage_surface(surface_ID, this->equipped_item, this->name);
	}

	// this room does not have an intact surface, the neighboring room might

	// find coordinates of neighboring room
	const Coordinate target = location.get_after_move((C::direction)U::to_surface(surface_ID));

	// if the neighboring room has the opposite surface intact (our west wall borders next room's east wall)
	if (world->room_at(target)->is_standing_wall(C::opposite_surface_id.find(surface_ID)->second)) // deliberately using just "z" throughout this block
	{
		// inflict damage upon the surface
		return world->room_at(target)->damage_surface(C::opposite_surface_id.find(surface_ID)->second, this->equipped_item, this->name);
	}

	// neither room has an intact surface

	// test if both walls do not exist
	if (!world->room_at(location)->has_surface(surface_ID) &&
		!world->room_at(target)->has_surface(C::opposite_surface_id.find(surface_ID)->second))
	{
		return Update_Messages("There is no " + surface_ID + " wall here.");
	}
	else
	{
		// any surface that does exist is rubble, and at least one surface exists
		return Update_Messages("There is only rubble where a wall once was.");
	}
}
Update_Messages Character::attack_door(const std::string & surface_ID)
{
	// get this check out of the way
	if (surface_ID == C::CEILING || surface_ID == C::FLOOR)
	{
		return Update_Messages("Damaging above or below you is not supported yet.");
	}

	// verify we are working with a primary compass point
	if (surface_ID != C::NORTH && surface_ID != C::EAST &&
		surface_ID != C::SOUTH && surface_ID != C::WEST)
	{
		return Update_Messages("Only doors in n/s/e/w surfaces can be damaged at this time.");
	}

	// if the current room has an intact surface with an intact door in it
	if (world->room_at(location)->has_surface(surface_ID) && world->room_at(location)->get_room_sides()[(size_t)U::to_surface(surface_ID)]->has_intact_door())
	{
		// applied damage to the door
		return world->room_at(location)->damage_door(surface_ID, this->equipped_item, this->name);
	}

	// the current room does not have an intact door in the specified direction,
	// test if the next room has an intact door facing us.

	// find coordinates of neighboring room
	const Coordinate target = location.get_after_move((C::direction)U::to_surface(surface_ID));

	// if the neighboring room has the opposite surface intact
	if (world->room_at(target)->is_standing_wall(C::opposite_surface_id.find(surface_ID)->second)) // deliberately using just "z" throughout this block
	{
		// inflict damaage upon the surface or door
		return world->room_at(target)->damage_door(C::opposite_surface_id.find(surface_ID)->second, this->equipped_item, this->name);
	}

	// this feedback might not be correct for all cases
	return Update_Messages("There is no door to your " + surface_ID + ".");
}
Update_Messages Character::attack_item(const std::string & target_ID)
{
	// if the target isn't here
	if (!world->room_at(location)->contains(target_ID))
	{
		return Update_Messages("There is no " + target_ID + " here.");
	}

	// if the user has an item equipped
	if (equipped_item != nullptr)
	{
		// if the attacking implement is not in the damage tables
		if (C::damage_tables.find(equipped_item->get_name()) == C::damage_tables.cend())
		{
			return Update_Messages("You can't do that with " + U::get_article_for(equipped_item->get_name()) + " " + equipped_item->get_name() + ".");
		}

		// extract the damage table for the attacking implement
		const std::map<std::string, int> damage_table = C::damage_tables.find(equipped_item->get_name())->second;

		// if the damage table does not have an entry for the target item ID
		if (damage_table.find(target_ID) == damage_table.cend())
		{
			return Update_Messages("You can't do that to a " + target_ID + ".");
		}

		// damage the item, return a different message depending of if the item was destroyed or damaged
		if (world->room_at(location)->damage_item(target_ID, damage_table.find(target_ID)->second))
		{
			return Update_Messages("You destroy the " + target_ID + " using your " + equipped_item->get_name() + ".",
				this->name + " uses " + U::get_article_for(equipped_item->get_name()) + " " + equipped_item->get_name() + " to destroy " + U::get_article_for(target_ID) + " " + target_ID + ".",
				true);
		}
		else
		{
			return Update_Messages("You damage the " + target_ID + " using your " + equipped_item->get_name() + ".",
				this->name + " uses " + U::get_article_for(equipped_item->get_name()) + " " + equipped_item->get_name() + " to damage " + U::get_article_for(target_ID) + " " + target_ID + ".");
		}
	}
	else // the user does not have an item equipped, do a barehanded attack
	{
		// extract the damage table for a bare-handed attack
		const std::map<std::string, int> damage_table = C::damage_tables.find(C::ATTACK_COMMAND)->second;

		// if the damage table does not contain an entry for the target
		if (damage_table.find(target_ID) == damage_table.cend())
		{
			return Update_Messages("You can't do that to a " + target_ID + ".");
		}

		// the damage table does contain an entry for the target
		if (world->room_at(location)->damage_item(target_ID, damage_table.find(target_ID)->second))
		{
			return Update_Messages("You destroy the " + target_ID + " using your bare hands.",
				this->name + " uses bare hands to destroy " + U::get_article_for(target_ID) + " " + target_ID + ".",
				true);
		}
		else
		{
			return Update_Messages("You damage the " + target_ID + " using your bare hands.",
				this->name + " uses bare hands to damage " + U::get_article_for(target_ID) + " " + target_ID + ".");
		}
	}
}
Update_Messages Character::add_to_bloomery(const std::string & item_ID, const unsigned & count)
{
	// ****** here

	if (!world->room_at(location)->contains(C::BLOOMERY_ID))
	{
		return Update_Messages("There is no bloomery here.");
	}

	// std::shared_ptr<Item> item = this->equipment_inventory

	// world->room_at(x, y)->get_contents

	return Update_Messages("Character::add_to_bloomery() is incomplete.");
}
Update_Messages Character::attack_character(std::shared_ptr<Character> & target)
{
	/*

	The attacking implement has a base damage of a random value from 1 to the implement's damage, inclusive.

	The target has a base defense of a random value from 0 to the target's defense rating, inclusive.

	The base defense is subtracted from the base attack, and the result is subtracted from the target's health.
	- The resulting damage will always be 0 or greater.
	- The target's health will never go below 0.
	- If the target's health reaches 0, the target dies.

	*/

	// an unarmed attack
	if (this->equipped_item == nullptr)
	{
		const int base_attack = U::random_int_from(1, C::damage_tables.find(C::ATTACK_COMMAND)->second.find("")->second);
		const int base_defense = U::random_int_from(0u, target->get_defense());

		// calculate the damage
		const int damage = std::max(0, base_attack - base_defense);

		// subtract the damage from the target's health, ensuring the target's health does not go below 0
		target->current_health -= std::min(target->current_health, damage);
	}
	// an attack using a type of equipment
	else if (std::shared_ptr<Equipment> weapon = U::convert_to<Equipment>(equipped_item))
	{
		const int base_attack = U::random_int_from(1u, weapon->get_combat_damage());
		const int base_defense = U::random_int_from(0u, target->get_defense());

		const int damage = std::max(0, base_attack - base_defense);

		target->current_health -= std::min(target->current_health, damage);
	}
	else // an attempted attack using a non-equipment item
	{
		return Update_Messages("You can't attack using " + U::get_article_for(equipped_item->get_name()) + " " + equipped_item->get_name() + ".");
	}

	// if the target is dead
	if (target->current_health == 0)
	{
		// die() handles the dying target's inventory
		Update_Messages update_messages = target->die();

		// remove the target from their current destination
		world->room_at(location)->remove_actor(target->get_ID());

		// attempt to unload the rooms around the player
		world->attempt_unload_radius(target->location, target->get_ID());

		// move the dead player to the spawn location
		const Coordinate spawn(C::DEFAULT_SPAWN_X, C::DEFAULT_SPAWN_Y); // this should be defined in the constant class because we use it elsewhere
		world->load_view_radius_around(spawn, target->get_ID());
		target->location = spawn;

		// restore the player's health
		target->reset_health();

		return update_messages;
	}

	return Update_Messages("You attack " + target->name + " with " +
		((equipped_item == nullptr) ? "your bare hands.\n" : U::get_article_for(equipped_item->get_name()) + " " + equipped_item->get_name() + ".\n"),

		name + " attacks " + target->name + " with " +
		((equipped_item == nullptr) ? "their bare hands.\n" : (U::get_article_for(equipped_item->get_name() + " " + equipped_item->get_name() + ".\n"))));
}
Update_Messages Character::die()
{
	// The player drops the item they are holding, all equipment from their inventory, a random amount of each stackable type from their inventory (at least one of each), and keeps any other item.

	const std::unique_ptr<Room> & room = world->reference_room_at(location);

	// the player drops the item they're holding
	room->insert(std::move(equipped_item));

	// The following use of "erase(item_it++)" is a subtle way of making sure our traversing iterator
	// is always valid, whether an entry from the map is removed or not. This is a feature
	// of post-incrementing.

	for (auto item_it = contents.begin(); item_it != contents.cend(); )
	{
		// all equipment is dropped
		if (U::is<Equipment>(item_it->second))
		{
			room->insert(this->erase(item_it++->first));
		}
		// a random amount of each stackable is dropped (at least one of each)
		else if (U::is<Stackable>(item_it->second))
		{
			// calculate the number of items to be dropped
			const unsigned amount = U::random_int_from(1u, this->count(item_it->first));
			// add that number of items to the room
			for (unsigned i = 0; i < amount; ++i)
				room->insert(Craft::make(item_it->first));
			// remove that number of items from the player
			this->erase(item_it++->first, amount);
		}
		else // items that are neither equipment nor stackable stay with the killed player
		{
			++item_it;
		}
	}

	return Update_Messages(name + " dies.", name + " dies.", std::make_pair(id, "You die."));
}

// movement info
C::move_attempt Character::validate_movement(const Coordinate & current, const C::direction & direction, const Coordinate & destination) const
{
	// determine if a character can move in a given direction (8 compass points)

	// validate direction (this is now a dumb error message)
	if (direction == C::direction::not_a_direction) { return C::move_attempt::not_a_direction; }

	// if the player wants to move in a primary direction (n/e/s/w)
	if (direction == C::direction::north || direction == C::direction::east ||
		direction == C::direction::south || direction == C::direction::west)
	{
		// save the value of an attempt to move out of the current room
		C::move_attempt move_attempt = world->room_at(current)->can_move_in_direction(direction, faction_ID);

		if (move_attempt != C::move_attempt::traversable)
		{
			// the player can't move out of the current room
			return move_attempt;
		}

		// save the value of an attempt to move into the destination room
		move_attempt = world->room_at(destination)->can_move_in_direction(U::opposite_direction(direction), faction_ID);

		if (move_attempt != C::move_attempt::traversable)
		{
			// the player can't move into the destination
			return move_attempt;
		}
	}
	// if the player wants to move in a secondary direction (nw/ne/se/sw), condition is
	// none of four possible walls obstruct path AND an obstructing corner is not formed by adjacent rooms
	else if (
		direction == C::direction::north_west || direction == C::direction::north_east ||
		direction == C::direction::south_east || direction == C::direction::south_west)
	{
		const std::unique_ptr<Room>::pointer current_room = world->room_at(current);
		const std::unique_ptr<Room>::pointer destination_room = world->room_at(destination);

		if (direction == C::direction::north_west)
		{
			const auto north_room = world->room_at(current.get_after_move(C::direction::north));
			const auto west_room = world->room_at(current.get_after_move(C::direction::west));

			if (current_room->has_surface(C::surface::north) || current_room->has_surface(C::surface::west) ||
				destination_room->has_surface(C::surface::south) || destination_room->has_surface(C::surface::east) ||
				((north_room->has_surface(C::surface::south) || north_room->has_surface(C::surface::west)) &&
				(west_room->has_surface(C::surface::north) || west_room->has_surface(C::surface::east))))
			{
				return C::move_attempt::indirectly_blocked_by_wall;
			}
		}
		else if (direction == C::direction::north_east)
		{
			const auto north_room = world->room_at(current.get_after_move(C::direction::north));
			const auto east_room = world->room_at(current.get_after_move(C::direction::east));

			if (current_room->has_surface(C::surface::north) || current_room->has_surface(C::surface::east) ||
				destination_room->has_surface(C::surface::south) || destination_room->has_surface(C::surface::west) ||
				((north_room->has_surface(C::surface::south) || north_room->has_surface(C::surface::east)) &&
				(east_room->has_surface(C::surface::north) || east_room->has_surface(C::surface::west))))
			{
				return C::move_attempt::indirectly_blocked_by_wall;
			}
		}
		else if (direction == C::direction::south_east)
		{
			const auto south_room = world->room_at(current.get_after_move(C::direction::south));
			const auto east_room = world->room_at(current.get_after_move(C::direction::east));

			if (current_room->has_surface(C::surface::south) || current_room->has_surface(C::surface::east) ||
				destination_room->has_surface(C::surface::north) || destination_room->has_surface(C::surface::west) ||
				((south_room->has_surface(C::surface::north) || south_room->has_surface(C::surface::east)) &&
				(east_room->has_surface(C::surface::south) || east_room->has_surface(C::surface::west))))
			{
				return C::move_attempt::indirectly_blocked_by_wall;
			}
		}
		else if (direction == C::direction::south_west)
		{
			const auto south_room = world->room_at(current.get_after_move(C::direction::south));
			const auto west_room = world->room_at(current.get_after_move(C::direction::west));

			if (current_room->has_surface(C::surface::south) || current_room->has_surface(C::surface::west) ||
				destination_room->has_surface(C::surface::north) || destination_room->has_surface(C::surface::east) ||
				((south_room->has_surface(C::surface::north) || south_room->has_surface(C::surface::west)) &&
				(west_room->has_surface(C::surface::south) || west_room->has_surface(C::surface::east))))
			{
				return C::move_attempt::indirectly_blocked_by_wall;
			}
		}
	}

	// no issues were detected
	return C::move_attempt::traversable;
}

unsigned Character::move_items(Container & source, Container & destination, const std::string & item_ID, const std::string & count)
{
	unsigned move_count = 0;

	if (count == C::ALL_COMMAND)
	{
		move_count = source.count(item_ID);
	}
	else
	{
		move_count = U::to_unsigned(count);
	}

	unsigned items_moved = 0;

	for (unsigned int i = 0; i < move_count; ++i)
	{
		if (destination.insert(source.erase(item_ID)))
		{
			++items_moved;
		}
	}

	return items_moved;
}

// combat helper functions
unsigned Character::get_defense() const
{
	return 1; // soak
}
