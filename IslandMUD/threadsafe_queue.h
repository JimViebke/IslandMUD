
// Created by Samvel Khalatyan on Apr 28, 2014
// Copyright (c) 2014 Samvel Khalatyan. All rights reserved
//
// The MIT License(MIT)
//
// Copyright(c) 2014 Samvel Khalatyan
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef THREADSAFE_QUEUE_H
#define THREADSAFE_QUEUE_H

#include <chrono>
#include <condition_variable>
#include <mutex>
#include <queue>

namespace threadsafe
{
	template<typename T>
	class queue
		/*
		* thread-safe queue
		*
		* It has one potential problem: the get will stall the
		* consuming thread if producer stops adding elements, e.g.
		* there are N + 1  calls to get(...) with N calls to put().
		*/
	{
	public:
		queue() {}

		void put(const T & value)
		{
			std::unique_lock<std::mutex> lock(_mutex);
			_queue.push(value);
			_cv.notify_one();
		}

		void get(T & dest)
		{
			std::unique_lock<std::mutex> lock(_mutex);
			_cv.wait(lock, [this] { return !_queue.empty(); });
			dest = _queue.front();
			_queue.pop();
		}

		T get()
		{
			std::unique_lock<std::mutex> lock(_mutex);
			_cv.wait(lock, [this] { return !_queue.empty(); });
			T data = _queue.front();
			_queue.pop();
			return data;
		}

		bool empty() const
		{
			std::unique_lock<std::mutex> lock(_mutex);
			return _queue.empty();
		}

		size_t size() const
		{
			std::unique_lock<std::mutex> lock(_mutex);
			return _queue.size();
		}

	protected:
		std::mutex _mutex;
		std::condition_variable _cv;
		std::queue<T> _queue;
	};
}

#endif
