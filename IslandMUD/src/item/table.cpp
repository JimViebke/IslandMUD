/* Jim Viebke
Jan 6 2016 */

#include "table.h"

Table::Table()
	: Item(C::TABLE_ID, false), Container() {}

Table::Table(const int & set_health, const std::multimap<std::string, std::shared_ptr<Item>> & set_contents)
	: Item(C::TABLE_ID, false, set_health), Container()
{
	this->contents = set_contents;
}
