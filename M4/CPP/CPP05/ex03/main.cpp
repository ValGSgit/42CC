#include "Bureaucrat.hpp"
#include "AForm.hpp"
#include "ShrubberyCreationForm.hpp"
#include "RobotomyRequestForm.hpp"
#include "PresidentialPardonForm.hpp"
#include "Intern.hpp"
#include <iostream>

int main() {
    try {
        std::cout << "==== Creating Intern ====" << std::endl;
        Intern someRandomIntern;
        
        std::cout << "\n==== Example from the subject ====" << std::endl;
        {
            Intern someRandomIntern;
            AForm* rrf;
            rrf = someRandomIntern.makeForm("robotomy request", "Bender");
            Bureaucrat boss("Boss", 1);
            boss.signForm(*rrf);
            boss.executeForm(*rrf);
            delete rrf;
        }
        
        std::cout << "\n==== Testing Shrubbery Creation Form ====" << std::endl;
        AForm* scf = someRandomIntern.makeForm("shrubbery creation", "garden");
        Bureaucrat gardener("Gardener", 137);
        gardener.signForm(*scf);
        gardener.executeForm(*scf);
        delete scf;
        
        std::cout << "\n==== Testing Robotomy Request Form ====" << std::endl;
        AForm* rrf = someRandomIntern.makeForm("robotomy request", "Bender");
        Bureaucrat technician("Technician", 45);
        technician.signForm(*rrf);
        technician.executeForm(*rrf);
        delete rrf;
        
        std::cout << "\n==== Testing Presidential Pardon Form ====" << std::endl;
        AForm* ppf = someRandomIntern.makeForm("presidential pardon", "Criminal");
        Bureaucrat president("President", 5);
        president.signForm(*ppf);
        president.executeForm(*ppf);
        delete ppf;
        
        std::cout << "\n==== Testing Invalid Form ====" << std::endl;
        // AForm* invalidForm = someRandomIntern.makeForm("invalid form", "target");
        // delete invalidForm;
		AForm* nlf = someRandomIntern.makeForm("Nuclear Launch Form", "Iran");
        Bureaucrat troomp("Trump", 137);
        troomp.signForm(*nlf);
        troomp.executeForm(*nlf);
        delete nlf;
        
    } catch (std::exception &e) {
        std::cout << "Exception caught: " << e.what() << std::endl;
    }
    
    return 0;
}
