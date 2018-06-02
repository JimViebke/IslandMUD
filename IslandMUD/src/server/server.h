#pragma once

/* Jim Viebke
Sept 10, 2016

Create threadsafe signals to communicate server status between threads. */

#include <mutex>

class Server
{
private:

	static std::mutex mutex;
	static std::condition_variable cv;
	static bool running;

	Server() = delete;

public:

	static void start();
	static void shutdown();

	static bool is_running();
	static void wait_for_shutdown();
};
