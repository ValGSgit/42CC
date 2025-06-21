#include "Zombie.hpp"

// First, implement a Zombie class. It has a private string attribute name.
// Add a member function void announce( void ); to the Zombie class. Zombies
// announce themselves as follows:
// <name>: BraiiiiiiinnnzzzZ...
// Do not print the angle brackets (< and >). For a zombie named Foo, the message
// would be:
// Foo: BraiiiiiiinnnzzzZ...
// Then, implement the following two functions:
// • Zombie* newZombie( std::string name );
// This function creates a zombie, names it, and returns it so you can use it outside
// of the function scope.
// • void randomChump( std::string name );
// This function creates a zombie, names it, and makes it announce itself.
// Now, what is the actual point of the exercise? You have to determine in which case
// it is better to allocate zombies on the stack or the heap.
// Zombies must be destroyed when you no longer need them. The destructor must print
// a message with the name of the zombie for debugging purposes.

int main () {
	std::cout << "=== Testing HEAP allocation (newZombie) ===" << std::endl;
	// Create zombie on heap - exists until deleted
	Zombie* heapZombie = newZombie("HeapZombie");
	Zombie* justTrudeau = newZombie("Another Heap Zombie");
	heapZombie->announce();
	justTrudeau->announce();
	
	std::cout << "\n=== Testing STACK allocation (randomChump) ===" << std::endl;
	// Create zombie on stack - automatically destroyed when function ends
	randomChump("StackZombie");
	randomChump("Another StackZombie");
	
	std::cout << "\n=== Cleaning up heap zombie ===" << std::endl;
	
	delete heapZombie;
	delete justTrudeau;
	
	std::cout << "\n=== Program ending ===" << std::endl;
	return 0;
}