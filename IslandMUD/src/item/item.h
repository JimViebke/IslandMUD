/* Jim Viebke
Feb 14, 2015 */

#ifndef ITEM_H
#define ITEM_H

#include <vector>
#include <string>
#include <map>
#include <memory>

#include "../constants.h"
#include "../utilities.h"

class Item
{
private:
	bool takable;
	std::string name;
	// int weight = 0;

protected:

	int health = 100;

	Item(const std::string & item_name, const bool & is_takable, const int & set_health = C::DEFAULT_ITEM_MAX_HEALTH);

	virtual ~Item();

public:
	bool is_takable() const;
	std::string get_name() const;
	int get_health() const;
	void set_health(const int & set_health);
	void update_health(const int & update_health);
};

class Stackable : public Item
{
public:
	unsigned amount = 1;

protected:
	Stackable(const std::string & cust_name) : Item(cust_name, true) {} // all stackable items are takable
};

class Fire_Container : public Item
{
protected:
	unsigned fuel_units_remaining = 0;
	unsigned temperature = C::AIR_TEMPERATURE;
	bool lit = false;

	time_t timestamp = U::current_time_in_ms();

	// add something to indicate fuel type, which will be used to determine the temperature of the fire 
	// Fuel_Type

	Fire_Container(const std::string & item_name) : Item(item_name, false) {}

	void update()
	{
		this->timestamp = U::current_time_in_ms();

		// update the temperature and fuel content
		// ...
	}
};

class Forgeable : public Item
{
private:
	friend class Bloomery; // bloomeries can access the private members (specs/stats) of forgeable items

	// contents
	unsigned iron_units = 0; // units are arbitrary, but consistent
	unsigned carbon_units = 0;
	unsigned impurity_units = 0;

	// attributes
	unsigned hardness;
	unsigned toughness;

	// the... well, there it is
	unsigned temperature = C::AIR_TEMPERATURE; // the default

	// Forgeable stores a state and a time.
	// Whenever the forgeable [thing] is observed, we update the [thing]'s specs and the timestamp.
	long long last_update_timestamp = U::current_time_in_ms();

protected:
	// this cannot be directly instantiated - specific items can inherit forgeability
	Forgeable(const std::string & item_ID) : Item(item_ID, true) {}

public:
	unsigned get_size() const
	{
		return iron_units + carbon_units + impurity_units;
	}

	void update_specs(const unsigned & ambient_temperature)
	{
		// Use the ambient temperature to update the temperature of the forgeable item.
		// The ambient temperature is the temperature of the fire, the forge, or just the air temperature when not in a fire.

		// Time and size are the only factors that affect the speed at which the temperature of the forgeable ite
		// approaches the ambient temperature.

		// The temperature of the forgeable item approaches the ambient temperature at 10 degrees per second,
		// or 1 degree per 100 milliseconds, or 600 degrees per minute.
		// Using this scale, items reach temperature in about one minute, and cool to room temperature in about one minute as well.

		// first update the timestamp
		last_update_timestamp = U::current_time_in_ms();

		// calculate the time that has elapsed between now and the last update time
		const long long milliseconds_since_last_update = U::current_time_in_ms() - last_update_timestamp;

		// Calculate how much the item's temperature has changed. This moves in the direction of the ambient temperature.
		const unsigned change_in_degrees = unsigned(milliseconds_since_last_update / 100); // 1 degree per 100 milliseconds, or 10 degrees per second.

		// if the change is greater than the difference, the temperature doesn't pass the ambient temperature, it is the ambient temperature
		if (change_in_degrees > U::difference(this->temperature, ambient_temperature))
		{
			this->temperature = ambient_temperature;
		}
		// the temperature has not yet reached the ambient temperature
		else if (this->temperature > ambient_temperature) // test if the item is cooling
		{
			this->temperature -= change_in_degrees; // decrease the temperature towards the ambient
		}
		else // the item is less than the ambient temperature, 
		{
			this->temperature += change_in_degrees; // increase the temperature towards the ambient
		}

		/*
		
		If the item passed a temperature range since the last update, we need to calculate how much time was spent in that range
		in order to determine how to update the item's stats.
		
		*/
	}

	void set_temperature(const unsigned & set_temperature)
	{
		temperature = set_temperature;
	}
};

class Bloom : public Forgeable // this is the glowy lump that comes out of a bloomery
{
public:
	Bloom() : Forgeable(C::BLOOM_ID) {}
};

class Mineral : public Stackable
{
protected:
	Mineral(const std::string & mineral_ID) : Stackable(mineral_ID) {}
};

class Forge : public Fire_Container
{
private:
	std::unique_ptr<Forgeable> workpiece;
	time_t workpiece_insert_time; // the time that the current workpiece placed in the forge

public:
	Forge() : Fire_Container(C::FORGE_ID) {}
};

class Mineral_Deposit : public Item
{
protected:
	Mineral_Deposit(const std::string & mineral_ID) : Item(mineral_ID, false) {}
};

class Iron_Deposit : public Mineral_Deposit
{
public:
	Iron_Deposit() : Mineral_Deposit(C::IRON_DEPOSIT_ID) {}
};

class Limestone_Deposit : public Mineral_Deposit
{
public:
	Limestone_Deposit() : Mineral_Deposit(C::LIMESTONE_DEPOSIT_ID) {}
};

class Iron : public Mineral
{
public:
	Iron() : Mineral(C::IRON_ID) {}
};

class Limestone : public Mineral
{
public:
	Limestone() : Mineral(C::LIMESTONE_ID) {}
};

#endif
