#include "MateriaSource.hpp"
#include <iostream>

MateriaSource::MateriaSource() : count(0) {
    for (int i = 0; i < 4; i++) {
        this->templates[i] = NULL;
    }
    // std::cout << "MateriaSource created" << std::endl;
}

MateriaSource::MateriaSource(const MateriaSource& src) : count(src.count) {

    for (int i = 0; i < 4; i++) {
        if (src.templates[i] != NULL) {
            this->templates[i] = src.templates[i]->clone();
        } else {
            this->templates[i] = NULL;
        }
    }
    // std::cout << "MateriaSource copied" << std::endl;
}

MateriaSource& MateriaSource::operator=(const MateriaSource& src) {
    // std::cout << "MateriaSource assignment operator called" << std::endl;
    if (this != &src) {
        for (int i = 0; i < 4; i++) {
            if (this->templates[i] != NULL) {
                delete this->templates[i];
                this->templates[i] = NULL;
            }
        }
        for (int i = 0; i < 4; i++) {
            if (src.templates[i] != NULL) {
                this->templates[i] = src.templates[i]->clone();
            }
        }
        
        this->count = src.count;
    }
    return *this;
}

MateriaSource::~MateriaSource() {
    for (int i = 0; i < 4; i++) {
        if (this->templates[i] != NULL) {
            delete this->templates[i];
        }
    }
    // std::cout << "MateriaSource destroyed" << std::endl;
}

void MateriaSource::learnMateria(AMateria* m) {
    if (m == NULL) {
        return;
    }
    
    if (this->count < 4) {
        this->templates[this->count] = m;
        this->count++;
        // std::cout << "MateriaSource learned " << m->getType() << std::endl;
    } else {
        // std::cout << "MateriaSource cannot learn more materias: storage full" << std::endl;
        delete m;
    }
}

AMateria* MateriaSource::createMateria(std::string const & type) {
    for (int i = 0; i < this->count; i++) {
        if (this->templates[i]->getType() == type) {
            // std::cout << "MateriaSource created " << type << std::endl;
            return this->templates[i]->clone();
        }
    }
    
    // std::cout << "MateriaSource cannot create unknown materia type: " << type << std::endl;
    return NULL;
}
