#include "Zombie.hpp"

void randomChump(std::string name) {
	Zombie zombie(name);
	zombie.announce();
	// Zombie is automatically destroyed here when going out of scope
}