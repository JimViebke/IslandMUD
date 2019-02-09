/* Jim Viebke
Feb 14, 2015 */

#ifndef ROOM_H
#define ROOM_H

#include <map> // for room contents multimap
#include <set> // playerlist
#include <optional>
#include <array>

#include "utilities.h"
#include "constants.h"
#include "item.h"
#include "room_side.h" // walls, floor, or ceiling
#include "message.h"
#include "coordinate.h"
#include "types.hpp"

class Room : public Container
{
private:
	bool water = false; // is the room dry land or water?
	std::shared_ptr<Chest> chest = nullptr; // nullptr if the room does not have a chest
	std::shared_ptr<Table> table = nullptr; // nullptr if the room does not have a table
	std::array<std::optional<Room_Side>, 6> room_sides;
	std::vector<character_id> viewing_actor_ids = {}; // the PCs and NPCs who can see this room
	std::vector<character_id> actor_ids = {}; // the PCs and NPCs in a room
	Coordinate coordinate;

public:

	Room(const int x, const int y) : Room(Coordinate(x, y)) {} // delegate
	Room(const Coordinate & coordinate) : Container(), coordinate(coordinate) {}

	// configuration
	void set_water_status(const bool & is_water) { water = is_water; }

	// room contents
	const std::multimap<std::string, std::shared_ptr<Item>> get_contents() const { return contents; }
	std::multimap<std::string, std::shared_ptr<Item>> & get_contents() { return contents; }
	const auto& get_room_sides() const { return room_sides; }
	const std::vector<character_id>& get_actor_ids() const { return actor_ids; }

	// room information
	bool has_wall() const;
	bool has_standing_wall() const;
	bool is_standing_wall(const std::string & surface_ID) const;
	bool has_surface(const std::string & direction_id) const;
	bool has_surface(const C::surface surface) const;
	C::move_attempt can_move_in_direction(const C::direction direction, const std::string & faction_ID) const;
	bool contains_no_items() const;
	bool is_unloadable() const;
	bool is_occupied_by(const character_id & actor_id) const;
	bool is_observed_by(const character_id & actor_id) const;
	bool is_water() const;
	bool is_forest() const;
	bool has_non_mineral_deposit_item() const;
	bool has_mineral_deposit() const;
	Coordinate get_coordinates() const;

	void remove(const C::surface surface);

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
	void add_surface(const C::surface surface, const std::string & material_ID, const int & surface_health = C::MAX_SURFACE_HEALTH);
	void add_door(const C::surface surface, const int & health, const std::string & material_ID, const std::string & faction_ID);

	// damage surface
	Update_Messages damage_surface(const std::string & surface_ID, const std::shared_ptr<Item> & equipped_item, const std::string & username);
	Update_Messages damage_door(const std::string & surface_ID, const std::shared_ptr<Item> & equipped_item, const std::string & username);

	// add and remove actors
	void add_actor(const character_id & actor_id);
	void remove_actor(const character_id & actor_id);
	void add_viewing_actor(const character_id & actor_id);
	void remove_viewing_actor(const character_id & actor_id);

	// printing
	std::string summary(const character_id & player_ID) const;

	friend bool operator==(std::unique_ptr<Room> & lhs, std::unique_ptr<Room> & rhs);
};

bool operator==(std::unique_ptr<Room> & lhs, std::unique_ptr<Room> & rhs);

#endif
