
/* Jim Viebke
Nov 13, 2015 */

#ifndef THREADSAFE_BIDIRECTIONAL_MAP_H
#define THREADSAFE_BIDIRECTIONAL_MAP_H

#include <map>
#include <mutex>

namespace threadsafe
{
	template <typename A, typename B>
	class bidirectional_map
	{
	private:
		std::map<A, B> map_a_b;
		std::map<B, A> map_b_a;
		mutable std::mutex mutex; // this mutex can be locked by constant member functions

	public:
		bidirectional_map()
		{
			static_assert(!is_same<A, B>::value, "");
		}

		void insert(const std::pair<A, B> & data)
		{
			std::unique_lock<std::mutex> lock(mutex);
			map_a_b[data.first] = data.second;
			map_b_a[data.second] = data.first;
		}
		void insert(const std::pair<B, A> & data)
		{
			std::unique_lock<std::mutex> lock(mutex);
			map_a_b[data.second] = data.first;
			map_b_a[data.first] = data.second;
		}

		void erase(const A & a)
		{
			std::unique_lock<std::mutex> lock(mutex);

			const B b = map_a_b.find(a)->second;

			map_a_b.erase(a);
			map_b_a.erase(b);
		}
		void erase(const B & b)
		{
			std::unique_lock<std::mutex> lock(mutex);

			const A a = map_b_a.find(b)->second;

			map_b_a.erase(b);
			map_a_b.erase(a);
		}

		typename map<A, B>::const_iterator find(const A & a) const
		{
			std::unique_lock<std::mutex> lock(mutex);
			return map_a_b.find(a);
		}
		typename map<B, A>::const_iterator find(const B & b) const
		{
			std::unique_lock<std::mutex> lock(mutex);
			return map_b_a.find(b);
		}

		B get(const A & a) const
		{
			std::unique_lock<std::mutex> lock(mutex);
			return map_a_b.find(a)->second;
		}
		A get(const B & b) const
		{
			std::unique_lock<std::mutex> lock(mutex);
			return map_b_a.find(b)->second;
		}
	};
}

#endif
