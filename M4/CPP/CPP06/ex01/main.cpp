#include "Serializer.hpp"
#include "Data.hpp"
#include <iostream>
#include <iomanip>

int main() {
	
    Data originalData;
    originalData.id = 42;
    originalData.name = "Hello, World!";
    originalData.value = 3.14159;
    std::cout << "Original Data:" << std::endl;
    std::cout << "Address: " << &originalData << std::endl;
    std::cout << "ID: " << originalData.id << std::endl;
    std::cout << "Name: " << originalData.name << std::endl;
    std::cout << "Value: " << originalData.value << std::endl;
    
    uintptr_t serialized = Serializer::serialize(&originalData);
    std::cout << "\nSerialized value: " << serialized << " (0x" 
              << std::hex << serialized << std::dec << ")" << std::endl;
    
    Data* deserialized = Serializer::deserialize(serialized);
    
    std::cout << "\nDeserialized Data:" << std::endl;
    std::cout << "Address: " << deserialized << std::endl;
    std::cout << "ID: " << deserialized->id << std::endl;
    std::cout << "Name: " << deserialized->name << std::endl;
    std::cout << "Value: " << deserialized->value << std::endl;
    
    std::cout << "\nPointer comparison:" << std::endl;
    std::cout << "Original: " << &originalData << std::endl;
    std::cout << "Deserialized: " << deserialized << std::endl;
    std::cout << "Are they equal? " << (&originalData == deserialized ? "YES" : "NO") << std::endl;
    
    return 0;
}
