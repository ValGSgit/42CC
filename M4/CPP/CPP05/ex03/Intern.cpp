#include "Intern.hpp"
// #include <>
#include "ShrubberyCreationForm.hpp"
#include "RobotomyRequestForm.hpp"
#include "PresidentialPardonForm.hpp"
#include <iostream>

Intern::Intern() {
    std::cout << "Intern default constructor called" << std::endl;
}

Intern::Intern(const Intern &src) {
    std::cout << "Intern copy constructor called" << std::endl;
    *this = src;
}

Intern &Intern::operator=(const Intern &rhs) {
    std::cout << "Intern copy assignment operator called" << std::endl;
    (void)rhs;
    return *this;
}

Intern::~Intern() {
    std::cout << "Intern destructor called" << std::endl;
}

AForm *Intern::createShrubberyForm(const std::string &target) const {
    return new ShrubberyCreationForm(target);
}

AForm *Intern::createRobotomyForm(const std::string &target) const {
    return new RobotomyRequestForm(target);
}

AForm *Intern::createPresidentialForm(const std::string &target) const {
    return new PresidentialPardonForm(target);
}

AForm *Intern::makeForm(const std::string &formName, const std::string &target) {
    const std::string formNames[3] = {
        "shrubbery creation",
        "robotomy request",
        "presidential pardon"
    };

    FormCreator formCreators[3] = {
        &Intern::createShrubberyForm,
        &Intern::createRobotomyForm,
        &Intern::createPresidentialForm
    };

    for (int i = 0; i < 3; i++) {
        if (formName == formNames[i]) {
            AForm *form = (this->*formCreators[i])(target);
            std::cout << "Intern creates " << form->getName() << std::endl;
            return form;
        }
    }
    throw FormNotFoundException();
}

const char *Intern::FormNotFoundException::what() const throw() {
    return "Error: Form not found";
}
