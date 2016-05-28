/* Jim Viebke
Feb 14, 2015 */

#ifndef ROOM_H
#define ROOM_H

#include <map> // for room contents multimap
#include <set> // playerlist

#include "utilities.h"
#include "constants.h"
#include "item.h"
#include "room_side.h" // walls, floor, or ceiling
#include "message.h"

class Room : public Container
{
private:
	bool water = false; // is the room dry land or water?
	std::shared_ptr<Chest> chest = nullptr; // nullptr if the room does not have a chest
	std::shared_ptr<Table> table = nullptr; // nullptr if the room does not have a table
	std::map<std::string, Room_Side> room_sides = {}; // the floor, walls, and ceiling in the room (present surfaces only)
	std::vector<std::string> viewing_actor_ids = {}; // the PCs and NPCs who can see this room
	std::vector<std::string> actor_ids = {}; // the PCs and NPCs in a room

public:

	Room() : Container() {}

	// configuration
	void set_water_status(const bool & is_water) { water = is_water; }

	// room contents
	const std::multimap<std::string, std::shared_ptr<Item>> get_contents() const { return contents; }
	std::multimap<std::string, std::shared_ptr<Item>> & get_contents() { return contents; }
	const std::map<std::string, Room_Side> get_room_sides() const { return room_sides; }
	const std::vector<std::string> get_actor_ids() const { return actor_ids; }

	// room information
	bool has_wall() const;
	bool has_standing_wall() const;
	bool is_standing_wall(const std::string & surface_ID) const;
	bool has_surface(const std::string & direction_id) const;
	std::string can_move_in_direction(const std::string & direction_ID, const std::string & faction_ID);
	bool contains_no_items() const;
	bool is_unloadable() const;
	bool is_occupied_by(const std::string & actor_id) const;
	bool is_observed_by(const std::string & actor_id) const;
	bool is_water() const;
	bool is_forest() const;
	bool has_non_mineral_deposit_item() const;
	bool has_mineral_deposit() const;

	// chests
	void add_chest(const std::string & set_faction_id);
	bool has_chest() const;
	std::string get_chest_faction_id() const;
	int chest_health() const;
	bool add_item_to_chest(const std::shared_ptr<Item> & item);
	Update_Messages chest_contents(const std::string & faction_ID, const std::string & username) const;
	void damage_chest();
	bool chest_has(const std::string & item_id) const;
	std::shared_ptr<Chest> get_chest() const;
	void set_chest(const std::shared_ptr<Chest> & set_chest);

	// tables
	void add_table();
	bool has_table() const;
	bool add_item_to_table(const std::shared_ptr<Item> & item);
	Update_Messages table_contents(const std::string & username) const;
	bool table_has(const std::string & item_id) const;
	std::shared_ptr<Item> remove_from_table(const std::string & item_ID);
	std::shared_ptr<Table> get_table() const;
	void set_table(const std::shared_ptr<Table> & set_table);

	// bloomeries
	std::string add_item_to_bloomery(const std::shared_ptr<Forgeable> & item);

	// items
	bool damage_item(const std::string & item_id, const int & amount);

	// add surfaces and doors
	void add_surface(const std::string & surface_ID, const std::string & material_ID);
	void add_surface(const std::string & surface_ID, const std::string & material_ID, const int & surface_health);
	void add_door(const std::string & directon_ID, const int & health, const std::string & material_ID, const std::string & faction_ID);

	// damage surface
	Update_Messages damage_surface(const std::string & surface_ID, const std::shared_ptr<Item> & equipped_item, const std::string & username);
	Update_Messages damage_door(const std::string & surface_ID, const std::shared_ptr<Item> & equipped_item, const std::string & username);

	// add and remove actors
	void add_actor(const std::string & actor_id);
	void remove_actor(const std::string & actor_id);
	void add_viewing_actor(const std::string & actor_id);
	void remove_viewing_actor(const std::string & actor_id);

	// printing
	std::string summary(const std::string & player_ID) const;

};

#endif
