#include "AMateria.hpp"
#include "ICharacter.hpp"
#include <iostream>

AMateria::AMateria() : type("") {
    // std::cout << "AMateria default constructor called" << std::endl;
}

AMateria::AMateria(std::string const & type) : type(type) {
    // std::cout << "AMateria parameterized constructor called" << std::endl;
}

AMateria::AMateria(const AMateria& src) : type(src.type) {
    // std::cout << "AMateria copy constructor called" << std::endl;
}

AMateria& AMateria::operator=(const AMateria& src) {
    // std::cout << "AMateria assignment operator called" << std::endl;
    if (this != &src) {
        this->type = src.type;
    }
    return *this;
}

AMateria::~AMateria() {
    // std::cout << "AMateria destructor called" << std::endl;
}

std::string const & AMateria::getType() const {
    return this->type;
}

void AMateria::use(ICharacter& target) {
    std::cout << "* AMateria: nothing happens to " << target.getName() << " *" << std::endl;
}
