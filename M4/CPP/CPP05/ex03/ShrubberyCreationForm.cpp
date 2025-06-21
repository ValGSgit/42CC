#include "ShrubberyCreationForm.hpp"
#include "Bureaucrat.hpp"
#include <fstream>
#include <iostream>

ShrubberyCreationForm::ShrubberyCreationForm()
    : AForm("ShrubberyCreationForm", 145, 137, "default") {
    std::cout << "ShrubberyCreationForm default constructor called" << std::endl;
}

ShrubberyCreationForm::ShrubberyCreationForm(const std::string &target)
    : AForm("ShrubberyCreationForm", 145, 137, target) {
    std::cout << "ShrubberyCreationForm parameterized constructor called" << std::endl;
}

ShrubberyCreationForm::ShrubberyCreationForm(const ShrubberyCreationForm &src)
    : AForm(src) {
    std::cout << "ShrubberyCreationForm copy constructor called" << std::endl;
}

ShrubberyCreationForm &ShrubberyCreationForm::operator=(const ShrubberyCreationForm &rhs) {
    std::cout << "ShrubberyCreationForm copy assignment operator called" << std::endl;
    if (this != &rhs) {
        AForm::operator=(rhs);
    }
    return *this;
}

ShrubberyCreationForm::~ShrubberyCreationForm() {
    std::cout << "ShrubberyCreationForm destructor called" << std::endl;
}

void ShrubberyCreationForm::execute(Bureaucrat const &executor) const {
    this->checkExecutionRequirements(executor);
    
    std::string filename = this->getTarget() + "_shrubbery";
    std::ofstream outfile(filename.c_str());
    
    if (!outfile) {
        std::cerr << "Error creating file: " << filename << std::endl;
        return;
    }
    
    outfile << "       _-_       " << std::endl;
    outfile << "    /~~   ~~\\    " << std::endl;
    outfile << " /~~         ~~\\ " << std::endl;
    outfile << "{               }" << std::endl;
    outfile << " \\  _-     -_  / " << std::endl;
    outfile << "   ~  \\\\ //  ~   " << std::endl;
    outfile << "_- -   | | _- _  " << std::endl;
    outfile << "  _ -  | |   -_  " << std::endl;
    outfile << "      // \\\\      " << std::endl;
    outfile << std::endl;
    
    outfile << std::endl;

    outfile << "           *" << std::endl;
    outfile << "          /|\\" << std::endl;
    outfile << "         /*|O\\" << std::endl;
    outfile << "        /*/|\\*\\" << std::endl;
    outfile << "       /X/O|*\\X\\" << std::endl;
    outfile << "      /*/X/|\\X\\*\\" << std::endl;
    outfile << "     /O/*/X|*\\O\\X\\" << std::endl;
    outfile << "    /*/O/X/|\\X\\O\\*\\" << std::endl;
    outfile << "   /X/O/*/X|O\\X\\*\\O\\" << std::endl;
    outfile << "  /O/X/*/O/|\\*\\X\\*\\X\\" << std::endl;
    outfile << "          |_|" << std::endl;
    
    outfile << std::endl;
    
    outfile.close();
    std::cout << "Created ASCII tree in file: " << filename << std::endl;
}
