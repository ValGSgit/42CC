#ifndef PHONEBOOK_HPP
#define PHONEBOOK_HPP

#include "contact.hpp"

class PhoneBook
{
	private:
		Contact contacts[8];
		int currentIndex;
		int totalContacts;

	public:
		PhoneBook();
		~PhoneBook();
		
		void addContact();
		void searchContacts() const;
		void displayContact(int index) const;
		void displayContactList() const;
		std::string truncateString(const std::string& str) const;
		bool isValidIndex(const std::string& input, int& index) const;
		bool isEmptySpaces(const std::string& str) const;
};

#endif