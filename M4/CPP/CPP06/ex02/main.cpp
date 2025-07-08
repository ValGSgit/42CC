#include "Base.hpp"
#include "A.hpp"
#include "B.hpp"
#include "C.hpp"
#include <iostream>
#include <cstdlib>
#include <ctime>

Base* generate(void) {

    static bool seeded = false;
    if (!seeded) {
        std::srand(static_cast<unsigned int>(std::time(NULL)));
        seeded = true;
    }
    
    int randomNum = std::rand() % 3;
    switch (randomNum) {
        case 0:
            std::cout << "Generated class A" << std::endl;
            return new A();
        case 1:
            std::cout << "Generated class B" << std::endl;
            return new B();
        case 2:
            std::cout << "Generated class C" << std::endl;
            return new C();
        default:
            return NULL; // Should never happen
    }
}

void identify(Base* p) {
    if (dynamic_cast<A*>(p) != NULL) {
        std::cout << "A" << std::endl;
    } else if (dynamic_cast<B*>(p) != NULL) {
        std::cout << "B" << std::endl;
    } else if (dynamic_cast<C*>(p) != NULL) {
        std::cout << "C" << std::endl;
    } else {
        std::cout << "Unknown type" << std::endl;
    }
}

// Function to identify the type using a reference
// Subject says dont use pointers inside this function
void identify(Base& p) {
    try {
        (void)dynamic_cast<A&>(p);
        std::cout << "A" << std::endl;
        return;
    } catch (...) {
    }
    
    try {
        (void)dynamic_cast<B&>(p);
        std::cout << "B" << std::endl;
        return;
    } catch (...) {
    }
    
    try {
        (void)dynamic_cast<C&>(p);
        std::cout << "C" << std::endl;
        return;
    } catch (...) {
        std::cout << "Unknown type" << std::endl;
    }
}


int main() {
    std::cout << "Running type identification tests..." << std::endl;
    
    for (int i = 0; i < 5; ++i) {
        std::cout << "\nTest #" << i + 1 << ":" << std::endl;
        Base* instance = generate();
        
        std::cout << "Identifying with pointer method: ";
        identify(instance);
        
        std::cout << "Identifying with reference method: ";
        identify(*instance);

        delete instance;
    }
    
    return 0;
}
