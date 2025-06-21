#include "AAnimal.hpp"
#include "Dog.hpp"
#include "Cat.hpp"
#include <iostream>

int main() {
    std::cout << "--- Testing AAnimal, Dog, and Cat with polymorphism ---" << std::endl;
    
    // Uncommenting this would give a compilation error since AAnimal is abstract
    // AAnimal* meta = new AAnimal();
    
    const AAnimal* dawg = new Dog();
    const AAnimal* puss = new Cat();
    
    std::cout << "\n" << dawg->getType() << "... thats the animal" << std::endl;
    std::cout << "\n" << puss->getType() << "... thats the animal" << std::endl;
    
    puss->makeSound(); // cat sound
    dawg->makeSound(); // dog sound
    
    delete puss;
    delete dawg;
    
    std::cout << "\n--- Testing deep copy ---" << std::endl;
    
    Dog dog1;
    {
        Dog dog2 = dog1; // Copy constructor
    } // dog2 destructor called here
    
    std::cout << "\ndog1 still exists outside the scope" << std::endl;
    dog1.makeSound();
    
    // Testing array of animals
    std::cout << "\n--- Testing array of animals ---" << std::endl;
    const int numAnimals = 4;
    AAnimal* animals[numAnimals];
    
    for (int i = 0; i < numAnimals / 2; i++)
        animals[i] = new Dog();
        
    for (int i = numAnimals / 2; i < numAnimals; i++)
        animals[i] = new Cat();

    for (int i = 0; i < numAnimals; i++)
        delete animals[i];
    
    return 0;
}
