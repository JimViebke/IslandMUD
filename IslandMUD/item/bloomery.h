
/* Jim Viebke
Dec 22 2015

Header file for Bloomery class. A bloomery is not a bloom. */

#include "item.h"

#ifndef BLOOMERY_H
#define BLOOMERY_H

class Bloomery : public Fire_Container // raw ores and unwanted metals go in, a glowy blooms comes out
{
public:
	Bloomery();

	void add_to_bloomery(const std::shared_ptr<Forgeable> & forgeable_item);

	std::string observe_bloomery();

	Bloom remove_bloom();

private:
	Bloom bloom;

	std::vector<std::shared_ptr<Forgeable>> melt_contents; // an item will melt and join the bloom when it reaches temperature and melts

	void update_bloomery(); // update the bloomery's temperature and fuel state, and the state of all items inside the bloomery
};

#endif
