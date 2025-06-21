#include "Cat.hpp"
#include <iostream>

Cat::Cat() : AAnimal() {
    std::cout << "Cat default constructor called" << std::endl;
    this->type = "Cat";
    this->brain = new Brain();
}

Cat::Cat(const Cat& src) : AAnimal(src) {
    std::cout << "Cat copy constructor called" << std::endl;
    this->brain = new Brain(*src.brain);
}

Cat& Cat::operator=(const Cat& src) {
    std::cout << "Cat copy assignment operator called" << std::endl;
    if (this != &src) {
        AAnimal::operator=(src);
        delete this->brain;
        this->brain = new Brain(*src.brain);
    }
    return *this;
}

Cat::~Cat() {
    std::cout << "Cat destructor called" << std::endl;
    delete this->brain;
}

void Cat::makeSound() const {
    std::cout << "Meow! Meow!" << std::endl;
}
