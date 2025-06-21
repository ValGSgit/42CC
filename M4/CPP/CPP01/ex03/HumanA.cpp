#include "HumanA.hpp"

HumanA::HumanA(const std::string& name, Weapon& weapon) 
	: name(name), weapon(weapon)
{
	std::cout << name << " has joined the battle wielding " << weapon.getType() << std::endl;
}

HumanA::~HumanA()
{
	std::cout << this->name << " is leaving the battle" << std::endl;
}

void HumanA::attack()
{
	std::cout << this->name << " attacks with their " << this->weapon.getType() << std::endl;
}