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

public:
	std::string name;
	// int weight = 0;

protected:

	int health = 100;

	Item(const std::string & item_name, const bool & is_takable, const int & set_health = C::DEFAULT_ITEM_MAX_HEALTH);

	virtual ~Item();

public:
	bool is_takable() const;
	int get_health() const;
	void set_health(const int & set_health);
	void update_health(const int & update_health);
};

class Material : public Item
{
public:
	unsigned amount = 1;

protected:
	Material(const std::string & cust_name) : Item(cust_name, true) {} // all materials are takable
};

class Fire_Container : public Item
{
protected:
	unsigned fuel_units_remaining = 0;
	unsigned temperature = C::AMBIENT_AIR_TEMPERATURE;
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
protected:
	Forgeable(const std::string & item_ID) : Item(item_ID, true) {}

public:
	unsigned iron_units = 0; // units are arbitrary, but consistent
	unsigned carbon_units = 0;
	unsigned impurity_units = 0;

	unsigned hardness;
	unsigned toughness;
	unsigned grain_size;

	unsigned temperature;

	long long last_update_timestamp = U::current_time_in_ms();

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

		// don't do anything if the item is already at the ambient temperature
		if (this->temperature == ambient_temperature) return;

		// calculate the time that has elapsed between now and the last update time
		const long long milliseconds_since_last_update = U::current_time_in_ms() - last_update_timestamp;

		// Calculate how much the item's temperature has changed. This always moves in the direction of the ambient temperature.
		const unsigned change_in_degrees = unsigned(milliseconds_since_last_update / 100); // 1 degree per 100 milliseconds

		// if the temperature has reached the ambient temperature
		if (change_in_degrees > U::difference(this->temperature, ambient_temperature))
		{
			// update the item's temperature
			this->temperature = ambient_temperature;
		}
		// the temperature has not yet reached the ambient temperature
		else if (this->temperature > ambient_temperature) // test if the item is cooling
		{
			this->temperature -= change_in_degrees; // decrease the temperature
		}
		else // the item is less than the ambient temperature, increase the temperature
		{
			this->temperature += change_in_degrees;
		}
	}
};

class Bloom : public Forgeable // this is the glowy lump that comes out of a bloomery
{
public:
	Bloom() : Forgeable(C::BLOOM_ID) {}
};

class Mineral : public Material
{
protected:
	Mineral(const std::string & mineral_ID) : Material(mineral_ID) {}
};

class Bloomery : public Fire_Container // raw ores and unwanted metals go in, a glowy blooms comes out
{
public:
	Bloomery() : Fire_Container(C::BLOOMERY_ID) { this->bloom = Bloom(); }

	void add_to_bloomery(const std::shared_ptr<Forgeable> & forgeable_item)
	{
		// Accept a shared_ptr to a Mineral types.
		// The mineral item will be wrapped in a Meltable item, which records
		melt_contents.push_back(forgeable_item);
	}

	std::string observe_bloomery()
	{
		update_bloomery();

		// ...
	}

	Bloom remove_bloom()
	{
		update_bloomery();

		// ...
	}

private:
	Bloom bloom;

	std::vector<std::shared_ptr<Forgeable>> melt_contents; // an item will melt and join the bloom when it reaches temperature and melts

	void update_bloomery() // update the bloomery's temperature and fuel state, and the state of all items inside the bloomery
	{
		// update the temperature and fuel content of the bloomery
		this->update();

		// the bloom may have a size of 0, update the temperature anyway
		bloom.temperature = this->temperature;

		// update the state of each item in the bloomery, using the temperature of the bloomery
		for (unsigned i = 0; i < melt_contents.size();) // for each item in the bloomery
		{
			// use the temperature of the bloom to update the item
			melt_contents[i]->update_specs(this->temperature);

			// test if the item has melted
			if (melt_contents[i]->temperature > 0 /*the forgeable item's melting point*/)
			{
				// the item melts, adding its contents to the bloom
				bloom.carbon_units += melt_contents[i]->carbon_units;
				bloom.iron_units += melt_contents[i]->iron_units;
				bloom.impurity_units += melt_contents[i]->impurity_units;

				melt_contents.erase(melt_contents.cbegin() + i);
			}
			else // the item did not melt, move to next item in the bloom
			{
				++i;
			}
		}
	}
};

class Equipment : public Item
{
	// string equipment_material; // stone, iron, wood, etc
	// string handle_material; // wood, etc
	// other members represent quality, health

protected:
	Equipment(const std::string & set_name) : Item(set_name, true) {} // all equipment is takable
};

class Forge : public Fire_Container
{
private:
	std::unique_ptr<Equipment> workpiece;
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
