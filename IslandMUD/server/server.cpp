
/* Jim Viebke
Sept 10, 2016 */

#include "server.h"

#include <iostream>

std::mutex Server::mutex;
bool Server::running;

void Server::start()
{
	std::lock_guard<std::mutex> lock(mutex);
	std::cout << "Server starting.\n";
	running = true;
}
void Server::shutdown()
{
	std::lock_guard<std::mutex> lock(mutex);
	std::cout << "Server shutting down.\n";
	running = false;
}

bool Server::is_running()
{
	std::lock_guard<std::mutex> lock(mutex);
	return running;
}
