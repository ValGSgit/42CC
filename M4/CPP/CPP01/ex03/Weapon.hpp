#ifndef WEAPON_H
# define WEAPON_H

# include <iostream>
# include <string>

// Implement a Weapon class that has:
// • A private attribute type, which is a string.
// • A getType() member function that returns a constant reference to type.
// • A setType() member function that sets type using the new value passed as a pa-
// rameter.

class Weapon
{
	private:
		std::string type;
	
	public:
		Weapon();
		Weapon(const std::string& type);
		~Weapon();
		const std::string& getType() const;
		void setType(const std::string& newType);
};

#endif