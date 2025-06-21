#ifndef INTERN_HPP
#define INTERN_HPP

#include <string>
#include "AForm.hpp"

class Intern {
    public:
        Intern();
        Intern(const Intern &src);
        Intern &operator=(const Intern &rhs);
        ~Intern();

        // Main function to create a form
        AForm *makeForm(const std::string &formName, const std::string &target);

        class FormNotFoundException : public std::exception {
        public:
            virtual const char *what() const throw();
        };

    private:
        // Function pointers for form creation
        typedef AForm* (Intern::*FormCreator)(const std::string &target) const;
        
        // Helper methods to create forms
        AForm *createShrubberyForm(const std::string &target) const;
        AForm *createRobotomyForm(const std::string &target) const;
        AForm *createPresidentialForm(const std::string &target) const;
};

#endif
