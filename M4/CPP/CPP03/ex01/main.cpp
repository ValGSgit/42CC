#include "ClapTrap.hpp"
#include "ScavTrap.hpp"
#include <iostream>

int main()
{
    std::cout << "=== Creating ClapTraps ===" << std::endl;
	ClapTrap juggerNut("Im the Juggernaut biatch");
    ClapTrap defaultTrap;
    ClapTrap bob("Bob");
    ClapTrap alice("Alice");
    ClapTrap bobClone(bob);
    
    std::cout << "\n=== Testing ClapTrap basic functionality ===" << std::endl;
	juggerNut.beRepaired(59);
    std::cout << "Yeah, theres no max hitpoints yet ;)" << std::endl;
    bob.attack("enemy");
    alice.attack("target");
    bob.takeDamage(5);
    bob.beRepaired(3);
    
    std::cout << "\n=== Creating ScavTraps ===" << std::endl;
    ScavTrap guardian("Guardian");
    ScavTrap defaultScav("That random dude");
    ScavTrap guardianClone(guardian);
    
    std::cout << "\n=== Testing ScavTrap functionality ===" << std::endl;
    guardian.attack("intruder");
    guardian.takeDamage(30);
    guardian.beRepaired(15);
    guardian.guardGate();
    
    std::cout << "\n=== Testing ClapTrap energy reduction ===" << std::endl;

	for (int i = 0; i < 9; i++) {
        alice.attack("Im the Juggernaut biatch");
		juggerNut.takeDamage(10);
    }
    // This should fail due to no energy points left
    alice.attack("dummy target");
    alice.beRepaired(5);
    
    std::cout << "\n=== Testing ScavTrap energy reduction ===" << std::endl;
    // ScavTrap has 50 energy points, so we'll use a lot of them
    for (int i = 0; i < 15; i++) {
        guardian.attack("target dummy");
        if (i % 5 == 0) {
            guardian.beRepaired(10);
        }
    }

    guardian.attack("last target");
    guardian.beRepaired(5);
    
    std::cout << "\n=== Testing hit points reduction ===" << std::endl;
    bob.takeDamage(20);

    bob.attack("enemy");
    bob.beRepaired(10);

    defaultScav.takeDamage(150);
    defaultScav.attack("enemy");
    defaultScav.beRepaired(50);
    
    std::cout << "\n=== Testing destructor calls (observe construction/destruction order) ===" << std::endl;
    return 0;
}
