/* Jim Viebke
April 1, 2014 */

#ifndef WORLD_H
#define WORLD_H

#include <vector>
#include <memory>
#include <map>
#include <thread>

#include "XML/pugixml.hpp"

#include "server/server.h"
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

	// For the world, we store x*y pointers. Most of these pointers will be null most of the time,
	// but this gives us the luxury of O(1) access time for any room.
	std::vector<std::unique_ptr<Room>> world;

	// The "main" thread (the thread making calls to World) doesn't have to pay the I/O cost of unloading rooms.
	// The world object internally contains a background thread that does the unloading work.
	// One tricky caveat is that if a room is still in the queue, and that room is requested to be loaded,
	// the room has to be "loaded" from the queue, not from disk.
	std::thread background_room_unloading_thread; // the thread
	std::deque<std::unique_ptr<Room>> unload_queue; // the queue of rooms to unload
	std::recursive_mutex unload_queue_mutex;
	std::condition_variable_any unload_queue_cv; // a cv so the main thread can wake the background unloading thread

	// A second caveat is that the room might not be in the queue,
	std::unique_ptr<Coordinate> writing_to_disk = nullptr; // which room is being written
	std::mutex writing_to_disk_mutex;
	std::condition_variable writing_to_disk_cv; // in case the main thread has to sleep while waiting for a room to finish saving

public:

	World();
	~World();

	// access a room using its coordinates
	std::unique_ptr<Room>::pointer room_at(const Coordinate & coordinate);
	const std::unique_ptr<Room>::pointer room_at(const Coordinate & coordinate) const;
	std::unique_ptr<Room> & reference_room_at(const Coordinate & coordinate);

	// debugging
	unsigned count_loaded_rooms() const;

	// load rooms around a player spawning in
	void load_view_radius_around(const Coordinate & coordinate, const std::string & character_ID);

	// loading and unloading rooms at the edge of vision
	void remove_viewer_and_attempt_unload(const Coordinate & coordinate, const std::string & viewer_ID);
	void remove_viewer_and_attempt_unload(const std::vector<Coordinate> & coordinates, const std::string & viewer_ID); // wrapper for multiple rooms

	// unloading of all rooms in view distance (for logging out or dying)
	void attempt_unload_radius(const Coordinate & coordinate, const std::string & player_ID);

	// test if a room can be removed from memory
	bool is_unloadable(const Coordinate & coordinate) const;

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

	// save the contents of a room to an XML file in memory
	void save_room_to_XML(const std::unique_ptr<Room> & room, pugi::xml_document & room_document) const;

	// create a new empty room given its coordinates and the world terrain
	std::unique_ptr<Room> create_room(const Coordinate & coordinate) const;

	// remove a room from memory
	void erase_room_from_memory(const Coordinate & coordinate);



	// runs in a thread to unload rooms in the background, off of the main thread
	void unload_rooms_to_disk();

	// move a passed room to disk
	void unload_room(const std::unique_ptr<Room> & room);

};

#endif
