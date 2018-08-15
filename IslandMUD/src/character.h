/* Jim Viebke
Feb 14, 2015 */

#ifndef CHARACTER_H
#define CHARACTER_H

#include <map> // for inventory multimap

#include "constants.h"
#include "item.h"
#include "world.h"
#include "recipes.h"
#include "message.h"

class Character : public Container
{
private:
	long long last_action_timestamp;

	// these are levels/skills/abilities
	int swordsmanship_level = C::SWORDSMANSHIP_LEVEL_MIN;
	int archery_level = C::ARCHERY_LEVEL_MIN;
	int forest_visibility_level = C::FOREST_VISIBILITY_LEVEL_MIN;
	int max_health = C::FULL_HEALTH_MIN; // the player's full health, which increases combat experience

	// this is not a skill/ability, it is the current status
	int current_health = C::FULL_HEALTH_MIN; // the player's current health

public:

	std::string name;

	// Item dragging_item; // a character can drag an item if they don't want to carry it.	
	static std::unique_ptr<Recipes> recipes; // exists in memory once for all PCs and NPCs

protected:

	Coordinate location;

	std::shared_ptr<Item> equipped_item;

	std::string faction_ID;

	std::string leader_ID;
	std::vector<std::string> follower_IDs;

	Character(const std::string & name, const std::string & set_faction_ID, std::unique_ptr<World> & world);

public:

	virtual ~Character(); // make Character into a polymorphic type

	Update_Messages save();

	// levels
	void set_swordsmanship_level(const int & level_value);
	void set_archery_level(const int & level_value);
	void set_forest_visibilty_level(const int & level_value);
	void set_health_max(const int & level_value);
	void reset_health();

	// setters
	void set_current_health(const int & current_health);

	// inventory information
	bool does_not_have(const std::string & item_name, const unsigned & item_count = 1) const;
	std::string get_inventory() const; // debugging

	// actions
	Update_Messages craft(const std::string & craft_item_id, std::unique_ptr<World> & world);
	Update_Messages move(const std::string & direction, std::unique_ptr<World> & world);
	Update_Messages move(const C::direction & direction, std::unique_ptr<World> & world);
	Update_Messages take(const std::string & item_id, std::unique_ptr<World> & world, const std::string & count = "1");
	Update_Messages drop(const std::string & drop_item_id, std::unique_ptr<World> & world, const std::string & count = "1");
	Update_Messages equip(const std::string & item_ID);
	Update_Messages unequip();
	Update_Messages add_to_chest(std::string insert_item_id, std::unique_ptr<World> & world, const std::string & count = "1");
	Update_Messages take_from_chest(const std::string & take_item_id, std::unique_ptr<World> & world, const std::string & count = "1");
	Update_Messages look_inside_chest(const std::unique_ptr<World> & world) const;
	Update_Messages add_to_table(const std::string & add_item_ID, std::unique_ptr<World> & world, const std::string & count = "1");
	Update_Messages take_from_table(const std::string take_item_ID, std::unique_ptr<World> & world, const std::string & count = "1");
	Update_Messages look_at_table(const std::unique_ptr<World> & world) const;
	Update_Messages construct_surface(const std::string & material_id, const C::surface & surface, std::unique_ptr<World> & world);
	Update_Messages construct_surface_with_door(const std::string & material_id, const C::surface & surface, const std::string & door_material_id, std::unique_ptr<World> & world);
	Update_Messages attack_surface(const std::string & surface_ID, std::unique_ptr<World> & world);
	Update_Messages attack_door(const std::string & surface_ID, std::unique_ptr<World> & world);
	Update_Messages attack_item(const std::string & target_ID, std::unique_ptr<World> & world);
	Update_Messages add_to_bloomery(const std::string & item_ID, const unsigned & count, std::unique_ptr<World> & world);
	Update_Messages attack_character(std::shared_ptr<Character> & target, std::unique_ptr<World> & world);
	Update_Messages die(std::unique_ptr<World> & world);

	// movement info
	std::string validate_movement(const Coordinate & current, const C::direction & direction, const Coordinate & destination, const std::unique_ptr<World> & world) const;

	static unsigned move_items(Container & source, Container & destination, const std::string & item_ID, const std::string & count);

	// combat helper functions
	unsigned get_defense() const;
	
	Coordinate get_location() const { return location; }

private:

	void login(std::unique_ptr<World> & world); // called from constructor
};

#endif
