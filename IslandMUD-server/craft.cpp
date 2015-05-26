/* Jim Viebke
Feb 16 2014 */

#include "craft.h"

shared_ptr<Item> Craft::make(const string & item_ID)
{
	/* This provides an alternative method of the below:
	http://www.codeguru.com/cpp/cpp/cpp_mfc/article.php/c4067/Switch-on-Strings-in-C.htm */

	// alphabetical based on ID, not object name
	if (item_ID == C::ANVIL_ID) { return make_shared<Anvil>(); }
	else if (item_ID == C::AXE_ID) { return make_shared<Axe>(); }
	else if (item_ID == C::BOW_ID) { return make_shared<Bow>(); }
	else if (item_ID == C::BRANCH_ID) { return make_shared<Branch>(); }
	else if (item_ID == C::DEBRIS_ID) { return make_shared<Debris>(); }
	else if (item_ID == C::FORGE_ID) { return make_shared<Forge>(); }
	else if (item_ID == C::HAMMER_ID) { return make_shared<Hammer>(); }
	else if (item_ID == C::STAFF_ID) { return make_shared<Staff>(); }
	else if (item_ID == C::STICK_ID) { return make_shared<Stick>(); }
	else if (item_ID == C::STONE_ID) { return make_shared<Rock>(); }
	else if (item_ID == C::SWORD_ID) { return make_shared<Sword>(); }
	else if (item_ID == C::TORCH_ID) { return make_shared<Torch>(); }
	else if (item_ID == C::TREE_ID) { return make_shared<Tree>(); }
	else if (item_ID == C::VINE_ID) { return make_shared<Vine>(); }
	else if (item_ID == C::WOOD_ID) { return make_shared<Wood>(); }
	else { return make_shared<Rock>(); } // something must be returned, so if the item id is invalid a rock manifests itself
}
