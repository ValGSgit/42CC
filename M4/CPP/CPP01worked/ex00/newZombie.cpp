#include "Zombie.hpp"

// Creates a zombie on the HEAP and returns a pointer
// The zombie persists until explicitly deleted
Zombie* newZombie(std::string name) {
	return new Zombie(name);
}