#include "Character.hpp"
#include <iostream>

Character::Character(std::string const & name) : name(name), floorCount(0) {
    for (int i = 0; i < 4; i++) {
        this->inventory[i] = NULL;
    }
    for (int i = 0; i < 100; i++) {
        this->floor[i] = NULL;
    }
    // std::cout << "Character " << name << " created" << std::endl;
}

Character::Character(const Character& src) : name(src.name), floorCount(0) {
    for (int i = 0; i < 4; i++) {
        if (src.inventory[i] != NULL) {
            this->inventory[i] = src.inventory[i]->clone();
        } else {
            this->inventory[i] = NULL;
        }
    }
    for (int i = 0; i < 100; i++) {
        this->floor[i] = NULL;
    }
    // std::cout << "Character " << name << " copied" << std::endl;
}

Character& Character::operator=(const Character& src) {
    // std::cout << "Character assignment operator called" << std::endl;
    if (this != &src) {
        this->name = src.name;
        for (int i = 0; i < 4; i++) {
            if (this->inventory[i] != NULL) {
                delete this->inventory[i];
                this->inventory[i] = NULL;
            }
        }
        for (int i = 0; i < 4; i++) {
            if (src.inventory[i] != NULL) {
                this->inventory[i] = src.inventory[i]->clone();
            } else {
                this->inventory[i] = NULL;
            }
        }
        
        // don't copy floor items bc they are specific to each instance
    }
    return *this;
}

Character::~Character() {
    for (int i = 0; i < 4; i++) {
        if (this->inventory[i] != NULL) {
            delete this->inventory[i];
        }
    }

    for (int i = 0; i < 100; i++) {
        if (this->floor[i] != NULL) {
            delete this->floor[i];
        }
    }
    // std::cout << "Character " << name << " destroyed" << std::endl;
}

std::string const & Character::getName() const {
    return this->name;
}

void Character::equip(AMateria* m) {
    if (m == NULL) {
        return;
    }
    
    for (int i = 0; i < 4; i++)
    {
        if (this->inventory[i] == NULL)
        {
            this->inventory[i] = m;
            // std::cout << "Character " << this->name << " equipped " << m->getType() << " in slot " << i << std::endl;
            return;
        }
    }
    // std::cout << "Character " << this->name << " cannot equip " << m->getType() << ": inventory full" << std::endl;
    delete m;
}

void Character::unequip(int idx) {
    if (idx < 0 || idx >= 4 || this->inventory[idx] == NULL) {
        // std::cout << "Character " << this->name << " cannot unequip from slot " << idx << std::endl;
        return;
    }
    
    // Store the unequipped materia on the floor
    if (floorCount < 100) {
        this->floor[floorCount] = this->inventory[idx];
        floorCount++;
        // std::cout << "Character " << this->name << " unequipped " << this->inventory[idx]->getType() << " from slot " << idx << std::endl;
        this->inventory[idx] = NULL;
    } else {
        // If floor is full, delete the materia
        delete this->inventory[idx];
        this->inventory[idx] = NULL;
    }
}

void Character::use(int idx, ICharacter& target) {
    if (idx < 0 || idx >= 4 || this->inventory[idx] == NULL) {
        // std::cout << "Character " << this->name << " cannot use materia from slot " << idx << std::endl;
        return;
    }
    
    this->inventory[idx]->use(target);
}
