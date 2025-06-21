#include "Cure.hpp"
#include "ICharacter.hpp"
#include <iostream>

Cure::Cure() : AMateria("cure") {
    // std::cout << "Cure default constructor called" << std::endl;
}

Cure::Cure(const Cure& src) : AMateria(src) {
    // std::cout << "Cure copy constructor called" << std::endl;
}

Cure& Cure::operator=(const Cure& src) {
    // std::cout << "Cure assignment operator called" << std::endl;
    if (this != &src) {
        AMateria::operator=(src);
    }
    return *this;
}

Cure::~Cure() {
    // std::cout << "Cure destructor called" << std::endl;
}

AMateria* Cure::clone() const {
    return new Cure(*this);
}

void Cure::use(ICharacter& target) {
    std::cout << "* heals " << target.getName() << "'s wounds *" << std::endl;
}
