#ifndef ZOMBIE_HPP
# define ZOMBIE_HPP

# include <iostream>
# include <string>

class	Zombie {

	private:
		std::string _name;
	
	public:
		// Constructor
		Zombie();
		Zombie(std::string name);
		
		// Destructor
		~Zombie();

		// Member function
		void	announce( void );
		void	setName( std::string name );
	};
	
// Non-member functions	
Zombie* zombieHorde( int N, std::string name );
Zombie* newZombie( std::string name );
void randomChump( std::string name );
void	wipeHorde( int N, Zombie *Horde);

#endif