#pragma once

#include <vector>
#include <thread>
#include <mutex>
#include <queue>
#include <memory>

namespace thread_pool
{
	class task
	{
	protected:
		task();
	public:
		virtual void run() = 0;
	};

	class thread_pool
	{
	public:
		thread_pool(const unsigned count);
		
		void add_task(std::shared_ptr<task> new_task);

		size_t queue_size() const;

		void stop();
		void join_all();
				
	private:
		void work();
		std::vector<std::thread> threads;

		bool running = true;

		std::queue<std::shared_ptr<task>> queue;
		mutable std::mutex mutex;
		std::condition_variable cv;
	};
}
