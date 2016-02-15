
/* Jim Viebke
Dec 24 2015

Container class, for things that hold things.

Both Character and Chest derive from this. */

#ifndef CONTAINER_H
#define CONTAINER_H

#include "item.h"

class Container
{
public:
	std::string contents_to_string() const;

	const std::multimap<std::string, std::shared_ptr<Item>> & get_contents() const;

	bool contains(const std::string & item_id) const;
	bool contains(const std::string & item_id, const unsigned & count) const;
	unsigned count(const std::string & item_id) const;
	unsigned size() const;

	bool insert(const std::shared_ptr<Item> & item);
	std::shared_ptr<Item> erase(const std::string & item_id); // returns nullptr for items that don't exist
	void erase(const std::string & item_id, const unsigned & count);

protected:
	std::multimap<std::string, std::shared_ptr<Item>> contents;

	Container(); // must be inherited to be instantiated
	
	void set_contents(const std::multimap<std::string, std::shared_ptr<Item>> & set_contents);
};

#endif
