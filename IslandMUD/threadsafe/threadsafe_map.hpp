#pragma once

/* Jim Viebke
July 2017

A generic, threadsafe <K,V> map designed to look a lot like std::map.
The user must acquire lock handles in order to performs calls.
When a handle is acquired, multiple calls can be made. The lock is released
when the handle falls out of scope.

The most obvious shortcoming of this class is that a write handle does not act
as a read handle. This should be fixed, perhaps with an intermediate type with
read permissions. */

#include <chrono>
#include <condition_variable>
#include <shared_mutex>
#include <map>

namespace threadsafe
{
	// Cannot be instantiated or thrown. Can be caught as a parent.
	class threadsafe_exception : std::exception
	{
	protected:
		threadsafe_exception() {}
	};

	//class key_not_found_exception : threadsafe_exception { public: };
	class value_not_found_exception : threadsafe_exception { public: };
	//class key_already_exists_exception : threadsafe_exception { public: };
	class not_my_handle_exception : threadsafe_exception { public: };

	template<typename K, typename V>
	class map
	{
	public:
		map() {}

		//using const_iterator = std::map<K, V>::const_iterator;

		class handle
		{
		private:
			std::shared_ptr<char> _handle;

			handle()
			{
				_handle = std::make_shared<char>(' ');
			}

			handle(const handle & rhs)
			{
				_handle = rhs._handle;
			}

		public:
			bool operator==(const handle & rhs)
			{
				// we don't want to know if the pointers have the same value
				// we want to know if their owned data occupy the same space in memory
				return &*_handle == &*rhs._handle;
			}
			bool operator!=(const handle & rhs)
			{
				return !(*this == rhs);
			}

			friend class map;
		};

		using shared_mutex = std::shared_mutex;

		class lock_type
		{
		protected:
			handle _handle; // use to determine which if this lock belongs to a certain map
			lock_type(const handle & set_handle) : _handle(set_handle) {}
		private:
			lock_type(const lock_type &) = delete;

			friend class threadsafe::map;
			friend class std::shared_ptr<lock_type>;
			friend class std::_Ref_count_obj<lock_type>;
		};

		// these can only be instantiated by threadsafe::map
		class read_lock : public lock_type
		{
		private:
			std::shared_lock<shared_mutex> lock;

			read_lock(const read_lock &) = delete; // no copying
			read_lock(shared_mutex & mutex, const handle & set_handle) : lock_type(set_handle), lock(mutex) {}

			friend class threadsafe::map;
			friend class std::shared_ptr<read_lock>;
			friend class std::_Ref_count_obj<read_lock>;
		};

		// a write lock is also a read lock
		class write_lock : public read_lock
		{
		private:
			write_lock(const write_lock &) = delete;
			write_lock(shared_mutex & mutex, const handle & set_handle) : read_lock(mutex, set_handle) {}

			friend class threadsafe::map;
			friend class std::shared_ptr<write_lock>;
			friend class std::_Ref_count_obj<write_lock>;
		};
		using lock_ptr = std::shared_ptr<lock_type>;
		using read_lock_ptr = std::shared_ptr<read_lock>;
		using write_lock_ptr = std::shared_ptr<write_lock>;

		// blocking
		read_lock_ptr get_read_lock() const
		{
			return std::make_shared<read_lock>(_mutex, _handle);
		}

		// blocking
		write_lock_ptr get_write_lock()
		{
			return std::make_shared<write_lock>(_mutex, _handle);
		}

		// good:

		void insert(const K & key, const V & value, const write_lock_ptr & lock)
		{
			check(lock);
			_map[key] = value;
		}

		const V& get(const K & key, const read_lock_ptr & lock) const
		{
			check(lock);
			return _map.find(key)->second;
		}
		V& get(const K & key, const write_lock_ptr & lock)
		{
			check(lock);
			return _map[key];
		}

		void erase(const K & key, const write_lock_ptr & lock)
		{
			check(lock);
			_map.erase(key);
		}

		bool contains(const K & key, const read_lock_ptr & lock) const
		{
			check(lock);
			return _map.find(key) != _map.cend();
		}

		size_t size(const read_lock_ptr & lock) const
		{
			check(lock);
			return _map.size();
		}

		auto find_by_value(const V & value)
		{
			for (const auto & it : _map)
				if (it.second == value)
					return it;

			throw value_not_found_exception();
		}

		auto begin() const
		{
			return _map.begin();
		}
		auto end() const
		{
			return _map.end();
		}

		auto cbegin() const
		{
			return _map.cbegin();
		}
		auto cend() const
		{
			return _map.cend();
		}

	private:
		// this should be called every time the user passes a lock to this class
		void check(const lock_ptr & lock) const
		{
			if (lock->_handle != _handle) throw not_my_handle_exception();
		}

		std::map<K, V> _map;

		// used to construct reader and writer handles to this object
		mutable shared_mutex _mutex;

		// used to determine if read and write locks belong to this map or another map
		handle _handle;
	};

}
