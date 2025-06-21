#include "Ice.hpp"
#include "ICharacter.hpp"
#include <iostream>

Ice::Ice() : AMateria("ice") {
    // std::cout << "Ice default constructor called" << std::endl;
}

Ice::Ice(const Ice& src) : AMateria(src) {
    // std::cout << "Ice copy constructor called" << std::endl;
}

Ice& Ice::operator=(const Ice& src) {
    // std::cout << "Ice assignment operator called" << std::endl;
    if (this != &src) {
        AMateria::operator=(src);
    }
    return *this;
}

Ice::~Ice() {
    // std::cout << "Ice destructor called" << std::endl;
}

AMateria* Ice::clone() const {
    return new Ice(*this);
}

void Ice::use(ICharacter& target) {
    std::cout << "* shoots an ice bolt at " << target.getName() << " *" << std::endl;
}
