/* Jim Viebke
Feb 16 2014 */

#include <memory>

#include "craft.h"

shared_ptr<Item> Craft::make(const string & item_id)
{
	/* This provides an alternative method of the below:
	http://www.codeguru.com/cpp/cpp/cpp_mfc/article.php/c4067/Switch-on-Strings-in-C.htm */

	// alphabetical based on ID, not object name
	if (item_id == C::AXE_ID) { return make_shared<Axe>(); }
	else if (item_id == C::BOW_ID) { return make_shared<Bow>(); }
	else if (item_id == C::BRANCH_ID) { return make_shared<Branch>(); }
	else if (item_id == C::STAFF_ID) { return make_shared<Staff>(); }
	else if (item_id == C::STICK_ID) { return make_shared<Stick>(); }
	else if (item_id == C::STONE_ID) { return make_shared<Rock>(); }
	else if (item_id == C::TORCH_ID) { return make_shared<Torch>(); }
	else if (item_id == C::TREE_ID) { return make_shared<Tree>(); }
	else if (item_id == C::VINE_ID) { return make_shared<Vine>(); }
	else if (item_id == C::WOOD_ID) { return make_shared<Wood>(); }
	else { return make_shared<Rock>(); } // if the item id is invalid, a rock manifests itself
}