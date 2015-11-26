/* Jim Viebke
Feb 14, 2015 */

#include <fstream>

#include "craft.h"
#include "game.h"

int main()
{
	srand((unsigned)time(NULL)); // seed rand

	// write game directories to disk
	U::create_path_if_not_exists(C::game_directory);
	U::create_path_if_not_exists(C::room_directory);
	U::create_path_if_not_exists(C::user_data_directory);

	// write utilities to disk
#ifdef _WIN32
	U::to_file(C::game_directory + "/" + "move room data to delete folder.bat.txt",
		std::string("move C:/IslandMUD/rooms C:/IslandMUD/delete"));
	U::to_file(C::game_directory + "/" + "wipe delete folder.bat",
		std::string("del /f/s/q C:/IslandMUD/delete > nul") + "\n" + "rmdir /s/q C:/IslandMUD/delete");
#else
	// add Linux equivalent?
#endif

	Game game;

	// run the processing thread using the main thread
	game.processing_thread();
}
