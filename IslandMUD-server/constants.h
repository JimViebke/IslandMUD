/* Jim Viebke
Feb 14, 2015 */

#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <string>
#include <map>
#include <vector>

using namespace std;

class Constants
{
public:
	// game data locations
	static const string game_directory;
	static const string world_terrain_file_location;
	static const string world_biome_file_location;
	static const string room_directory;
	static const string user_data_directory;

	const static char
		WATER_CHAR,
		LAND_CHAR,
		FOREST_CHAR,
		ITEM_CHAR;

	const static int
		GROUND_INDEX,
		VIEW_DISTANCE,

		WORLD_X_DIMENSION,
		WORLD_Y_DIMENSION,
		WORLD_Z_DIMENSION,

		DEFAULT_SPAWN_X,
		DEFAULT_SPAWN_Y,
		DEFAULT_SPAWN_Z;

	const static string
		GOOD_SIGNAL,
		BAD_SIGNAL,
		BAD_ITEM_ID,

		WOOD_ID,
		STONE_ID,
		VINE_ID,

		STICK_ID,
		BRANCH_ID,

		TREE_ID,

		AXE_ID,
		STAFF_ID,
		SWORD_ID,
		BOW_ID,
		TORCH_ID,
		HAMMER_ID,
		FORGE_ID,
		ANVIL_ID,

		BAD_COMMAND,
		LOGIN_COMMAND,
		LOGOUT_COMMAND,
		HELP_COMMAND,
		CRAFT_COMMAND,
		TAKE_COMMAND,
		EQUIP_COMMAND,
		DEQUIP_COMMAND,
		MOVE_COMMAND,
		DROP_COMMAND,
		CONSTRUCT_COMMAND, // room sides
		WAIT_COMMAND, // dev only. World is real-time.
		PRINT_RECIPES_COMMAND, // dev only. No full recipe printout in normal game.
		ATTACK_COMMAND,

		NORTH,
		NORTH_EAST,
		EAST,
		SOUTH_EAST,
		SOUTH,
		SOUTH_WEST,
		WEST,
		NORTH_WEST,

		FLOOR,
		CEILING,

		UP,
		DOWN,

		// room xml
		XML_ROOM,
		XML_ITEM,
		XML_SURFACE,
		XML_SURFACE_HEALTH,
		XML_SURFACE_DIRECTION,
		XML_SURFACE_MATERIAL,

		// user data xml
		XML_USER_LOCATION,
		XML_USER_EQUIPMENT,
		XML_USER_MATERIALS,
		XML_USER_MATERIAL_COUNT;

	// surface information

	static const vector<string> surface_ids;
	static const vector<string> direction_ids;
	static const map<string, string> opposite_surface_id;

	// maps material IDs to the material count required to make a surface
	// therefore also acts as the list of valid construction surfaces (see implementation) 
	const static map<string, unsigned> SURFACE_REQUIREMENTS;

	// box drawing

	const static char
		NW_CORNER,
		NE_CORNER,
		SW_CORNER,
		SE_CORNER,
		NS_WALL,
		WE_WALL;

	const static int
		MIN_SURFACE_HEALTH,
		MAX_SURFACE_HEALTH;

	const static map<string, map<string, int>> damage_tables;
};

typedef Constants C;

#endif
