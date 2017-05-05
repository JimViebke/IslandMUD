
/* Jim Viebke
Sept 10, 2016 */

#include "server.h"

#include <iostream>

std::mutex Server::mutex;
std::condition_variable Server::cv;
bool Server::running;

void Server::start()
{
	std::lock_guard<std::mutex> lock(mutex);
	running = true;
	std::cout << "Server started.\n";
}
void Server::shutdown()
{
	std::lock_guard<std::mutex> lock(mutex);
	std::cout << "Server shutting down.\n";
	running = false;
	std::cout << "Server shut down.\n";
	cv.notify_all();
}

bool Server::is_running()
{
	std::lock_guard<std::mutex> lock(mutex);
	return running;
}

void Server::wait_for_shutdown()
{
	std::unique_lock<std::mutex> lock(mutex);
	while (running)
		cv.wait(lock);
}
