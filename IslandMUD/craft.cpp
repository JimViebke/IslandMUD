/* Jim Viebke
Feb 16 2014 */

#include "craft.h"

namespace
{
	template <typename Item_Type> std::shared_ptr<Item> create_instance() { return std::make_shared<Item_Type>(); }
}

std::shared_ptr<Item> Craft::make(const std::string & item_ID)
{
	// alphabetical based on ID, not object name
	typedef std::shared_ptr<Item>(*create_item_pointer)();
	const static std::map<std::string, create_item_pointer> items = {
		{ C::ANVIL_ID, &create_instance<Anvil> },
		{ C::ARROW_ID, &create_instance<Arrow> },
		{ C::ARROWHEAD_ID, &create_instance<Arrowhead> },
		{ C::AXE_ID, &create_instance<Axe> },
		{ C::BLOOMERY_ID, &create_instance<Bloomery> },
		{ C::BOARD_ID, &create_instance<Board> },
		{ C::BOW_ID, &create_instance<Bow> },
		{ C::BRANCH_ID, &create_instance<Branch> },
		{ C::DEBRIS_ID, &create_instance<Debris> },
		{ C::FORGE_ID, &create_instance<Forge> },
		{ C::HAMMER_ID, &create_instance<Hammer> },
		{ C::IRON_ID, &create_instance<Iron> },
		{ C::IRON_DEPOSIT_ID, &create_instance<Iron_Deposit> },
		{ C::LIMESTONE_ID, &create_instance<Limestone> },
		{ C::LIMESTONE_DEPOSIT_ID, &create_instance<Limestone_Deposit> },
		{ C::LOG_ID, &create_instance<Log> },
		{ C::STAFF_ID, &create_instance<Staff> },
		{ C::STICK_ID, &create_instance<Stick> },
		{ C::STONE_ID, &create_instance<Stone> },
		{ C::SWORD_ID, &create_instance<Sword> },
		{ C::TORCH_ID, &create_instance<Torch> },
		{ C::TREE_ID, &create_instance<Tree> },
		{ C::VINE_ID, &create_instance<Vine> },
		{ C::WOOD_ID, &create_instance<Wood> }
	};

	// extract the corresponding entry in the map
	const std::map<std::string, create_item_pointer>::const_iterator pair = items.find(item_ID);

	// craft a new instance of the specified item by calling the function pointer,
	// creating a new stone if the key (item ID) was not in the map
	return (pair != items.cend()) ? pair->second() : std::make_shared<Stone>();
}
