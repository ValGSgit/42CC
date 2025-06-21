#include "AForm.hpp"
#include "Bureaucrat.hpp"
#include "PresidentialPardonForm.hpp"
#include "RobotomyRequestForm.hpp"
#include "ShrubberyCreationForm.hpp"

int main() {

	// class NuclearLaunchForm : public AForm("Nuclear Launch form", 69, 12, "default");
	std::cout << "-----------------Test 1. Everything at once-----------------" << std::endl;
	Bureaucrat *Macron = new Bureaucrat("Macron", 138);
	Bureaucrat Trump("Trump", 1);
	PresidentialPardonForm ppForm("Criminal");
	RobotomyRequestForm rrForm("Bender");
	ShrubberyCreationForm scForm("Garden");

	Trump.signForm(ppForm);
	Trump.signForm(rrForm);
	Trump.signForm(scForm);
	Trump.executeForm(ppForm);
	Trump.executeForm(rrForm);
	Trump.executeForm(scForm);

	Macron->signForm(ppForm);
	Macron->signForm(rrForm);
	Macron->signForm(scForm);
	Macron->executeForm(ppForm);
	Macron->executeForm(rrForm);
	Macron->executeForm(scForm);

	delete Macron;

	std::cout << "-----------------Test 2. Multiple of each -----------------" << std::endl;

	RobotomyRequestForm rr1Form("Target1");
	RobotomyRequestForm rr2Form("Target2");
	RobotomyRequestForm rr3Form("Target3");
	RobotomyRequestForm rr4Form("Target4");
	RobotomyRequestForm rr5Form("Target5");
	RobotomyRequestForm rr6Form("Target6");

	Trump.signForm(rr1Form);
	Trump.signForm(rr2Form);
	Trump.signForm(rr3Form);
	Trump.signForm(rr4Form);
	Trump.signForm(rr5Form);
	Trump.signForm(rr6Form);
	Trump.executeForm(rr1Form);
	Trump.executeForm(rr2Form);
	Trump.executeForm(rr3Form);
	Trump.executeForm(rr4Form);
	Trump.executeForm(rr5Form);
	Trump.executeForm(rr6Form);

}