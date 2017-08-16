/* Jim Viebke
Feb 14, 2015 */

#include "craft.h"
#include "game.h"

#include "threadsafe\threadsafe_map.hpp"

class Object
{
public:
	int i1, i2, i3, i4;
};

threadsafe::map<int, std::string> m1;

void test()
{

	auto write_lock = m1.get_write_lock();

	std::string test = "lalala";
	m1.insert(34, test, write_lock);

	auto x = m1.find_by_value("lalala");
}

int main()
{
	test();
	return TRUE;

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
