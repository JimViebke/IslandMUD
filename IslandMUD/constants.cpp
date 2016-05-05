/* Jim Viebke
Feb 14, 2015 */

#include <iostream>

#include "constants.h"

// general
const int C::GROUND_INDEX = 3; // there are three levels below this
const int C::VIEW_DISTANCE = 9; // 5+1+5 to a side == 11*11 total area

const int C::WORLD_X_DIMENSION = 1000;
const int C::WORLD_Y_DIMENSION = 1000;
const int C::WORLD_Z_DIMENSION = 8;

const int C::DEFAULT_SPAWN_X = C::WORLD_X_DIMENSION / 2; // N/S center
const int C::DEFAULT_SPAWN_Y = C::WORLD_Y_DIMENSION / 2; // E/W center
const int C::DEFAULT_SPAWN_Z = C::GROUND_INDEX; // ground level spawn

const unsigned C::GAME_PORT_NUMBER = 8050;
const unsigned C::GAME_MAP_PORT_NUMBER = 8051;

// game data locations
#ifdef _WIN32
const std::string C::game_directory = "C:/IslandMUD"; // Windows path
#else
const std::string C::game_directory = "/home/IslandMUD"; // Linux path
#endif
const std::string C::world_terrain_file_location = C::game_directory + "/world_terrain.txt";
const std::string C::iron_deposit_map_file_location = C::game_directory + "/iron_deposit_map.txt";
const std::string C::limestone_deposit_map_file_location = C::game_directory + "/limestone_deposit_map.txt";
const std::string C::room_directory = C::game_directory + "/rooms";
const std::string C::user_data_directory = C::game_directory + "/user_data";

// faction IDs
const std::string C::PC_FACTION_ID = "player";
const std::string C::NPC_NEUTRAL_FACTION_ID = "neutral_NPC";
const std::string C::NPC_HOSTILE_FACTION_ID = "hostile_NPC";

// map symbols
const char C::OUT_OF_BOUNDS_CHAR = '*';
const char C::ERROR_CHAR = '!';
const char C::PLAYER_CHAR = '@';
const char C::OTHER_PLAYER_CHAR = 'a';
const char C::NPC_NEUTRAL_CHAR = '+';
const char C::WATER_CHAR = '~';
const char C::LAND_CHAR = ' ';
const char C::FOREST_CHAR = '%';
const char C::GENERIC_MINERAL_CHAR = 'm';
const char C::ITEM_CHAR = '?';
const char C::CHEST_CHAR = 'c';
const char C::TABLE_CHAR = 't';
const char C::RUBBLE_CHAR = '!';
const char C::WALL_CHAR = '#';
const char C::DOOR_CHAR = '/';

// engine signals
const std::string C::GOOD_SIGNAL = "good";
const std::string C::BAD_SIGNAL = "bad";

// item IDs

const std::string C::BLOOMERY_ID = "bloomery";
const std::string C::BLOOM_ID = "bloom";
const std::string C::FORGE_ID = "forge";
const std::string C::ANVIL_ID = "anvil";
const std::string C::DEBRIS_ID = "debris";

// item IDs -> materials
const std::string C::WOOD_ID = "wood";
const std::string C::STONE_ID = "stone";
const std::string C::VINE_ID = "vine";
const std::string C::STICK_ID = "stick";
const std::string C::BRANCH_ID = "branch";
const std::string C::ARROW_ID = "arrow";
const std::string C::ARROWHEAD_ID = "arrowhead";
const std::string C::BOARD_ID = "board";

// item IDs -> other
const std::string C::TREE_ID = "tree";
const std::string C::CHEST_ID = "chest";
const std::string C::TABLE_ID = "table";
const std::string C::LOG_ID = "log";

// item IDs -> equipment
const std::string C::AXE_ID = "axe";
const std::string C::STAFF_ID = "staff";
const std::string C::SWORD_ID = "sword";
const std::string C::BOW_ID = "bow";
const std::string C::TORCH_ID = "torch";
const std::string C::HAMMER_ID = "hammer";

// item IDs -> rocks and minerals
const std::string C::IRON_DEPOSIT_ID = "iron deposit"; // when they are in the ground
const std::string C::LIMESTONE_DEPOSIT_ID = "limestone deposit";

const std::string C::IRON_ID = "iron"; // once they are out of the ground
const std::string C::LIMESTONE_ID = "limestone";

// none of these are actual item IDs, they exist for the parser to work with multi-word IDs
const std::string C::KEYWORD_DEPOSIT = "deposit";

