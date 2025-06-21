#include "Bureaucrat.hpp"
#include "Form.hpp"
#include <iostream>

int main() {
    try {
        std::cout << "--- Test case 1: Creating forms with valid grades ---" << std::endl;
        Form f1("Tax Form", 100, 50);
        Form DFLForm;
        // Form goFail("Exception form", -29, 169); //Will catch exception here and skip to the catch
        Form f2("Secret Document", 10, 5);
        
        std::cout << f1 << std::endl;
        std::cout << f2 << std::endl;
        std::cout << DFLForm << std::endl;
        
        std::cout << "\n--- Test case 2: Creating bureaucrats ---" << std::endl;
        Bureaucrat Shrek("Shrek", 120);
        Bureaucrat Donkeh("Donkeh", 8);
        Bureaucrat Dfolt;
        
        std::cout << Shrek << std::endl;
        std::cout << Donkeh << std::endl;
        std::cout << Dfolt << std::endl;
        
        std::cout << "\n--- Test case 3: Signing forms ---" << std::endl;
        Shrek.signForm(f1);
        Shrek.signForm(f2);
        Donkeh.signForm(f1);
        Donkeh.signForm(f2);

        Dfolt.signForm(DFLForm);
        Dfolt.signForm(DFLForm);
        
        std::cout << "\n--- Test case 4: Invalid form creation ---" << std::endl;
        try {
            Form invalid1("Invalid Form", 0, 50); // Too high grade to sign
        } catch (std::exception &e) {
            std::cout << "Exception caught: " << e.what() << std::endl;
        }
        
        try {
            Form invalid2("Invalid Form", 50, 151); // Too low grade to execute
        } catch (std::exception &e) {
            std::cout << "Exception caught: " << e.what() << std::endl;
        }
        
    } catch (std::exception &e) {
        std::cout << "Unexpected exception: " << e.what() << std::endl;
    }
    
    return 0;
}
