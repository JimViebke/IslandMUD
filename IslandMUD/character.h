/* Jim Viebke
Feb 14, 2015 */

#ifndef CHARACTER_H
#define CHARACTER_H

#include <map> // for inventory multimap

#include "item.h"
#include "craft.h"
#include "room.h"
#include "world.h"
#include "recipes.h"
#include "message.h"

class Character
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
	string name;

protected:

	string faction_ID;
	shared_ptr<Item> equipped_item;

	string leader_ID;
	vector<string> follower_IDs;

	multimap<string, shared_ptr<Equipment>> equipment_inventory; // equipment doesn't stack
	map<string, shared_ptr<Material>> material_inventory; // materials stack

public:

	// Item dragging_item; // a character can drag an item if they don't want to carry it.
	static Recipes recipes; // exists in memory once for all PCs and NPCs

protected:

	Character(const string & name, const string & set_faction_ID);
	virtual ~Character() {} // to make a polymorphic type

public:

	string login(World & world);
	string logout();

	// levels
	void set_swordsmanship_level(const int & level_value);
	void set_archery_level(const int & level_value);
	void set_forest_visibilty_level(const int & level_value);
	void set_health_max(const int & level_value);

	// setters
	void set_current_health(const int & current_health);

	// inventory information
	bool has(const string & item_name, const unsigned & item_count = 1) const;
	bool does_not_have(const string & item_name, const unsigned & item_count = 1) const;
	string get_inventory() const; // debugging

	// inventory manipulation
	void add(const shared_ptr<Item> & item);
	void remove(const string & item_id, const unsigned & count = 1);
	Update_Messages equip(const string & item_ID);
	Update_Messages unequip();

	// actions
	string craft(const string & craft_item_id, World & world);
	Update_Messages move(const string & direction, World & world);
	Update_Messages take(const string & item_id, World & world);
	string drop(const string & drop_item_id, World & world);
	string add_to_chest(const string & insert_item_id, World & world);
	string take_from_chest(const string & take_item_id, World & world);
	string look_inside_chest(const World & world) const;
	string construct_surface(const string & material_id, const string & surface_id, World & world);
	string construct_surface_with_door(const string & material_id, const string & surface_id, const string & door_material_id, World & world);
	string attack_surface(const string & surface_ID, World & world);
	string attack_door(const string & surface_ID, World & world);
	string attack_item(const string & target_ID, World & world);

	// movement info
	string validate_movement(const int & cx, const int & cy, const int & cz, const string & direction_ID, const int & dx, const int & dy, const int & dz, const World & world) const;

};

#endif