// action commands
const std::string C::LOGIN_COMMAND = "login";
const std::string C::SAVE_COMMAND = "save";
const std::string C::SHOW_HELP_COMMAND = "help";
const std::string C::LEGEND_COMMAND = "legend";
const std::string C::INVENTORY_COMMAND = "inventory";
const std::string C::LOOK_COMMAND = "look";
const std::string C::TAKE_COMMAND = "take";
const std::string C::EQUIP_COMMAND = "equip";
const std::string C::ITEM_COMMAND = "item"; // view equipped item
const std::string C::DEQUIP_COMMAND = "dequip";
const std::string C::CRAFT_COMMAND = "craft";
const std::string C::MINE_COMMAND = "mine";
const std::string C::MOVE_COMMAND = "move";
const std::string C::DROP_COMMAND = "drop";
const std::string C::INSERT_COMMAND = "insert";
const std::string C::FROM_COMMAND = "from";
const std::string C::CONSTRUCT_COMMAND = "construct";
const std::string C::WAIT_COMMAND = "wait"; // for debugging (see constants.h)
const std::string C::WITH_COMMAND = "with";
const std::string C::PRINT_RECIPES_COMMAND = "recipes"; // for debugging (see constants.h)
const std::string C::ATTACK_COMMAND = "attack";

const std::string C::ALL_COMMAND = "all"; // quantity

// direction commands/IDs
const std::string C::NORTH = "north";
const std::string C::NORTH_EAST = "northeast";
const std::string C::EAST = "east";
const std::string C::SOUTH_EAST = "southeast";
const std::string C::SOUTH = "south";
const std::string C::SOUTH_WEST = "southwest";
const std::string C::WEST = "west";
const std::string C::NORTH_WEST = "northwest";

// floor and ceiling IDs + directions
const std::string C::FLOOR = "floor";
const std::string C::CEILING = "ceiling";

const std::string C::UP = "up";
const std::string C::DOWN = "down";

// doors and walls
const std::string C::SURFACE = "surface";
const std::string C::WALL = "wall";
const std::string C::DOOR = "door";

// room xml
const std::string C::XML_ROOM = "room";
const std::string C::XML_IS_WATER = "is_water";
const std::string C::XML_ITEMS = "items";
const std::string C::XML_ITEM = "item";
const std::string C::XML_ITEM_ID = "item_ID";
const std::string C::XML_ITEM_HEALTH = "health";
const std::string C::XML_ITEM_COUNT = "count";
const std::string C::XML_SURFACE = "surface";
const std::string C::XML_SURFACE_HEALTH = "health";
const std::string C::XML_SURFACE_DIRECTION = "direction";
const std::string C::XML_SURFACE_MATERIAL = "material";
const std::string C::XML_DOOR = "door";
const std::string C::XML_DOOR_HEALTH = "health";
const std::string C::XML_DOOR_MATERIAL = "material";
const std::string C::XML_DOOR_FACTION = "faction";
const std::string C::XML_CHEST = "chest";
const std::string C::XML_CHEST_HEALTH = "health";
const std::string C::XML_CHEST_FACTION_ID = "faction_id";
const std::string C::XML_TABLE = "table";
const std::string C::XML_TABLE_HEALTH = "health";

// user data xml
const std::string C::XML_USER_ACCOUNT = "account";
const std::string C::XML_USER_PASSWORD = "password";
const std::string C::XML_USER_STATUS = "status";
const std::string C::XML_USER_STATUS_CURRENT_HEALTH = "current_health";
const std::string C::XML_USER_LOCATION = "location";
const std::string C::XML_USER_LEVELS = "levels";

// user level xml
const std::string C::XML_LEVEL_SWORDSMANSHIP = "swordsmanship";
const std::string C::XML_LEVEL_ARCHERY = "archery";
const std::string C::XML_LEVEL_FOREST_VISIBILITY = "forest_visibility";
const std::string C::XML_LEVEL_HEALTH_MAX = "max_health";

// user health xml
const std::string C::XML_CURRENT_HEALTH = "current_health";

// item health/integrity
const int C::DEFAULT_ITEM_MIN_HEALTH = 0;
const int C::DEFAULT_ITEM_MAX_HEALTH = 100;

