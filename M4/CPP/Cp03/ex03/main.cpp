#include "DiamondTrap.hpp"
#include <iostream>


void runTest(const std::string& testName) {
    std::cout << "\n----------------------------------------------" << std::endl;
    std::cout << "TEST: " << testName << std::endl;
    std::cout << "----------------------------------------------" << std::endl;
}

int main()
{
    runTest("Construction Order Demo");
    {
        std::cout << "\n1. Creating DiamondTrap:" << std::endl;
        DiamondTrap diamond("Diamond");
		DiamondTrap xoxo("The dude");
        
        std::cout << "\n2. Who Am I test:" << std::endl;
        diamond.whoAmI();
    }
    runTest("Inherited Attributes Test");
    {
        DiamondTrap diamond("Jack");
        
        std::cout << "Attack test (should use ScavTrap's attack):" << std::endl;
        diamond.attack("enemy");
        
        std::cout << "\nHigh Fives test (from FragTrap):" << std::endl;
        diamond.highFivesGuys();
        
        std::cout << "\nGuard Gate test (from ScavTrap):" << std::endl;
        diamond.guardGate();
    }
    runTest("Stats Test");
    {
        ClapTrap clap("Clappy");
        ScavTrap scav("Scavvy");
        FragTrap frag("Fraggy");
        DiamondTrap diamond("Diamondy");
        
        std::cout << "Testing attack damage through attacks:" << std::endl;
        clap.attack("Some random person");
        scav.attack("Some random person");
        frag.attack("Some random person");
        diamond.attack("Some random person");
        
        std::cout << "\nEnergy points through multiple actions:" << std::endl;
        
		clap.beRepaired(280);
        for (int i = 0; i < 10; i++) {
            std::cout << "Attack " << i+1 << ": ";
            diamond.attack("Clappy");
			clap.takeDamage(30);
        }
		clap.attack("Diamondy");
    }
    runTest("Copy Operations Test");
    {
        DiamondTrap original("Original");
        std::cout << "\nCreating a copy with copy constructor:" << std::endl;
        DiamondTrap copy(original);
        
        std::cout << "\nWho am I for original:" << std::endl;
        original.whoAmI();
        
        std::cout << "Who am I for copy:" << std::endl;
        copy.whoAmI();
        
        std::cout << "\nTesting copy behavior:" << std::endl;
        original.takeDamage(30);
        std::cout << "Original after damage: ";
        original.attack("target");
        
        std::cout << "Copy (should be unaffected): ";
        copy.attack("target");

    }
    std::cout << "\n--------------------------------------------------------------" << std::endl;
    std::cout << "If destruction order and behaviours are correct then thats it" << std::endl;
    std::cout << "--------------------------------------------------------------" << std::endl;
    return 0;
}
