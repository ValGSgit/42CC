#include "Animal.hpp"
#include "Dog.hpp"
#include "Cat.hpp"
#include "WrongAnimal.hpp"
#include "WrongCat.hpp"
#include <iostream>

int main()
{
    std::cout << "--- Testing Animal, Dog, and Cat with polymorphism ---" << std::endl;
    const Animal* meta = new Animal();
    const Animal* dawg = new Dog();
    const Animal* puss = new Cat();
    // Animal wtf = Dog();
    
    std::cout << "\n" << dawg->getType() << "... thats the animal" << std::endl;
    std::cout << "\n" << puss->getType() << "... thats the animal" << std::endl;
    // std::cout << "\n" << wtf.getType() << "... thats the animal" << std::endl;

    puss->makeSound(); // cat sound
    dawg->makeSound(); // dog sound
    meta->makeSound(); // animal sound
    // wtf.makeSound();

    delete meta;
    delete puss;
    delete dawg;

    std::cout << "\n--- Testing WrongAnimal and WrongCat without polymorphism ---" << std::endl;
    const WrongAnimal* wrongMeta = new WrongAnimal();
    const WrongAnimal* wrongCat = new WrongCat();
    const Animal* Meta = new Animal();
    const Animal* elGato = new Cat();
   
    std::cout << wrongMeta->getType() << " is the type of wrong Animal." << std::endl;
    std::cout << wrongCat->getType() << " is the type of wrong Cat." << std::endl;
    std::cout << Meta->getType() << " is the type of normal Animal." << std::endl;
    std::cout << elGato->getType() << " is the type of normal Cat." << std::endl;
    
    wrongCat->makeSound();
    wrongMeta->makeSound();
    elGato->makeSound();
    Meta->makeSound();
    
    delete wrongMeta;
    delete wrongCat;
    delete elGato;
    delete Meta;

    std::cout << "\n--- Additional tests ---" << std::endl;

    std::cout << "\nTesting with regular objects:" << std::endl;
    Animal regularAnimal;
    Dog regularDog;
    Cat regularCat;
    
    regularAnimal.makeSound();
    regularDog.makeSound();
    regularCat.makeSound();

    std::cout << "\nTesting copy constructor:" << std::endl;
    Dog originalDog;
    Dog copiedDog = originalDog;

    std::cout << "Original dog type: " << originalDog.getType() << std::endl;
    std::cout << "Copied dog type: " << copiedDog.getType() << std::endl;
    
    std::cout << "\nTesting assignment operator:" << std::endl;
    Cat originalCat;
    Cat assignedCat;
    assignedCat = originalCat;

    std::cout << "Original cat type: " << originalCat.getType() << std::endl;
    std::cout << "Assigned cat type: " << assignedCat.getType() << std::endl;
    return 0;
}
