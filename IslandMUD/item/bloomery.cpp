
#include "bloomery.h"

Bloomery::Bloomery() : Fire_Container(C::BLOOMERY_ID)
{
	this->bloom = Bloom();
}

void Bloomery::add_to_bloomery(const std::shared_ptr<Forgeable> & forgeable_item)
{
	// Accept a shared_ptr to a Mineral types.
	// The mineral item will be wrapped in a Meltable item, which records
	melt_contents.push_back(forgeable_item);
}

std::string Bloomery::observe_bloomery()
{
	update_bloomery();

	// ...

	return "[Bloomery observation]";
}

Bloom Bloomery::remove_bloom()
{
	update_bloomery();

	// ...
	
	const Bloom copy = this->bloom; // save
	this->bloom = Bloom(); // reset original
	return copy; // return copy
}

void Bloomery::update_bloomery() // update the bloomery's temperature and fuel state, and the state of all items inside the bloomery
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
		if (melt_contents[i]->temperature > 0 /* <-- placeholder for the forgeable item's melting point */)
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
