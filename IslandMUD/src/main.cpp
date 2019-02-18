/* Jim Viebke
Feb 14, 2015 */

#include "craft.h"
#include "game.h"

int main()
{
	// seed rand
	srand((unsigned)time(NULL));

	// disable syncing with printf
	std::ios_base::sync_with_stdio(false);

	// write game directories to disk
	U::create_path_if_not_exists(C::game_directory);
	U::create_path_if_not_exists(C::room_directory);
	U::create_path_if_not_exists(C::user_data_directory);

	// create the game object
	Game game;

	// start the game's main logic from the main thread
	game.run();
}
