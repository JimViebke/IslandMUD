/* Jim Viebke
Feb 14, 2015 */

#include <fstream>

#include "craft.h"
#include "game.h"

int main()
{
	using namespace std;
		
	srand((unsigned)time(NULL)); // seed rand

	// write game directories to disk
	U::create_path_if_not_exists(C::game_directory);
	U::create_path_if_not_exists(C::room_directory);
	U::create_path_if_not_exists(C::user_data_directory);

	// write utilities to disk
#ifdef _WIN32
	U::to_file(C::game_directory + "/" + "move room data to delete folder.bat.txt",
		string("move C:/IslandMUD/rooms C:/IslandMUD/delete"));
	U::to_file(C::game_directory + "/" + "wipe delete folder.bat",
		string("del /f/s/q C:/IslandMUD/delete > nul") + "\n" + "rmdir /s/q C:/IslandMUD/delete");
#else
	// add Linux equivalent?
#endif

	Game game;

	// instead of spawning a new thread for networking, run it in the main thread
	game.networking_thread();

}
