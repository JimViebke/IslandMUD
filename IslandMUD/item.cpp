
/* Jim Viebke
Dec 22 2015

Source filefor Item parent class. */

#include "item.h"

Item::Item(const std::string & item_name, const bool & is_takable, const int & set_health)
	: name(item_name), takable(is_takable)
{
	this->set_health(set_health);
}

Item::~Item() {}

bool Item::is_takable() const
{
	return takable;
}
std::string Item::get_name() const
{
	return name;
}
int Item::get_health() const
{
	return health;
}
void Item::set_health(const int & set_health)
{
	// if the passed value is out of bounds, set health to max_health, otherwise set to the passed value
	health = ((set_health > C::DEFAULT_ITEM_MAX_HEALTH || set_health < C::DEFAULT_ITEM_MIN_HEALTH)
		? C::DEFAULT_ITEM_MAX_HEALTH : set_health);
}
void Item::update_health(const int & update_health)
{
	// subtract the passed amount from the current health
	health -= update_health;

	// use set_health() to validate
	set_health(health);
}
