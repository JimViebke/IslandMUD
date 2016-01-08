/* Jim Viebke
Feb 14, 2015 */

#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <string>
#include <map>
#include <vector>

class Constants; // "forward declaring"...

typedef Constants C; // ...to do this here

class Constants
{
public:
	// general
	static const int
		GROUND_INDEX,
		VIEW_DISTANCE,

		WORLD_X_DIMENSION,
		WORLD_Y_DIMENSION,
		WORLD_Z_DIMENSION,

		DEFAULT_SPAWN_X,
		DEFAULT_SPAWN_Y,
		DEFAULT_SPAWN_Z;

	static const unsigned
		GAME_PORT_NUMBER,
		GAME_MAP_PORT_NUMBER;

	// game data locations
	static const std::string
		game_directory,
		world_terrain_file_location,
		iron_deposit_map_file_location,
		limestone_deposit_map_file_location,
		room_directory,
		user_data_directory;

	// faction IDs
	static const std::string PC_FACTION_ID; // players ("PCs")
	static const std::string NPC_NEUTRAL_FACTION_ID; // neutral island inhabitants
	static const std::string NPC_HOSTILE_FACTION_ID; // antagonists

	// map symbols
	static const char
		OUT_OF_BOUNDS_CHAR,
		ERROR_CHAR,
		PLAYER_CHAR,
		OTHER_PLAYER_CHAR,
		NPC_NEUTRAL_CHAR,
		WATER_CHAR,
		LAND_CHAR,
		FOREST_CHAR,
		GENERIC_MINERAL_CHAR,
		ITEM_CHAR,
		CHEST_CHAR,
		TABLE_CHAR,
		RUBBLE_CHAR;

	static const std::string
		// engine signals
		GOOD_SIGNAL,
		BAD_SIGNAL,

		// item IDs
		BAD_ITEM_ID,

		BLOOMERY_ID,
		BLOOM_ID,
		FORGE_ID,
		ANVIL_ID,
		DEBRIS_ID, // collapsed ceilings

		// item IDs -> materials
		WOOD_ID,
		STONE_ID,
		VINE_ID,
		STICK_ID,
		BRANCH_ID,
		ARROW_ID,
		ARROWHEAD_ID,
		BOARD_ID,

		// item IDs -> other
		TREE_ID,
		CHEST_ID,
		TABLE_ID,
		LOG_ID,

		// item IDs -> equipment
		AXE_ID,
		STAFF_ID,
		SWORD_ID,
		BOW_ID,
		TORCH_ID,
		HAMMER_ID,

		// item IDs -> rocks and minerals
		IRON_DEPOSIT_ID, // when they are in the ground
		LIMESTONE_DEPOSIT_ID,

		IRON_ID, // once they are out of the ground
		LIMESTONE_ID,

		// none of these are actual item IDs, they exist for the parser to work with multi-word IDs
		KEYWORD_DEPOSIT,

		// action commands
		LOGIN_COMMAND,
		SAVE_COMMAND,
		SHOW_HELP_COMMAND,
		LEGEND_COMMAND,
		CRAFT_COMMAND,
		MINE_COMMAND,
		TAKE_COMMAND,
		EQUIP_COMMAND,
		ITEM_COMMAND, // view equipped item
		DEQUIP_COMMAND,
		MOVE_COMMAND,
		DROP_COMMAND,
		INSERT_COMMAND,
		FROM_COMMAND,
		CONSTRUCT_COMMAND, // room sides
		WAIT_COMMAND, // dev only. World is real-time.
		WITH_COMMAND,
		PRINT_RECIPES_COMMAND, // dev only. No full recipe printout in normal game.
		ATTACK_COMMAND,

		// direction commands/IDs
		NORTH,
		NORTH_EAST,
		EAST,
		SOUTH_EAST,
		SOUTH,
		SOUTH_WEST,
		WEST,
		NORTH_WEST,

		// floor and ceiling IDs + directions
		FLOOR,
		CEILING,

		UP,
		DOWN,

		// doors and walls
		SURFACE,
		WALL,
		DOOR,

