/* Jim Viebke
May 19 2015 */

#ifndef DOOR_H
#define DOOR_H

#include "constants.h"

class Door
{
private:
	int health = C::MAX_DOOR_HEALTH; // set by default
	string faction_ID; // set by constructor
	string material_ID; // set by constructor

public:
	Door(const string & faction, const string & material) : faction_ID(faction), material_ID(material) {}
	Door(const string & faction, const string & material, const int & set_health) : faction_ID(faction), material_ID(material), health(set_health)
	{
		// cheat and use this for validation
		update_health_by(0);
	}

	void set_health_to(const int & set_health);
	void update_health_by(const int & health_update);

	// getters for serialization
	int get_health() const;
	string get_faction_ID() const;
	string get_material_ID() const;

	bool is_rubble() const;

};

#endif
