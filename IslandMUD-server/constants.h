/* Jim Viebke
Feb 14, 2015 */

#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <string>
#include <map>

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
		BOW_ID,
		TORCH_ID,

		BAD_COMMAND,
		LOGIN_COMMAND,
		LOGOUT_COMMAND,
		GET_HELP_COMMAND,
		CRAFT_COMMAND,
		TAKE_COMMAND,
		MOVE_COMMAND,
		DROP_COMMAND,
		CONSTRUCT_COMMAND, // room sides
		WAIT_COMMAND, // dev only. World is real-time.
		PRINT_RECIPES_COMMAND, // dev only. No full recipe printout in normal game.

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

		// world
		XML_ROOM,
		XML_ITEM,
		XML_SURFACE,
		XML_DIRECTION,
		XML_MATERIAL,

		// user
		XML_USER_LOCATION,
		XML_USER_EQUIPMENT,
		XML_USER_MATERIALS,
		XML_USER_MATERIAL_COUNT;

	// maps material IDs to the material count required to make a surface
	// therefore also acts as the list of valid construction surfaces (see implementation) 
	static map<string, unsigned> SURFACE_REQUIREMENTS;

	// box drawing

	const static char
		NW_CORNER,
		NE_CORNER,
		SW_CORNER,
		SE_CORNER,
		NS_WALL,
		WE_WALL;

};

typedef Constants C;

#endif
