#include "phonebook.hpp"
#include "contact.hpp"

bool Contact::isEmpty() const {
	return firstName.empty();
}

PhoneBook::PhoneBook() : currentIndex(0), totalContacts(0) {}

PhoneBook::~PhoneBook() {}

std::string PhoneBook::truncateString(const std::string& str) const {
	if (str.length() <= 10)
	return str;
	return str.substr(0, 9) + ".";
}

bool PhoneBook::isEmptySpaces(const std::string& str) const {
	for (size_t i = 0; i < str.length(); i++) {
		if (!std::isspace(str[i]))
			return false;
	}
	return true;
}

bool PhoneBook::isValidIndex(const std::string& input, int& index) const
{
	if (input.empty() || isEmptySpaces(input)) {
		return false;
	}
	for (size_t i = 0; i < input.length(); i++) {
		if (!std::isdigit(input[i]))
			return false;
	}
	std::stringstream ss(input);
	ss >> index;
	return (index >= 0 && index < totalContacts);
}

void PhoneBook::addContact() {
	std::string input;
	Contact newContact;
	
	std::cout << "Enter first name: ";
	std::getline(std::cin, input);
	if (input.empty() || isEmptySpaces(input)) {
		std::cout << "Error: First name cannot be empty unless you're a ghost." << std::endl;
		return;
	}
	newContact.setFirstName(input);
	std::cout << "Enter last name: ";
	std::getline(std::cin, input);
	if (input.empty() || isEmptySpaces(input)) {
		std::cout << "Error: Last name cannot be empty unless you're an orphan." << std::endl;
		return;
	}
	newContact.setLastName(input);
	std::cout << "Enter nickname: ";
	std::getline(std::cin, input);
	if (input.empty() || isEmptySpaces(input)) {
		std::cout << "Error: Nickname cannot be empty." << std::endl;
		return;
	}
	newContact.setNickname(input);
	std::cout << "Enter phone number: ";
	std::getline(std::cin, input);
	if (input.empty() || isEmptySpaces(input)) {
		std::cout << "Error: Phonebook without phone numbers is like Facebook without faces." << std::endl;
		return;
	}
	newContact.setPhoneNumber(input);
	
	std::cout << "Enter darkest secret: ";
	std::getline(std::cin, input);
	if (input.empty() || isEmptySpaces(input)) {
		std::cout << "Error: The Phonebook feeds on secrets, please provide one." << std::endl;
		return;
	}
	newContact.setDarkestSecret(input);
	
	contacts[currentIndex] = newContact;
	currentIndex = (currentIndex + 1) % 8;
	if (totalContacts < 8)
		totalContacts++;
	
	std::cout << "Contact added successfully!" << std::endl;
}

void PhoneBook::displayContactList() const {
	if (totalContacts == 0) {
		std::cout << "No contacts to display." << std::endl;
		return;
	}
	
	std::cout << std::setw(10) << "Index" << "|";
	std::cout << std::setw(10) << "First Name" << "|";
	std::cout << std::setw(10) << "Last Name" << "|";
	std::cout << std::setw(10) << "Nickname" << std::endl;
	
	for (int i = 0; i < totalContacts; i++) {
		std::cout << std::setw(10) << i << "|";
		std::cout << std::setw(10) << truncateString(contacts[i].getFirstName()) << "|";
		std::cout << std::setw(10) << truncateString(contacts[i].getLastName()) << "|";
		std::cout << std::setw(10) << truncateString(contacts[i].getNickname()) << std::endl;
	}
}

void PhoneBook::displayContact(int index) const {
	if (index < 0 || index >= totalContacts) {
		std::cout << "Error: Invalid index." << std::endl;
		return;
	}
	
	const Contact& contact = contacts[index];
	std::cout << "First Name: " << contact.getFirstName() << std::endl;
	std::cout << "Last Name: " << contact.getLastName() << std::endl;
	std::cout << "Nickname: " << contact.getNickname() << std::endl;
	std::cout << "Phone Number: " << contact.getPhoneNumber() << std::endl;
	std::cout << "Darkest Secret: " << contact.getDarkestSecret() << std::endl;
}

void PhoneBook::searchContacts() const {
	if (totalContacts == 0) {
		std::cout << "No contacts to search." << std::endl;
		return;
	}
	
	displayContactList();
	
	std::string input;
	std::cout << "Enter the index of the contact to display: ";
	std::getline(std::cin, input);
	
	int index;
	if (!isValidIndex(input, index)) {
		std::cout << "Error: Invalid index." << std::endl;
		return;
	}
	displayContact(index);
}
