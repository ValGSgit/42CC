// Write a program that contains:
// • A string variable initialized to HI THIS IS BRAIN.
// • stringPTR: a pointer to the string.
// • stringREF: a reference to the string.
// Your program must print:
// • The memory address of the string variable.
// • The memory address held by stringPTR.
// • The memory address held by stringREF.
// And then:
// • The value of the string variable.
// • The value pointed to by stringPTR.
// • The value pointed to by stringREF.
// That’s all—no tricks. The goal of this exercise is to demystify references, which may
// seem completely new. Although there are some small differences, this is simply another
// syntax for something you already do: address manipulation.
#include <iostream>
#include <iomanip>

int main(void)
{
	std::string intro = "HI THIS IS BRAIN";
	std::string *stringPTR = &intro;    // Pointer to intro (stores address)
	std::string &stringREF = intro;     // Reference to intro (alias)

	// Print memory addresses
	std::cout << "Address of intro: " << &intro << std::endl;
	std::cout << "Address held by stringPTR: " << stringPTR << std::endl;
	std::cout << "Address held by stringREF: " << &stringREF << std::endl;
	
	std::cout << std::endl;
	
	// Print values
	std::cout << "Value of intro: " << intro << std::endl;
	std::cout << "Value pointed to by stringPTR: " << *stringPTR << std::endl;
	std::cout << "Value pointed to by stringREF: " << stringREF << std::endl;
	
	return 0;
}