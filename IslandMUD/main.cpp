/* Jim Viebke
Feb 14, 2015 */

#include "craft.h"
#include "game.h"

int main()
{
	srand((unsigned)time(NULL)); // seed rand

	// write game directories to disk
	U::create_path_if_not_exists(C::game_directory);
	U::create_path_if_not_exists(C::room_directory);
	U::create_path_if_not_exists(C::user_data_directory);

	// create the game object
	Game game;

	// start the game's main logic from the main thread
	game.run();
}
