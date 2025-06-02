#include "Zombie.hpp"

// Constructor - initializes the zombie with a name
Zombie::Zombie(std::string name) : _name(name) {
	std::cout << _name << " has been created!" << std::endl;
}

Zombie::Zombie() : _name("DFL_Zombie Name") {
    std::cout << "Default constructor called for " << _name << std::endl;
}

// Destructor - called when zombie is destroyed
Zombie::~Zombie() {
	std::cout << _name << " has been destroyed!" << std::endl;
}

// Announce function - zombie announces itself
void Zombie::announce(void) {
	std::cout << _name << ": BraiiiiiiinnnzzzZ..." << std::endl;
}

// Set name function - sets the zombie's name
void Zombie::setName(std::string name) {
	_name = name;
}