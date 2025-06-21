#include "Zombie.hpp"

Zombie::Zombie(std::string name) : _name(name) {
	std::cout << _name << " has been created!" << std::endl;
}

Zombie::Zombie() : _name("DFL_Zombie Name") {
    std::cout << "Default constructor called for " << _name << std::endl;
}

Zombie::~Zombie() {
	std::cout << _name << " has been destroyed!" << std::endl;
}

void Zombie::announce(void) {
	std::cout << _name << ": BraiiiiiiinnnzzzZ..." << std::endl;
}

void Zombie::setName(std::string name) {
	_name = name;
}