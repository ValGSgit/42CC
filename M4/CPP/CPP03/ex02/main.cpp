#include "ClapTrap.hpp"
#include "ScavTrap.hpp"
#include "FragTrap.hpp"
#include <iostream>

void runTest(const std::string& testName) {
    std::cout << "\n----------------------------------------------" << std::endl;
    std::cout << "TEST: " << testName << std::endl;
    std::cout << "----------------------------------------------" << std::endl;
}

int main()
{
    runTest("Constructor Chaining");
    std::cout << "The constructor of the class to be inherited runs before the constructor of the class it inherits to" << std::endl;
    {
        std::cout << "\n1. Creating a ClapTrap:" << std::endl;
        ClapTrap clap("ClapDude");
        
        std::cout << "\n2. Creating a ScavTrap:" << std::endl;
        ScavTrap scav("ScavDude");
        
        std::cout << "\n3. Creating a FragTrap:" << std::endl;
        FragTrap frag("FragDude");
        
        std::cout << "\n4. Destruction order is reverse of construction?" << std::endl;
        // Objects will be destroyed in reverse order when this scope exits
    }
    
    runTest("ClapTrap Functionality");
    {
        ClapTrap bob("Bob");
        std::cout << "Attack demonstration:" << std::endl;
        bob.attack("target dummy");
        
        std::cout << "\nDamage demonstration:" << std::endl;
        bob.takeDamage(5);
        
        std::cout << "\nRepair demonstration:" << std::endl;
        bob.beRepaired(3);
    }
    
    runTest("ScavTrap Functionality");
    {
        ScavTrap sam("Sam");
        std::cout << "Attack demonstration (should be different from ClapTrap):" << std::endl;
        sam.attack("intruder");
        
        std::cout << "\nDamage and repair:" << std::endl;
        sam.takeDamage(30);
        sam.beRepaired(15);
        
        std::cout << "\nUnique ability - Guard Gate:" << std::endl;
        sam.guardGate();
    }
    
    runTest("FragTrap Functionality");
    {
        FragTrap finn("Finn");
        std::cout << "Attack demonstration:" << std::endl;
        finn.attack("monster");
        
        std::cout << "\nDamage and repair:" << std::endl;
        finn.takeDamage(40);
        finn.beRepaired(20);
        
        std::cout << "\nUnique ability - High Fives:" << std::endl;
        finn.highFivesGuys();
    }
    
    runTest("Points Limitation (Hitpoints, attack, energy, etc...)");
    {
        std::cout << "1. ClapTrap energy depletion test:" << std::endl;
        ClapTrap a("The Chef");
        ClapTrap kK("Some poor bastard");
        kK.beRepaired(80);
        std::cout << "Depleting energy points..." << std::endl;
        for (int i = 0; i < 10; i++) {
            std::cout << "Attack " << (i+1) << "/10: ";
            a.attack("Some poor bastard");
            kK.takeDamage(10);
        }
        std::cout << "\nTrying to attack with no energy: ";
        a.attack("target");
        std::cout << "Trying to repair with no energy: ";
        a.beRepaired(5);
        
        std::cout << "\n2. Hit points reduction test:" << std::endl;
        ClapTrap c("Carlos");
        std::cout << "Taking fatal damage: ";
        c.takeDamage(20);
        std::cout << "Trying to attack with no hit points: ";
        c.attack("target");
        std::cout << "Trying to repair with no hit points: ";
        c.beRepaired(5);
    }
    
    runTest("Class Attribute Differences");
    {
        ClapTrap clap("C");
        ScavTrap scav("S");
        FragTrap frag("F");
        
        std::cout << "Attack damage demonstration:" << std::endl;
        clap.attack("target");
        scav.attack("target");
        frag.attack("target");
        
        std::cout << "\nHit points demonstration:" << std::endl;
        clap.takeDamage(8);
        scav.takeDamage(69);
        frag.takeDamage(99);
    }
    
    runTest("Copy Construction and Assignment");
    {
        FragTrap original("The dude");
        std::cout << "\nCreating a copy with copy constructor:" << std::endl;
        FragTrap copy(original);
        
        std::cout << "\nTesting copy behavior:" << std::endl;
        original.takeDamage(30);
        std::cout << "The dude after damage: ";
        original.attack("target");
        
        std::cout << "Copy of The dude (should be unaffected): ";
        copy.attack("target");
        
    }
    
    std::cout << "\n----------------------------------------------" << std::endl;
    std::cout << "All tests completed - watch destruction order" << std::endl;
    std::cout << "----------------------------------------------" << std::endl;\
    return 0;
}
