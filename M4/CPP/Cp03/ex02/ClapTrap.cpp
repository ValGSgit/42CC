#include "ClapTrap.hpp"

ClapTrap::ClapTrap() : _name("Default"), _hitPoints(10), _energyPoints(10), _attackDamage(0)
{
	std::cout << "ClapTrap default constructor called" << std::endl;
}

ClapTrap::ClapTrap(const std::string& name) : _name(name), _hitPoints(10), _energyPoints(10), _attackDamage(0)
{
	std::cout << "ClapTrap named constructor called for " << _name << std::endl;
}

ClapTrap::ClapTrap(const ClapTrap& other)
{
	std::cout << "ClapTrap copy constructor called" << std::endl;
	*this = other;
}

ClapTrap& ClapTrap::operator=(const ClapTrap& other)
{
	std::cout << "ClapTrap assignment operator called" << std::endl;
	if (this != &other)
	{
		_name = other._name;
		_hitPoints = other._hitPoints;
		_energyPoints = other._energyPoints;
		_attackDamage = other._attackDamage;
	}
	return *this;
}

ClapTrap::~ClapTrap()
{
	std::cout << "ClapTrap destructor called for " << _name << std::endl;
}

void ClapTrap::attack(const std::string& target)
{
	if (_energyPoints <= 0)
    {
        std::cout << "ScavTrap: " << _name << " has no energy points left to attack!" << std::endl;
        return;
    }
    else if (_hitPoints <= 0)
    {
        std::cout << "ScavTrap: " << _name << " is already dead so he cant attack lel" << std::endl;
        return ;
    }
	
	_energyPoints--;
	std::cout << "ClapTrap: " << _name << " attacks " << target 
		<< ", causing " << _attackDamage << " points of damage!" << std::endl;
}

void ClapTrap::takeDamage(unsigned int amount)
{
	if (_hitPoints <= 0)
	{
		std::cout << "ClapTrap: " << _name << " is already defeated and can't take more damage!" << std::endl;
		return;
	}
	
	_hitPoints -= amount;
	if (_hitPoints < 0)
		_hitPoints = 0;
		
	std::cout << "ClapTrap: " << _name << " takes " << amount 
		<< " damage points and now has " << _hitPoints << " hit points left!" << std::endl;
}

void ClapTrap::beRepaired(unsigned int amount)
{
	if (_energyPoints <= 0 || _hitPoints <= 0)
	{
		std::cout << "ClapTrap: " << _name << " has no energy/hit points left to repair itself!" << std::endl;
		return;
	}
	
	_energyPoints--;
	_hitPoints += amount;
	
	std::cout << "ClapTrap: " << _name << " repairs itself for " << amount 
		<< " hit points and now has " << _hitPoints << " hit points!" << std::endl;
}
