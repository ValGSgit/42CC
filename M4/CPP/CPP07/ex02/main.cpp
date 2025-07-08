#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include "Array.hpp"

#define MAX_VAL 750

int main() {

    Array<int> numbers(MAX_VAL);
    int* mirror = new int[MAX_VAL];
    
    srand(time(NULL));
    for (int i = 0; i < MAX_VAL; i++) {
        const int value = rand();
        numbers[i] = value;
        mirror[i] = value;
    }
    
    Array<int> tmp = numbers;
    
    Array<int> test;
    test = numbers;
    
    Array<std::string> x(2);
    std::string *stuff = new std::string[2];
    for (int i = 0; i < 2; i++){
        x[i] = stuff[i];
    }
    try {
        std::cout << x[26] << std::endl;
    }
    catch (const std::exception &e) {
        std::cout << "something went wrong " << e.what() << std::endl;
    }
    

    for (int i = 0; i < MAX_VAL; i++) {
        if (mirror[i] != numbers[i] || mirror[i] != tmp[i] || mirror[i] != test[i]) {
            std::cerr << "didn't save the same value!!" << std::endl;
            return 1;
        }
    }
    try {
        numbers[-2] = 0;
    } catch(const std::exception& e) {
        std::cerr << "Exception caught for negative index: " << e.what() << std::endl;
    }
    
    try {
        numbers[MAX_VAL] = 0;
    } catch(const std::exception& e) {
        std::cerr << "Exception caught for out-of-bounds index: " << e.what() << std::endl;
    }
    
    for (int i = 0; i < MAX_VAL; i++) {
        numbers[i] = rand();
    }

    Array<int> empty;
    std::cout << "Empty array size: " << empty.size() << std::endl;
    
    Array<std::string> strings(5);
    strings[0] = "Hello";
    strings[1] = "World";
    strings[2] = "C++";
    strings[3] = "Templates";
    strings[4] = "Array";
    
    std::cout << "String array contents:" << std::endl;
    for (unsigned int i = 0; i < strings.size(); i++) {
        std::cout << strings[i] << " ";
    }
    std::cout << std::endl;
    
    delete[] mirror;
    return 0;
}
