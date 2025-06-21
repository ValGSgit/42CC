#include "RobotomyRequestForm.hpp"
#include "Bureaucrat.hpp"
#include <fstream>
#include <iostream>
#include <ctime>

RobotomyRequestForm::RobotomyRequestForm()
    : AForm("RobotomyRequestForm", 72, 45, "default") {
    std::cout << "RobotomyRequestForm default constructor called" << std::endl;
}

RobotomyRequestForm::RobotomyRequestForm(const std::string &target)
    : AForm("RobotomyRequestForm", 72, 45, target) {
    std::cout << "RobotomyRequestForm parameterized constructor called" << std::endl;
}

RobotomyRequestForm::RobotomyRequestForm(const RobotomyRequestForm &src)
    : AForm(src) {
    std::cout << "RobotomyRequestForm copy constructor called" << std::endl;
}

RobotomyRequestForm &RobotomyRequestForm::operator=(const RobotomyRequestForm &rhs) {
    std::cout << "RobotomyRequestForm copy assignment operator called" << std::endl;
    if (this != &rhs) {
        AForm::operator=(rhs);
    }
    return *this;
}

RobotomyRequestForm::~RobotomyRequestForm() {
    std::cout << "RobotomyRequestForm destructor called" << std::endl;
}

void RobotomyRequestForm::execute(Bureaucrat const &executor) const {
	this->checkExecutionRequirements(executor);
	
	std::cout << " * DRILLING NOISES *"<< std::endl;

	// Use random number generation for 50% success rate
	// Use current time to determine success (odd vs even seconds)
	std::time_t now = std::time(NULL);
	bool success = (now % 2 == 0); // Even seconds = success, odd seconds = failure
	
	if (success) {
		std::cout << this->getTarget() << " has been robotomized successfully!" << std::endl;
	} else {
		std::cout << "Robotomy failed on " << this->getTarget() << "." << std::endl;
	}
}
