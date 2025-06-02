#include "phonebook.hpp"
#include "contact.hpp"

int main() {
	PhoneBook phoneBook;
	std::string command;
	
	std::cout << "Welcome to the awesome 80s phonebook!" << std::endl;
	
	while (true) {
		std::cout << "Enter a command (ADD, SEARCH, EXIT): ";
		std::getline(std::cin, command);
		
		if (command == "ADD") {
			phoneBook.addContact();
		}
		else if (command == "SEARCH") {
			phoneBook.searchContacts();
		}
		else if (command == "EXIT") {
			std::cout << "Goodbye!" << std::endl;
			break;
		}
		else if (!command.empty()) {
			std::cout << "Invalid command. Please use ADD, SEARCH, or EXIT." << std::endl;
		}
	}
	
	return 0;
}