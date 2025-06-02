// Time to create a horde of Zombies!
// Implement the following function in the appropriate file:
// Zombie* zombieHorde( int N, std::string name );
// It must allocate N Zombie objects in a single allocation. Then, it must initialize the
// zombies, giving each of them the name passed as a parameter. The function returns a
// pointer to the first zombie.
// Implement your own tests to ensure that your zombieHorde() function works as ex-
// pected. Try calling announce() for each of the zombies.
// Do not forget to use delete to deallocate all the zombies and check for memory
// leaks.
#include "Zombie.hpp"

int main(void)
{
	std::string name = "MoreZombies";
	int	N = 3;
	// Stack constructed Zombie
	Zombie Pepe = Zombie("Pepe");

	Pepe.announce();
	
	// Pepe gets deleted automatically at the end of function
	// Because its on the stack
	
	Zombie *Horde = zombieHorde(N, name);
	
	for (int i = 0; i < N; i++)
	{
		Horde[i].announce();
	}
	wipeHorde(N, Horde);
	std::cout << "Deleting single Zombie Pepe" << std::endl;
	return 0;
}