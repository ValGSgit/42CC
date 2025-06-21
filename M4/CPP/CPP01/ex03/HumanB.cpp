#include "HumanB.hpp"

HumanB::HumanB(const std::string& name) : name(name), weapon(NULL)
{
	std::cout << name << " has joined the battle without a weapon" << std::endl;
}

HumanB::~HumanB()
{
	std::cout << this->name << " is leaving the battle" << std::endl;
}

void HumanB::setWeapon(Weapon& weapon)
{
	this->weapon = &weapon;
	std::cout << name << " has found a " << weapon.getType() << std::endl;
}

void HumanB::attack()
{
	if (this->weapon)
		std::cout << this->name << " attacks with their " << this->weapon->getType() << std::endl;
	else
		std::cout << this->name << " has no weapon to attack with!" << std::endl;
}