namespace // anonymous namespace to prevent access outside this file
{
	std::map<std::string, std::string> generate_articles_map()
	{
		std::cout << "\nLoading article lookup...";

		std::map<std::string, std::string> temp_articles;

		// item IDs

		temp_articles[C::ANVIL_ID] =
			temp_articles[C::ARROWHEAD_ID] =
			temp_articles[C::ARROW_ID] =
			temp_articles[C::AXE_ID] =
			temp_articles[C::IRON_ID] =
			temp_articles[C::IRON_DEPOSIT_ID] = "an";

		temp_articles[C::BLOOM_ID] =
			temp_articles[C::BLOOMERY_ID] =
			temp_articles[C::BOARD_ID] =
			temp_articles[C::BOW_ID] =
			temp_articles[C::BRANCH_ID] =
			temp_articles[C::CHEST_ID] =
			temp_articles[C::FORGE_ID] =
			temp_articles[C::HAMMER_ID] =
			temp_articles[C::LIMESTONE_ID] =
			temp_articles[C::LIMESTONE_DEPOSIT_ID] =
			temp_articles[C::LOG_ID] =
			temp_articles[C::STAFF_ID] =
			temp_articles[C::STICK_ID] =
			temp_articles[C::STONE_ID] =
			temp_articles[C::SWORD_ID] =
			temp_articles[C::TABLE_ID] =
			temp_articles[C::TORCH_ID] =
			temp_articles[C::TREE_ID] =
			temp_articles[C::VINE_ID] =
			temp_articles[C::WOOD_ID] = "a";

		// direction IDs

		temp_articles[C::NORTH] =
			temp_articles[C::WEST] =
			temp_articles[C::SOUTH] = "a";

		temp_articles[C::EAST] = "an";

		return temp_articles;
	}

	std::map<std::string, std::string> generate_plurals_map()
	{
		std::cout << "\nLoading plurals lookup...";

		std::map<std::string, std::string> temp_plurals;

		// item IDs

		temp_plurals[C::ANVIL_ID] = "anvils";
		temp_plurals[C::ARROWHEAD_ID] = "arrowheads";
		temp_plurals[C::ARROW_ID] = "arrows";
		temp_plurals[C::AXE_ID] = "axes";
		// temp_plurals[C::IRON_ID] = "";
		// temp_plurals[C::IRON_DEPOSIT_ID] = "an";

		temp_plurals[C::BLOOM_ID] = "bloom";
		temp_plurals[C::BLOOMERY_ID] = "bloomeries";
		temp_plurals[C::BOARD_ID] = "boards";
		temp_plurals[C::BOW_ID] = "bows";
		temp_plurals[C::BRANCH_ID] = "branches";
		temp_plurals[C::FORGE_ID] = "forges";
		temp_plurals[C::HAMMER_ID] = "hammers";
		// temp_plurals[C::LIMESTONE_ID] = "";
		// temp_plurals[C::LIMESTONE_DEPOSIT_ID] = "";
		temp_plurals[C::LOG_ID] = "logs";
		temp_plurals[C::STAFF_ID] = "staves";
		temp_plurals[C::STICK_ID] = "sticks";
		temp_plurals[C::STONE_ID] = "stones";
		temp_plurals[C::SWORD_ID] = "swords";
		temp_plurals[C::TORCH_ID] = "torches";
		temp_plurals[C::TREE_ID] = "trees";
		temp_plurals[C::VINE_ID] = "vines";
		// temp_plurals[C::WOOD_ID] = "";

		return temp_plurals;
	}
}

const std::map<std::string, std::string> C::articles = generate_articles_map();
const std::map<std::string, std::string> C::plurals = generate_plurals_map();

// store a vector of all valid surface ids
const std::vector<std::string> C::surface_ids = { C::NORTH, C::EAST, C::SOUTH, C::WEST, C::FLOOR, C::CEILING };

// store a vector of all valid movement directions
const std::vector<std::string> C::direction_ids = {
	C::NORTH, C::EAST, C::SOUTH, C::WEST,
	C::NORTH_EAST, C::NORTH_WEST,
	C::SOUTH_EAST, C::SOUTH_WEST,
	C::UP, C::DOWN };

const std::vector<std::string> C::primary_direction_ids = {
	C::NORTH, C::EAST, C::SOUTH, C::WEST };

// map surface ids to their opposite surface
const std::map<std::string, std::string> C::opposite_surface_id = {
	{ C::NORTH, C::SOUTH },
	{ C::SOUTH, C::NORTH },
	{ C::EAST, C::WEST },
	{ C::WEST, C::EAST },
	{ C::CEILING, C::FLOOR },
	{ C::FLOOR, C::CEILING },
};

