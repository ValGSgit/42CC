#include "Bureaucrat.hpp"
#include <iostream>

int main() {
    std::cout << "\n--- Testing normal bureaucrat creation ---" << std::endl;
    try {
        Bureaucrat normal("Normal", 75);
        std::cout << normal << std::endl;

        std::cout << "\n--- Testing grade increment ---" << std::endl;
        normal.incrementGrade();
        std::cout << "After increment: " << normal << std::endl;

        std::cout << "\n--- Testing grade decrement ---" << std::endl;
        normal.decrementGrade();
        std::cout << "After decrement: " << normal << std::endl;
    }
    catch (std::exception &e) {
        std::cout << "Exception: " << e.what() << std::endl;
    }

    std::cout << "\n--- Testing grade too high (0) ---" << std::endl;
    try {
        Bureaucrat tooHigh("TooHigh", 0);
        std::cout << tooHigh << std::endl;
    }
    catch (std::exception &e) {
        std::cout << "Exception: " << e.what() << std::endl;
    }

    std::cout << "\n--- Testing grade too low (151) ---" << std::endl;
    try {
        Bureaucrat tooLow("TooLow", 151);
        std::cout << tooLow << std::endl;
    }
    catch (std::exception &e) {
        std::cout << "Exception: " << e.what() << std::endl;
    }

    std::cout << "\n--- Testing increment beyond limit ---" << std::endl;
    try {
        Bureaucrat topGrade("TopGrade", 1);
        std::cout << topGrade << std::endl;
        std::cout << "Attempting to increment beyond grade 1..." << std::endl;
        topGrade.incrementGrade();
    }
    catch (std::exception &e) {
        std::cout << "Exception: " << e.what() << std::endl;
    }

    std::cout << "\n--- Testing decrement beyond limit ---" << std::endl;
    try {
        Bureaucrat bottomGrade("BottomGrade", 150);
        std::cout << bottomGrade << std::endl;
        std::cout << "Attempting to decrement beyond grade 150..." << std::endl;
        bottomGrade.decrementGrade();
    }
    catch (std::exception &e) {
        std::cout << "Exception: " << e.what() << std::endl;
    }

    std::cout << "\n--- Testing copy constructor and assignment operator ---" << std::endl;
    try {
        Bureaucrat original("Original", 150);
        std::cout << "Original: " << original << std::endl;
        
        Bureaucrat copy(original);
        std::cout << "Copy: " << copy << std::endl;
        
        Bureaucrat assigned;
        assigned = original;
        std::cout << "Assigned: " << assigned << std::endl;
		assigned.decrementGrade();
    }
    catch (std::exception &e) {
        std::cout << "Exception: " << e.what() << std::endl;
    }

    std::cout << "\n--- Different exception handling approaches ---" << std::endl;
    std::cout << "\n1. Using multiple catch blocks:" << std::endl;
    try {
        Bureaucrat test1("Test1", 150);
        std::cout << "Created: " << test1 << std::endl;
        test1.decrementGrade();
    }
    catch (const Bureaucrat::GradeTooHighException &e) {
        std::cout << "Caught specific high exception: " << e.what() << std::endl;
    }
    catch (const Bureaucrat::GradeTooLowException &e) {
        std::cout << "Caught specific low exception: " << e.what() << std::endl;
    }
    catch (const std::exception &e) {
        std::cout << "Caught generic exception: " << e.what() << std::endl;
    }

    std::cout << "\n2. Using exception type checking:" << std::endl;
    try {
        Bureaucrat test2("Test2", 151);
    }
    catch (const std::exception &e) {
        std::cout << "Caught exception: " << e.what() << std::endl;
        
        // Check exception type using dynamic_cast
        if (dynamic_cast<const Bureaucrat::GradeTooHighException*>(&e)) {
            std::cout << "Identified as a GradeTooHighException" << std::endl;
        }
        else if (dynamic_cast<const Bureaucrat::GradeTooLowException*>(&e)) {
            std::cout << "Identified as a GradeTooLowException" << std::endl;
        }
    }
    std::cout << "\n3. Using nested try-catch blocks:" << std::endl;
    try {
        try {
            Bureaucrat test3("Test3", 151);
        }
        catch (const Bureaucrat::GradeTooLowException &e) {
            std::cout << "Inner catch: " << e.what() << std::endl;
            // Will handle here, throw back and catch again ;)
            throw;
        }
    }
    catch (const std::exception &e) {
        std::cout << "Outer catch: " << e.what() << std::endl;
    }

	std::cout << "\n4. Using general exception check:" << std::endl;
	try {
		Bureaucrat CJ("CJ", 1);
		Bureaucrat bruh("Big Smoke", 150);
		
		try {
			CJ.incrementGrade();
		}
		catch (std::exception &e) {
			std::cout << "CJ exception caught: " << e.what() << std::endl;
		}
		try {
			bruh.decrementGrade();
		}
		catch (std::exception &e) {

			// Instead of handling this as a std::exception, throw a different type
			// std::cout << "Bruh exception: " << e.what() << std::endl;
			std::cout << "If we catch Bruh exception here it doesnt reach unknown throw" << std::endl;
			throw "Custom non-std::exception error"; // This is a const char* exception
		}
	}
	catch (std::exception &e) {
		std::cout << "Type catch: " << e.what() << std::endl;
	}
	catch (...) // Unknown throw type - will catch the const char* exception
	{
		std::cout << "You entered the wrong house fool!" << std::endl;
		std::cout << "Ayo Smoke relax, its me CJ" << std::endl;
	}

    return 0;
}