		// room xml
		XML_ROOM,
		XML_IS_WATER,
		XML_ITEMS,
		XML_ITEM,
		XML_ITEM_ID,
		XML_ITEM_HEALTH,
		XML_ITEM_COUNT,
		XML_SURFACE,
		XML_SURFACE_HEALTH,
		XML_SURFACE_DIRECTION,
		XML_SURFACE_MATERIAL,
		XML_DOOR,
		XML_DOOR_HEALTH,
		XML_DOOR_MATERIAL,
		XML_DOOR_FACTION,
		XML_CHEST,
		XML_CHEST_HEALTH,
		XML_CHEST_FACTION_ID,
		XML_TABLE,
		XML_TABLE_HEALTH,

		// user data xml
		XML_USER_ACCOUNT,
		XML_USER_PASSWORD,
		XML_USER_STATUS,
		XML_USER_STATUS_CURRENT_HEALTH,
		XML_USER_LOCATION,
		XML_USER_LEVELS,

		// user level xml
		XML_LEVEL_SWORDSMANSHIP,
		XML_LEVEL_ARCHERY,
		XML_LEVEL_FOREST_VISIBILITY,
		XML_LEVEL_HEALTH_MAX,

		// user health xml
		XML_CURRENT_HEALTH;

	// item health/integrity
	static const int
		DEFAULT_ITEM_MIN_HEALTH,
		DEFAULT_ITEM_MAX_HEALTH;

	// map an item ID to its article so we can have "an axe" and "a forge", rather than use "a(n)" for all items
	static const std::map<std::string, std::string> articles;

	// map an item ID to its plural
	static const std::map<std::string, std::string> plurals;

	// int to char conversion
	static const std::vector<char> numbers;

	// surface information
	static const std::vector<std::string> surface_ids;
	static const std::vector<std::string> direction_ids;
	static const std::vector<std::string> primary_direction_ids; // NESW
	static const std::map<std::string, std::string> opposite_surface_id;
	static const std::map<std::string, std::string> opposite_direction_id;

	// maps material IDs to the material count required to make a surface
	// therefore also acts as the list of valid construction surfaces (see implementation)
	static const std::map<std::string, unsigned> SURFACE_REQUIREMENTS;
	static const std::map<std::string, unsigned> DOOR_REQUIREMENTS;

	static const char
		WALL_CHAR,
		DOOR_CHAR;

	// walls, ceiling, and floor - min and max health

	static const int
		MIN_SURFACE_HEALTH,
		MAX_SURFACE_HEALTH,
		MAX_DOOR_HEALTH;

	static const std::map<std::string, std::map<std::string, int>> damage_tables;

	// AI constants
	static const int
		AI_MAX_OBJECTIVE_ATTEMPTS; // the point at which an AI must return control

	static const int
		// AI movement costs
		AI_MOVEMENT_COST,
		AI_MOVEMENT_COST_DIAGONAL,
		AI_MOVEMENT_COST_FOREST,
		AI_MOVEMENT_COST_FOREST_DIAGONAL,

		// levels
		SWORDSMANSHIP_LEVEL_MIN,
		SWORDSMANSHIP_LEVEL_MAX,
		ARCHERY_LEVEL_MIN,
		ARCHERY_LEVEL_MAX,
		FOREST_VISIBILITY_LEVEL_MIN,
		FOREST_VISIBILITY_LEVEL_MAX,
		HEALTH_MIN, // explaination in source
		FULL_HEALTH_MIN,
		FULL_HEALTH_MAX;

	// AI objective keywords
	static const std::string
		AI_OBJECTIVE_ACQUIRE,
		AI_OBJECTIVE_GOTO,
		AI_OBJECTIVE_CONSTRUCT;

	static const int
		MAX_CHEST_HEALTH;

	// fortress generation bounds
	static const int
		FORTRESS_PARTITION_MIN_SIZE,
		FORTRESS_MIN_X,
		FORTRESS_MAX_X,
		FORTRESS_MIN_Y,
		FORTRESS_MAX_Y;

	// blacksmithing constants
	static const unsigned
		AMBIENT_AIR_TEMPERATURE; // air temperature (in Celcius)
};

#endif
