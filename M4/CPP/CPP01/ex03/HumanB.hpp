#ifndef HUMANB_H
# define HUMANB_H

# include <iostream>
# include <string>
# include "Weapon.hpp"

// **HumanB should use a pointer** because:
//    - May not have a weapon (can be null)
//    - Can change weapons with `setWeapon()`
//    - Pointers can be null and reassigned

class HumanB
{
	private:
		std::string name;
		Weapon* weapon; // Pointer because HumanB may not have a weapon
	
	public:
		HumanB(const std::string& name);
		~HumanB();
		void setWeapon(Weapon& weapon);
		void attack();
};

#endif