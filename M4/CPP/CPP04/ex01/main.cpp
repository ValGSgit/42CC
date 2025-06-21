#include "Animal.hpp"
#include "Dog.hpp"
#include "Cat.hpp"
#include "Brain.hpp"
#include <iostream>

int main()
{
    // Basic test from the subject
    std::cout << "--- Basic test from the subject ---" << std::endl;
    const Animal* j = new Dog();
    const Animal* i = new Cat();
    
    delete j;
    delete i;
    
    std::cout << "\n--- Test with an array of Animal objects ---" << std::endl;
    const int numAnimals = 6;
    // Animal **animals = new Animal*[numAnimals];
    Animal *animals[numAnimals];

    for (int i = 0; i < numAnimals; i++) {
        if (i < numAnimals / 2) {
            animals[i] = new Dog();
        } else {
            animals[i] = new Cat();
        }
    }

    for (int i = 0; i < numAnimals; i++) {
        delete animals[i];
    }
    // delete []animals;
    
    std::cout << "\n--- Test for deep copies ---" << std::endl;

    Dog* originalDog = new Dog();
    Brain* dogBrain = originalDog->getBrain();
    
    dogBrain->setIdea(0, "I want to chase a ball!");
    dogBrain->setIdea(1, "I need to bark at the mailman!");
    
    std::cout << "Original Dog idea 0: " << dogBrain->getIdea(0) << std::endl;
    std::cout << "Original Dog idea 1: " << dogBrain->getIdea(1) << std::endl;
    
    Dog* copyDog = new Dog(*originalDog);
    Brain* copyBrain = copyDog->getBrain();
    
    std::cout << "Copy Dog idea 0: " << copyBrain->getIdea(0) << std::endl;
    std::cout << "Copy Dog idea 1: " << copyBrain->getIdea(1) << std::endl;
 
    dogBrain->setIdea(0, "I've changed my mind. I want to sleep!");
    
    std::cout << "\nAfter changing idea 0 in the original Dog:" << std::endl;
    std::cout << "Original Dog idea 0: " << dogBrain->getIdea(0) << std::endl;
    std::cout << "Copy Dog idea 0: " << copyBrain->getIdea(0) << std::endl;
    
    delete originalDog;
    delete copyDog;
    
    std::cout << "\n--- Test copy assignment operator ---" << std::endl;
    
    Cat* originalCat = new Cat();
    Cat* assignedCat = new Cat();
    
    Brain* catBrain = originalCat->getBrain();
    catBrain->setIdea(0, "I want to sleep on the couch!");
    catBrain->setIdea(1, "I need to ignore my owner!");
    
    *assignedCat = *originalCat;
    Brain* assignedBrain = assignedCat->getBrain();
    
    std::cout << "Original Cat idea 0: " << catBrain->getIdea(0) << std::endl;
    std::cout << "Assigned Cat idea 0: " << assignedBrain->getIdea(0) << std::endl;
    
    catBrain->setIdea(0, "I've changed my mind. I want to play with yarn!");
    

    std::cout << "\nAfter changing idea 0 in the original Cat:" << std::endl;
    std::cout << "Original Cat idea 0: " << catBrain->getIdea(0) << std::endl;
    std::cout << "Assigned Cat idea 0: " << assignedBrain->getIdea(0) << std::endl;
    
    delete originalCat;
    delete assignedCat;
    
    return 0;
}
