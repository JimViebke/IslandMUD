/* Jim Viebke
April 1, 2014 */

#ifndef WORLD_H
#define WORLD_H

#include <vector>
#include <memory>
#include <map>

#include "XML/pugixml.hpp"

#include "room.h"
#include "generator.h"
#include "threadsafe/threadsafe_queue.h"

class World
{
private:
	
	// 2d terrain (biome) map
	std::unique_ptr<std::vector<std::vector<char>>> terrain;
	std::unique_ptr<std::vector<std::vector<char>>> iron_deposit_map;
	std::unique_ptr<std::vector<std::vector<char>>> limestone_deposit_map;

	std::vector<std::unique_ptr<Room>> world;

	/*
	The "main" thread (the thread making calls to World) doesn't have to pay the I/O cost of unloading rooms.
	The world object internally contains a background thread that does the unloading work.
	This comes with the tricky caveat that if a room is still in the queue, and that room is requested to be loaded,
	the room has to be "loaded" from the queue, not from disk. */
	std::thread background_room_unloading_thread; // the thread
	std::deque<std::unique_ptr<Room>> unload_queue; // the queue
	std::mutex unload_queue_mutex; // a mutex for gaurding the queue
	std::condition_variable unload_queue_cv; // a cv so the main thread can wake the background unloading thread

public:

	World();

	// access a room given coordinates
	std::unique_ptr<Room>::pointer room_at(const Coordinate & coordinate);
	const std::unique_ptr<Room>::pointer room_at(const Coordinate & coordinate) const;
	std::unique_ptr<Room> & room_pointer_at(const Coordinate & coordinate);

	// debugging
	unsigned count_loaded_rooms() const;

	// load rooms around a player spawning in
	void load_view_radius_around(const Coordinate & coordinate, const std::string & character_ID);

	// loading and unloading rooms at the edge of vision
	void remove_viewer_and_attempt_unload(const Coordinate & coordinate, const std::string & viewer_ID);

	// unloading of all rooms in view distance (for logging out or dying)
	void attempt_unload_radius(const Coordinate & coordinate, const std::string & player_ID);

	// test if a room can be removed from memory
	bool is_unloadable(const Coordinate & coordinate) const;

	// move a room from world to disk
	void unload_room(const Coordinate & coordinate);

	// room information
	bool room_has_surface(const Coordinate & coordinate, const std::string & direction_ID) const;



private:

	void create_world_container();
	void load_or_generate_terrain_and_mineral_maps();

	// three functions for loading and verifying the world map and the two mineral maps

	bool load_existing_world_terrain();
	bool load_existing_iron_deposit_map();
	bool load_existing_limestone_deposit_map();

	// a room at x,y,z does not exist on the disk; create it
	void generate_room_at(const Coordinate & coordinate);

	// load the room x,y to an xml_document
	void load_room_to_XML(const Coordinate & coordinate, pugi::xml_document & vertical_rooms);

	// build a room given an XML node, add to world at x,y,z
	void add_room_to_world(pugi::xml_node & room_document, const Coordinate & coordinate);

	// move specific room into memory
	void load_room_to_world(const Coordinate & coordinate);

	// move a passed room to disk
	void unload_room(const Coordinate & coordinate, const std::unique_ptr<Room>::pointer room);

	// save the contents of a room to an XML file in memory
	void save_room_to_XML(const std::unique_ptr<Room>::pointer room, pugi::xml_document & room_document) const;

	// create a new empty room given its coordinates and the world terrain
	std::unique_ptr<Room> create_room(const Coordinate & coordinate) const;

	// remove a room from memory
	void erase_room_from_memory(const Coordinate & coordinate);
	
	void unload_rooms_to_disk()
	{
		// This only unloads one room at a time in order to make sure that the main thread
		// can access the queue as quickly as possible. The main thread will always try to
		// "load" a room from the unload queue (if it exists) before resorting to loading
		// from the disk.
		
		for (;;)
		{
			std::unique_ptr<Room> room;
			
			{
				std::unique_lock<std::mutex> lock(unload_queue_mutex);

				// wait if there are no rooms in the queue unload
				while (unload_queue.is_empty() && Server::is_running())
					unload_queue_cv.wait(lock);
								
				// this only clears the queue when the server is running
				// Upon server shutdown, the main thread adds all remaining rooms in the World to the queue,
				// and then unloads the queue.
				if (!Server::is_running()) return; // this thread now releases the unload queue mutex and dies
				
				// debug code
				std::stringstream ss;
				ss << "Unloading a room from the unload queue. Current size: " << unload_queue.size() << std::endl;
				std::cout << ss.str();
				
				// get the room
				room = std::move(unload_queue.front());
				
				// clear the nullptr at the beginning of the queue
				unload_queue.pop();
			
			} // release the lock on the queue
			
			// add a one-second wait here to demonstrate that the performance of this thread interacting with the desk doesn't effect upon the main thread
			
			// unload one room
			unload_room(unload_queue.front()->coordinates(), unload_queue.front());
			
		} // repeat until server shutdown
	}
	
	// this snippet will have to be added to the loading logic
	{ // check if the room is waiting in the unload queue
		
		std::unique_lock<std::mutex> lock(unload_queue_mutex);
		// for each room in the queue
		for (auto it = unload_queue.cbegin(); it != unload_queue.cend(); ++it)
		{
			// if the room we want to load is about to be unloaded
			if (**it == coordinates) // dereference twice, because we have an iterator to a unique pointer
			{
				// move the room back to the world
				world.at(coordinates.hash()) = std::move(*it);
				// erase the entry in the unload queue
				unload_queue.erase(it);
				// our work here is done; stop searching for the room
				break;
			}
		}
	}
	
	// this snippet will be added in the main thread during server shutdown
	{
		std::unique_lock<std::mutex> lock(unload_queue_mutex);
		
		// unload every room in the unload queue
		while (unload_queue.size() > 0)
		{
				// pull the room out of the list
				std::unique_ptr<Room> room = std::move(unload_queue.front());
				// clear the nullptr at the beginning of the queue
				unload_queue.pop();
				// save the room to disk
				unload_room(room->coordinates(), room);
		}
	}
	
};

#endif
