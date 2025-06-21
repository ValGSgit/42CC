#include "Cat.hpp"

Cat::Cat() : Animal(), brain(new Brain()) {
    std::cout << "Cat default constructor called" << std::endl;
    this->type = "Cat";
}

Cat::Cat(const Cat& src) : Animal(src) {
    std::cout << "Cat copy constructor called" << std::endl;
    this->brain = new Brain(*src.brain); // Deep copy
}

Cat& Cat::operator=(const Cat& src) {
    std::cout << "Cat copy assignment operator called" << std::endl;
    if (this != &src) {
        Animal::operator=(src);
        delete brain; // Delete the old brain
        brain = new Brain(*src.brain); // Deep copy
    }
    return *this;
}

Cat::~Cat() {
    std::cout << "Cat destructor called" << std::endl;
    delete brain;
}

void Cat::makeSound() const {
    std::cout << "Meow! Meow!" << std::endl;
}

Brain* Cat::getBrain() const {
    return this->brain;
}