const std::map<std::string, std::string> C::opposite_direction_id = {
	{ C::NORTH, C::SOUTH },
	{ C::SOUTH, C::NORTH },
	{ C::EAST, C::WEST },
	{ C::WEST, C::EAST },
	{ C::NORTH_EAST, C::SOUTH_WEST },
	{ C::NORTH_WEST, C::SOUTH_EAST },
	{ C::SOUTH_EAST, C::NORTH_WEST },
	{ C::SOUTH_WEST, C::NORTH_EAST }
};

// Amount of a resource required to construct a surface (wall/ceiling/floor) of that type
const std::map<std::string, unsigned> C::SURFACE_REQUIREMENTS =
{
	{ C::WOOD_ID, 5 }, // {material, count required}
	{ C::STONE_ID, 5 },
	{ C::STICK_ID, 5 },
	{ C::BRANCH_ID, 5 }
};

// Amount of a resource required to construct a door of that type
const std::map<std::string, unsigned> C::DOOR_REQUIREMENTS =
{
	{ C::WOOD_ID, 5 }, // {material, count required}
	{ C::STONE_ID, 5 },
	{ C::STICK_ID, 5 },
	{ C::BRANCH_ID, 5 }
};

// walls, ceiling, and floor - min and max health

const int C::MIN_SURFACE_HEALTH = 0;
const int C::MAX_SURFACE_HEALTH = 100;
const int C::MAX_DOOR_HEALTH = 100;

/* Create a two-dimensional map to calculate damamge values.
Associate each attacking implement with a map of target, value pairs. */

const std::map<std::string, std::map<std::string, int>> C::damage_tables =
{
	// unarmed attacks are deliberately set high for testing

	{ std::string(C::ATTACK_COMMAND), { // attack command is also used to represent an unarmed attack
		{ C::STICK_ID, 49 }, // formerly 6
		{ C::WOOD_ID, 49 }, // formerly 3
		{ C::STONE_ID, 49, }, // formerly 1
		{ C::BRANCH_ID, 49 },
		{ C::TREE_ID, 49 } // possibly remove this
	} },

	{ std::string(C::STAFF_ID), {
		{ C::STICK_ID, 8 },
		{ C::WOOD_ID, 5 },
		{ C::STONE_ID, 1 }
	} },

	{ std::string(C::AXE_ID), {
		{ C::STICK_ID, 10 },
		{ C::WOOD_ID, 10 },
		{ C::STONE_ID, 5 },
		{ C::TREE_ID, 33 } // also set high for testing
	} },

	{ std::string(C::SWORD_ID), {
		{ C::STICK_ID, 8 },
		{ C::WOOD_ID, 7 },
		{ C::STONE_ID, 2 }
	} }
};

// AI constants
const int C::AI_MAX_OBJECTIVE_ATTEMPTS = 50; // the point at which an AI must return control

// AI movement costs
const int C::AI_MOVEMENT_COST = 10; // 1 * 10
const int C::AI_MOVEMENT_COST_DIAGONAL = 14; // sqrt(2) * 10
const int C::AI_MOVEMENT_COST_FOREST = C::AI_MOVEMENT_COST * 2;
const int C::AI_MOVEMENT_COST_FOREST_DIAGONAL = C::AI_MOVEMENT_COST_DIAGONAL * 2;

// levels
const int C::SWORDSMANSHIP_LEVEL_MIN = 10; // damage inflicted per strike
const int C::SWORDSMANSHIP_LEVEL_MAX = 50;
const int C::ARCHERY_LEVEL_MIN = 50; // odds of a shot making contact
const int C::ARCHERY_LEVEL_MAX = 90;
const int C::FOREST_VISIBILITY_LEVEL_MIN = 10; // odds of seeing a hostile that is in a forest room (redrawn for every move of either player
const int C::FOREST_VISIBILITY_LEVEL_MAX = 90;
const int C::HEALTH_MIN = 0;
const int C::FULL_HEALTH_MIN = 100; // this is full health for new players
const int C::FULL_HEALTH_MAX = 200; // this is full health for fully leveled health

// AI objective keywords
const std::string C::AI_OBJECTIVE_ACQUIRE = "acquire";
const std::string C::AI_OBJECTIVE_GOTO = "goto";
const std::string C::AI_OBJECTIVE_CONSTRUCT = "construct";

// chests
const int C::MAX_CHEST_HEALTH = 100;

// fortress generation

const int C::FORTRESS_PARTITION_MIN_SIZE = 3;
const int C::FORTRESS_MIN_X = 20;
const int C::FORTRESS_MAX_X = 20;
const int C::FORTRESS_MIN_Y = 20;
const int C::FORTRESS_MAX_Y = 20;

const unsigned C::AMBIENT_AIR_TEMPERATURE = 15;
