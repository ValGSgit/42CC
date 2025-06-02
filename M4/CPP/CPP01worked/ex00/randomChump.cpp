#include "Zombie.hpp"

// Creates a zombie on the STACK and makes it announce itself
// The zombie is automatically destroyed when function ends
void randomChump(std::string name) {
	Zombie zombie(name);
	zombie.announce();
	// Zombie is automatically destroyed here when going out of scope
}