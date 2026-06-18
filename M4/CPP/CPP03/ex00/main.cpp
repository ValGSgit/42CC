#include "ClapTrap.hpp"
#include <iostream>

int main()
{
    std::cout << "=== Creating ClapTraps ===" << std::endl;
	ClapTrap juggerNut("Im the Juggernaut biatch");
    ClapTrap defaultTrap;
    ClapTrap bob("Bob");
    ClapTrap alice("Alice");
    ClapTrap bobClone(bob); // Copy constructor test
    
    std::cout << "\n=== Testing basic functionality ===" << std::endl;
	juggerNut.beRepaired(59);
    std::cout << "Yeah, theres no max hitpoints yet ;)" << std::endl;
    bob.attack("enemy");
    alice.attack("target");
    bob.takeDamage(5);
    bob.beRepaired(3);
    
    std::cout << "\n=== Testing energy depletion ===" << std::endl;
    // Test using up all energy points
    for (int i = 0; i < 5; i++) {
        alice.attack("dummy target");
    }
	for (int i = 0; i < 5; i++) {
        alice.attack("Im the Juggernaut biatch");
		juggerNut.takeDamage(10);
    }
    // This should fail due to no energy points left
    alice.attack("dummy target");
    alice.beRepaired(5);
    
    std::cout << "\n=== Testing hit points depletion ===" << std::endl;
    // Test taking fatal damage
    bob.takeDamage(20); // More than the 8 hit points Bob has left
    // These should fail due to no hit points left
    bob.attack("enemy");
    bob.beRepaired(10);
    
    std::cout << "\n=== Testing destructor calls ===" << std::endl;
    return 0;
}
