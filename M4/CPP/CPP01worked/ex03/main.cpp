// Implement a Weapon class that has:
// • A private attribute type, which is a string.
// • A getType() member function that returns a constant reference to type.
// • A setType() member function that sets type using the new value passed as a pa-
// rameter.
// Now, create two classes: HumanA and HumanB. They both have a Weapon and
// a name. They also have a member function attack() that displays (without the angle
// brackets):
// <name> attacks with their <weapon type>
// HumanA and HumanB are almost identical except for these two small details:
// • While HumanA takes the Weapon in its constructor, HumanB does not.
// • HumanB may not always have a weapon, whereas HumanA will always be
// armed.
// 9Memory allocation, pointers to members,
// references and switch statements
// C++ - Module 01
// If your implementation is correct, executing the following code will print an attack
// with crude spiked club followed by a second attack with some other type of club for
// both test cases:
// int main()
// {
// {
// Weapon
// }
// {
// }
// }
// club = Weapon(crude spiked club);
// HumanA bob(Bob, club);
// bob.attack();
// club.setType(some other type of club);
// bob.attack();
// Weapon
// club = Weapon(crude spiked club);
// HumanB jim(Jim);
// jim.setWeapon(club);
// jim.attack();
// club.setType(some other type of club);
// jim.attack();
// return 0;
// Do not forget to check for memory leaks.
// In which case do you think it would be best to use a pointer to
// Weapon? And a reference to Weapon? Why? Think about it before
// starting this exercise.
