#include "Dog.hpp"

Dog::Dog() : Animal(), brain(new Brain()) {
    std::cout << "Dog default constructor called" << std::endl;
    this->type = "Dog";
}

Dog::Dog(const Dog& src) : Animal(src) {
    std::cout << "Dog copy constructor called" << std::endl;
    this->brain = new Brain(*src.brain); // Deep copy
}

Dog& Dog::operator=(const Dog& src) {
    std::cout << "Dog copy assignment operator called" << std::endl;
    if (this != &src) {
        Animal::operator=(src);
        delete brain; // Delete the old brain
        brain = new Brain(*src.brain); // Deep copy
    }
    return *this;
}

Dog::~Dog() {
    std::cout << "Dog destructor called" << std::endl;
    delete brain;
}

void Dog::makeSound() const {
    std::cout << "Woof! Woof!" << std::endl;
}

Brain* Dog::getBrain() const {
    return this->brain;
}
