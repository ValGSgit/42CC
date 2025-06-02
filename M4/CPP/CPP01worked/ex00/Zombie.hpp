#ifndef ZOMBIE_HPP
# define ZOMBIE_HPP

# include <iostream>
# include <string>

class	Zombie {

	private:
		std::string _name;
	
	public:
		// Constructor
		Zombie(std::string name);
		
		// Destructor
		~Zombie();
		
		// Member function
		void	announce( void );
};

// Non-member functions
Zombie* newZombie( std::string name );
void randomChump( std::string name );

#endif