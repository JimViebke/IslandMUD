/* Jim Viebke
Jul 31 2015 */

#include "craft.h"

// contents
void Chest::add(const shared_ptr<Item> & item)
{
	// if the item is a material and is therefore stackable
	if (R::is<Material>(item))
	{
		// check if the player already has an instance of the item
		if (this->has(item->name))
		{
			// if so, increment the count
			this->material_contents[item->name]->amount++;
		}
		else
		{
			// if not, give the player a new instance of the item
			this->material_contents.insert(pair<string, shared_ptr<Material>>(item->name, R::convert_to<Material>(Craft::make(item->name))));
		}
	}
	else // the item is not a material and is therefore an Equipment type
	{
		// insert the new item
		this->equipment_contents.insert(pair<string, shared_ptr<Equipment>>(item->name, R::convert_to<Equipment>(item)));
	}
}
void Chest::remove(const string & item_id, const unsigned & count)
{
	// WARNING - for materials this assumes the chest has [count] instances

	// remove or reduce the item in the chest
	if (equipment_contents.find(item_id) != equipment_contents.cend())
	{
		for (unsigned i = 0; i < count; ++i)
		{
			equipment_contents.erase(equipment_contents.find(item_id));
		}
	}
	else if (material_contents.find(item_id) != material_contents.cend()) // the item is present in the material inventory
	{
		material_contents.find(item_id)->second->amount -= count; // decrement the material count in the player's inventory
		if (material_contents.find(item_id)->second->amount < 1)
		{
			material_contents.erase(material_contents.find(item_id));
		}
	}
	else
	{
		// the chest does not have the item
	}
}
bool Chest::has(const string & item_id, const unsigned & count) const
{
	if (count == 1) // only one instance is required
	{
		return
			equipment_contents.find(item_id) != equipment_contents.cend() ||
			material_contents.find(item_id) != material_contents.cend();
	}
	else // more than one item is required
	{
		return (equipment_contents.count(item_id) >= count) // equipment_contents contains enough of the required item
			||
			(material_contents.find(item_id) != material_contents.cend() && // the material exists in the chest
			material_contents.find(item_id)->second->amount >= count); // AND in sufficient quantity
	}
}
shared_ptr<Item> Chest::take(const string & item_id)
{
	if (equipment_contents.find(item_id) != equipment_contents.cend())
	{
		// extract the item
		shared_ptr<Equipment> item = equipment_contents.find(item_id)->second;
		// remove it from the chest
		remove(item_id);
		// pass the item back
		return item;
	}
	else if (material_contents.find(item_id) != material_contents.cend())
	{
		// remove or reduce the item in the chest
		remove(item_id);
		// pass a new instance of the item back
		return Craft::make(item_id);
	}
	else
	{
		// the item does not exist in either type of contents, indicating
		// a validation error in the calling function
		return Craft::make(C::STONE_ID);
	}
}
string Chest::contents() const
{
	if (equipment_contents.size() == 0 && material_contents.size() == 0)
	{
		return "The chest is empty.";
	}

	stringstream output;
	output << "The chest contains";

	for (multimap<string, shared_ptr<Equipment>>::const_iterator it = equipment_contents.begin();
		it != equipment_contents.end(); ++it)
	{
		output << " a(n) " << it->second->name;
	}
	for (map<string, shared_ptr<Material>>::const_iterator it = material_contents.begin();
		it != material_contents.end(); ++it)
	{
		output << " " << it->second->name << ":" << it->second->amount;
	}

	return (output.str() + ".");
}
multimap<string, shared_ptr<Equipment>> Chest::get_equipment_contents() const
{
	return equipment_contents;
}
map<string, shared_ptr<Material>> Chest::get_material_contents() const
{
	return material_contents;
}

// health
void Chest::damage(const int & amount)
{
	// PASS A NEGATIVE VALUE FOR DAMAGE

	health += amount;

	// if the health is larger than allowed
	if (health > C::MAX_CHEST_HEALTH)
	{
		// bring it back down to max
		health = C::MAX_CHEST_HEALTH;
	}

	// if health is negative
	if (health < 0)
	{
		// move health back up to 0
		health = 0;
	}
}
void Chest::set_health(const int & amount)
{
	health = amount;

	damage(0); // reuse the validation within
}
int Chest::get_health() const
{
	return health;
}

// faction ID retrieval
string Chest::get_faction_id() const
{
	return this->faction_id;
}
