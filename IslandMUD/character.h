/* Jim Viebke
Feb 14, 2015 */

#ifndef CHARACTER_H
#define CHARACTER_H

#include <map> // for inventory multimap

#include "constants.h"
#include "item.h"
#include "craft.h"
#include "room.h"
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
	int x = C::DEFAULT_SPAWN_X; // location coordinates
	int y = C::DEFAULT_SPAWN_Y; // it's handy to have these signed for validation reasons
	int z = C::DEFAULT_SPAWN_Z;
	std::string name;

protected:

	std::string faction_ID;
	std::shared_ptr<Item> equipped_item;

	std::string leader_ID;
	std::vector<std::string> follower_IDs;

public:

	// Item dragging_item; // a character can drag an item if they don't want to carry it.	
	static std::unique_ptr<Recipes> recipes; // exists in memory once for all PCs and NPCs

protected:

	Character(const std::string & name, const std::string & set_faction_ID);
	virtual ~Character(); // make Character into a polymorphic type

public:

	std::string login(World & world);
	std::string save();

	// levels
	void set_swordsmanship_level(const int & level_value);
	void set_archery_level(const int & level_value);
	void set_forest_visibilty_level(const int & level_value);
	void set_health_max(const int & level_value);

	// setters
	void set_current_health(const int & current_health);

	// inventory information
	bool does_not_have(const std::string & item_name, const unsigned & item_count = 1) const;
	std::string get_inventory() const; // debugging

	// actions
	Update_Messages craft(const std::string & craft_item_id, World & world);
	Update_Messages move(const std::string & direction, World & world);
	Update_Messages take(const std::string & item_id, World & world);
	Update_Messages drop(const std::string & drop_item_id, World & world, const unsigned & count = 1);
	Update_Messages equip(const std::string & item_ID);
	Update_Messages unequip();
	Update_Messages add_to_chest(std::string insert_item_id, World & world, const unsigned & count = 1);
	Update_Messages take_from_chest(const std::string & take_item_id, World & world);
	Update_Messages look_inside_chest(const World & world) const;
	Update_Messages add_to_table(const std::string & add_item_ID, World & world, const unsigned & count = 1);
	Update_Messages take_from_table(const std::string remove_item_ID, World & world);
	Update_Messages look_at_table(const World & world) const;
	Update_Messages construct_surface(const std::string & material_id, const std::string & surface_id, World & world);
	Update_Messages construct_surface_with_door(const std::string & material_id, const std::string & surface_id, const std::string & door_material_id, World & world);
	Update_Messages attack_surface(const std::string & surface_ID, World & world);
	Update_Messages attack_door(const std::string & surface_ID, World & world);
	Update_Messages attack_item(const std::string & target_ID, World & world);
	Update_Messages add_to_bloomery(const std::string & item_ID, const unsigned & count, World & world);

	// movement info
	std::string validate_movement(const int & cx, const int & cy, const int & cz, const std::string & direction_ID, const int & dx, const int & dy, const int & dz, const World & world) const;

	// These are used to create function pointers to their corresponding non-static member functions aobve.
	static Update_Messages drop_call(std::shared_ptr<Character> & character, const std::string & item_ID, World & world, const unsigned & count);
	static Update_Messages add_to_chest_call(std::shared_ptr<Character> & character, const std::string & item_ID, World & world, const unsigned & count);
	static Update_Messages add_to_table_call(std::shared_ptr<Character> & character, const std::string & item_ID, World & world, const unsigned & count);

};

#endif
