#include "Zombie.hpp"

Zombie* zombieHorde( int N, std::string name )
{
	if (N <= 0)
		return (NULL);
	
	Zombie *Horde = new Zombie[N];
	
	for (int i = 0; i < N; i++)
	{
		Horde[i].setName(name);
	}
	return (Horde);
}

void	wipeHorde( int N, Zombie *Horde)
{
	if (N <= 0 || Horde == NULL)
		return ;
	delete[] Horde;
}
