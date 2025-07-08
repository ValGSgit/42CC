#include <iostream>
#include <string>
#include "iter.hpp"

template <typename T>
void printElement(const T& element) {
    std::cout << element << " ";
}

template <typename T>
void incrementElement(T& element) {
    element++;
}

int main(void) {
    int intArray[] = {1, 2, 3, 4, 5};
    size_t intLength = 5;
    
    std::cout << "Original int array: ";
    iter(intArray, intLength, printElement<int>);
    std::cout << std::endl;
    
    iter(intArray, intLength, incrementElement<int>);
    
    std::cout << "Modified int array: ";
    iter(intArray, intLength, printElement<int>);
    std::cout << std::endl;
    
    std::string strArray[] = {"Hello", "World", "C++", "Templates"};
    size_t strLength = 4;
    
    std::cout << "String array: ";
    iter(strArray, strLength, printElement<std::string>);
    std::cout << std::endl;
    
    const char* constArray[] = {"Const", "Array", "Test"};
    size_t constLength = 3;
    
    std::cout << "Const array: ";
    iter(constArray, constLength, printElement<const char*>);
    std::cout << std::endl;
    
    return 0;
}
