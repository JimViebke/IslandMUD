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

using namespace std;

class Character
{
public:
	string name;
	__int64 last_action_timestamp;
	string leader_ID;
	vector<string> follower_ids;
	shared_ptr<Item> equipped_item;

	multimap<string, shared_ptr<Equipment>> equipment_inventory; // equipment doesn't stack
	map<string, shared_ptr<Material>> material_inventory; // materials stack

	// Item dragging_item; // a character can drag an item if they don't want to carry it.
	static Recipes recipes; // exists in memory once for all PCs and NPCs

	int x = C::DEFAULT_SPAWN_X; // location coordinates
	int y = C::DEFAULT_SPAWN_Y;
	int z = C::DEFAULT_SPAWN_Z;

	Character(const string & name) : name(name) {}
	virtual ~Character() {} // to make a polymorphic type

	string login(World & world);
	string logout();

	// inventory information
	bool has(const string & item_name, const unsigned & item_count = 1) const;
	bool does_not_have(const string & item_name, const unsigned & item_count = 1) const;

	// inventory manipulation
	void add(const shared_ptr<Item> & item);
	void remove(const string & item_id, const unsigned & count = 1);
	string equip(const string & item_ID);
	string unequip(const string & item_ID);

	// actions
	string craft(const string & craft_item_id, World & world);
	string move(const string & direction, World & world);
	string take(const string & item_id, World & world);
	string drop(const string & drop_item_id, World & world);
	string construct_surface(const string & material_id, const string & surface_id, World & world);
	string attack_surface(const string & surface_ID, World & world);

	// movement info
	string validate_movement(const string & direction_ID, const int & dx, const int & dy, const int & dz, const World & world) const;

};

#endif
