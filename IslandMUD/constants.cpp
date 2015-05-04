/* Jim Viebke
Feb 14, 2015 */

#include "constants.h"

const string C::game_directory = "C:/IslandMUD";
const string C::room_directory = C::game_directory + "/rooms";
const string C::world_terrain_file_location = C::game_directory + "/world_terrain.txt";
const string world_biome_file_location = C::game_directory + "/biome_map.txt";

const char C::WATER_CHAR = '~';
const char C::LAND_CHAR = ' ';
const char C::FOREST_CHAR = '%';
const char C::ITEM_CHAR = '?';

const int C::GROUND_INDEX = 3; // there are three levels below this
const int C::VIEW_DISTANCE = 5; // 5+1+5 to a side == 11*11 total area

const int C::WORLD_X_DIMENSION = 100;
const int C::WORLD_Y_DIMENSION = 100;
const int C::WORLD_Z_DIMENSION = 10;

const int C::DEFAULT_SPAWN_X = C::WORLD_X_DIMENSION / 2; // N/S center
const int C::DEFAULT_SPAWN_Y = C::WORLD_Y_DIMENSION / 2; // E/W center
const int C::DEFAULT_SPAWN_Z = C::GROUND_INDEX; // ground level spawn

// engine signals

const string C::GOOD_SIGNAL = "good";
const string C::BAD_SIGNAL = "bad";

// item idsf

const string C::BAD_ITEM_ID = "bad_item_id";

// materials

const string C::WOOD_ID = "wood";
const string C::STONE_ID = "stone";
const string C::VINE_ID = "vine";
const string C::STICK_ID = "stick";
const string C::BRANCH_ID = "branch";

const string C::TREE_ID = "tree";

// equipment

const string C::AXE_ID = "axe";
const string C::STAFF_ID = "staff";
const string C::BOW_ID = "bow";
const string C::TORCH_ID = "torch";

// verb commands

const string C::BAD_COMMAND = "unknown_command";
const string C::GET_HELP_COMMAND = "help";
const string C::TAKE_COMMAND = "take";
const string C::CRAFT_COMMAND = "craft";
const string C::MOVE_COMMAND = "move";
const string C::DROP_COMMAND = "drop";
const string C::CONSTRUCT_COMMAND = "construct";
const string C::WAIT_COMMAND = "wait"; // for debugging (see .h)
const string C::PRINT_RECIPES_COMMAND = "recipes"; // for debugging (see .h)

// direction commands

const string C::NORTH = "north";
const string C::NORTH_EAST = "northeast";
const string C::EAST = "east";
const string C::SOUTH_EAST = "southeast";
const string C::SOUTH = "south";
const string C::SOUTH_WEST = "southwest";
const string C::WEST = "west";
const string C::NORTH_WEST = "northwest";

// floor and ceiling IDs (walls use NESW)

const string C::FLOOR = "floor";
const string C::CEILING = "ceiling";

const string C::UP = "up";
const string C::DOWN = "down";

const string C::XML_ROOM = "room";
const string C::XML_ITEM = "item";
const string C::XML_SURFACE = "surface";
const string C::XML_DIRECTION = "direction";
const string C::XML_MATERIAL = "material";

// surface material count requirements

map<string, unsigned> C::SURFACE_REQUIREMENTS =
{
	{ C::WOOD_ID, 5 }, // {material, count required}
	{ C::STONE_ID, 5 },
	{ C::STICK_ID, 5 },
	{ C::BRANCH_ID, 5 }
};

// box drawing

/* Exploded view

╔ ═ ╗

║   ║

╚ ═ ╝ */

const char C::NW_CORNER = char(201);
const char C::NE_CORNER = char(187);
const char C::SW_CORNER = char(200);
const char C::SE_CORNER = char(188);
const char C::NS_WALL = char(186);
const char C::WE_WALL = char(205);
