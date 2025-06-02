#include "Zombie.hpp"

// Constructor - initializes the zombie with a name
Zombie::Zombie(std::string name) : _name(name) {
	std::cout << _name << " has been created!" << std::endl;
}

// Destructor - called when zombie is destroyed
Zombie::~Zombie() {
	std::cout << _name << " has been destroyed!" << std::endl;
}

// Announce function - zombie announces itself
void Zombie::announce(void) {
	std::cout << _name << ": BraiiiiiiinnnzzzZ..." << std::endl;
}