#include "AMateria.hpp"
#include "Ice.hpp"
#include "Cure.hpp"
#include "Character.hpp"
#include "MateriaSource.hpp"
#include <iostream>

int main() {
    std::cout << "\n--- Testing basic functionality as shown in the subject ---" << std::endl;
    IMateriaSource* src = new MateriaSource();
    src->learnMateria(new Ice());
    src->learnMateria(new Cure());
    
    ICharacter* me = new Character("me");
    
    AMateria* tmp;
    tmp = src->createMateria("ice");
    me->equip(tmp);
    tmp = src->createMateria("cure");
    me->equip(tmp);
    
    ICharacter* bob = new Character("bob");
    
    me->use(0, *bob);
    me->use(1, *bob);
    
    delete bob;
    delete me;
    delete src;
    
    std::cout << "\n--- Additional tests ---" << std::endl;
    std::cout << "Testing Character deep copy:" << std::endl;
    
    Character* ash = new Character("Ash");
    ash->equip(new Ice());
    ash->equip(new Cure());
    
    Character* gary = new Character(*ash); // Copy constructor
    
    std::cout << "\nAsh uses materia:" << std::endl;
    ash->use(0, *gary);
    ash->use(1, *gary);
    
    std::cout << "\nGary uses materia (should be the same types):" << std::endl;
    gary->use(0, *ash);
    gary->use(1, *ash);
    
    std::cout << "\nTesting unequip:" << std::endl;
    ash->unequip(0); // This should store the materia on the floor
    ash->use(0, *gary); // This should do nothing
    
    // Equipping a new materia after unequipping
    ash->equip(new Ice());
    ash->use(0, *gary); // Should work now
    
    std::cout << "\nTesting equipping more than 4 materias:" << std::endl;
    ash->equip(new Ice());
    ash->equip(new Cure());
    ash->equip(new Ice()); // This should be in slot 3
    ash->equip(new Cure()); // This should fail (inventory full)
    
    ash->use(0, *gary);
    ash->use(1, *gary);
    ash->use(2, *gary);
    ash->use(3, *gary);
    
    std::cout << "\nTesting using non-existent materias:" << std::endl;
    ash->use(4, *gary); // This should do nothing (invalid index)
    
    std::cout << "\nTesting MateriaSource functionality:" << std::endl;
    MateriaSource ms;
    ms.learnMateria(new Ice());
    ms.learnMateria(new Cure());
    ms.learnMateria(new Ice());
    ms.learnMateria(new Cure());
    ms.learnMateria(new Ice()); // This should fail (storage full)
    
    AMateria* ice = ms.createMateria("ice");
    AMateria* cure = ms.createMateria("cure");
    AMateria* unknown = ms.createMateria("unknown"); // This should return NULL
    
    if (ice) {
        std::cout << "Created materia of type: " << ice->getType() << std::endl;
        delete ice;
    }
    
    if (cure) {
        std::cout << "Created materia of type: " << cure->getType() << std::endl;
        delete cure;
    }
    
    if (unknown == NULL) {
        std::cout << "Unknown materia type couldn't be created (NULL)" << std::endl;
    }
    
    delete ash;
    delete gary;
    
    return 0;
}
