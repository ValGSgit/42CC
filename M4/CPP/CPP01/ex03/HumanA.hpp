#ifndef HUMANA_H
# define HUMANA_H

# include <iostream>
# include <string>
# include "Weapon.hpp"

// ### **References vs Pointers - The Core Decision**

// **HumanA should use a reference** because:
//    - Always has a weapon (never null)
//    - The weapon is passed in constructor and never changes
//    - References can't be reassigned or null



class HumanA
{
	private:
		std::string name;
		Weapon& weapon; // Reference because HumanA should always have a weapon
	
	public:
		HumanA(const std::string& name, Weapon& weapon);
		~HumanA();
		void attack();
};

#endif