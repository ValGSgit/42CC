#include "Weapon.hpp"
#include "HumanA.hpp"
#include "HumanB.hpp"

// Now, create two classes: HumanA and HumanB. They both have a Weapon and
// a name. They also have a member function attack() that displays (without the angle
// brackets):
// <name> attacks with their <weapon type>
// HumanA and HumanB are almost identical except for these two small details:
// • While HumanA takes the Weapon in its constructor, HumanB does not.
// • HumanB may not always have a weapon, whereas HumanA will always be
// armed.

// If your implementation is correct, executing the following code will print an attack
// with "crude spiked club" followed by a second attack with "some other type of club" for
// both test cases:
int main()
{
	{
		Weapon club = Weapon("bag of dried nuts");
		HumanA bob("Bob", club);
		bob.attack();
		club.setType("salami nun-chucks");
		bob.attack();
	}
	{
		Weapon club = Weapon("grandmother's hard candy");
		HumanB jim("Jim");
		jim.setWeapon(club);
		jim.attack();
		club.setType("baguette crossbow");
		jim.attack();
	}
	return 0;
}