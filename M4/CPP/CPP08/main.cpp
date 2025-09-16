#include <iostream>
#include <vector>
#include <list>
#include <deque>
#include <stack>
#include <queue>
#include <algorithm>
#include <iterator>
#include <string>
#include <cassert>
#include "ex00/easyfind.hpp"
#include "ex01/Span.hpp"
#include "ex02/MutantStack.hpp"

// Helper function to print container contents
template<typename Container>
void printContainer(const Container& container, const std::string& name) {
    std::cout << name << ": ";
    for (typename Container::const_iterator it = container.begin(); it != container.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;
}

// Helper function to print container using iterators
template<typename Iterator>
void printRange(Iterator begin, Iterator end, const std::string& name) {
    std::cout << name << ": ";
    for (Iterator it = begin; it != end; ++it) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;
}

void testBasicContainerUsage() {
    std::cout << "\n=== BASIC CONTAINER USAGE ===\n";
    
    // Vector - Dynamic array
    std::vector<int> vec;
    for (int i = 1; i <= 5; ++i) {
        vec.push_back(i * 10);
    }
    printContainer(vec, "Vector");
    std::cout << "Vector capacity: " << vec.capacity() << ", size: " << vec.size() << std::endl;
    
    // List - Doubly linked list
    std::list<int> lst;
    for (int i = 1; i <= 5; ++i) {
        lst.push_back(i * 20);
    }
    lst.push_front(0);
    printContainer(lst, "List");
    
    // Deque - Double-ended queue
    std::deque<int> deq;
    for (int i = 1; i <= 5; ++i) {
        deq.push_back(i * 30);
        deq.push_front(i * -30);
    }
    printContainer(deq, "Deque");
    
    // Stack (standard - no iterators)
    std::stack<int> stk;
    for (int i = 1; i <= 5; ++i) {
        stk.push(i * 40);
    }
    std::cout << "Stack top: " << stk.top() << ", size: " << stk.size() << std::endl;
}

void testIteratorTypes() {
    std::cout << "\n=== ITERATOR TYPES AND TRAVERSAL ===\n";
    
    std::vector<int> vec;
    for (int i = 1; i <= 6; ++i) {
        vec.push_back(i);
    }
    
    // Forward iteration
    printRange(vec.begin(), vec.end(), "Forward");
    
    // Reverse iteration
    printRange(vec.rbegin(), vec.rend(), "Reverse");
    
    // Const iteration (read-only)
    std::vector<int>::const_iterator cit = vec.begin();
    std::cout << "First element (const): " << *cit << std::endl;
    
    // Random access capabilities
    std::vector<int>::iterator it = vec.begin();
    std::cout << "Random access - element at index 3: " << *(it + 3) << std::endl;
    std::cout << "Iterator arithmetic - distance: " << (vec.end() - vec.begin()) << std::endl;
}

void testCustomImplementations() {
    std::cout << "\n=== CUSTOM IMPLEMENTATIONS ===\n";
    
    // Test easyfind
    std::vector<int> vec;
    for (int i = 1; i <= 10; ++i) {
        vec.push_back(i);
    }
    
    try {
        std::vector<int>::iterator found = easyfind(vec, 5);
        std::cout << "easyfind found: " << *found << std::endl;
    } catch (const std::exception& e) {
        std::cout << "easyfind error: " << e.what() << std::endl;
    }
    
    try {
        easyfind(vec, 99);
    } catch (const std::exception& e) {
        std::cout << "easyfind (not found): " << e.what() << std::endl;
    }
    
    // Test Span
    Span sp(10);
    for (int i = 1; i <= 5; ++i) {
        sp.addNumber(i * 10);
    }
    
    std::cout << "Span shortest span: " << sp.shortestSpan() << std::endl;
    std::cout << "Span longest span: " << sp.longestSpan() << std::endl;
    
    // Test bulk addition
    std::vector<int> bulk;
    for (int i = 60; i <= 80; i += 5) {
        bulk.push_back(i);
    }
    sp.addNumbers(bulk.begin(), bulk.end());
    std::cout << "After bulk addition - longest span: " << sp.longestSpan() << std::endl;
    
    // Test MutantStack
    MutantStack<int> mstack;
    for (int i = 1; i <= 5; ++i) {
        mstack.push(i * 100);
    }
    
    std::cout << "MutantStack top: " << mstack.top() << std::endl;
    printRange(mstack.begin(), mstack.end(), "MutantStack forward");
    printRange(mstack.rbegin(), mstack.rend(), "MutantStack reverse");
}

void testAlgorithms() {
    std::cout << "\n=== STL ALGORITHMS ===\n";
    
    std::vector<int> vec;
    for (int i = 1; i <= 10; ++i) {
        vec.push_back(i);
    }
    
    // Find
    std::vector<int>::iterator found = std::find(vec.begin(), vec.end(), 7);
    if (found != vec.end()) {
        std::cout << "Found 7 at position: " << (found - vec.begin()) << std::endl;
    }
    
    // Count
    vec.push_back(5); // Add duplicate
    int count = std::count(vec.begin(), vec.end(), 5);
    std::cout << "Count of 5: " << count << std::endl;
    
    // Transform
    std::vector<int> doubled(vec.size());
    std::transform(vec.begin(), vec.end(), doubled.begin(), 
                   std::bind2nd(std::multiplies<int>(), 2));
    printContainer(doubled, "Doubled");
    
    // Sort and search
    std::vector<int> unsorted;
    unsorted.push_back(5); unsorted.push_back(2); unsorted.push_back(8); 
    unsorted.push_back(1); unsorted.push_back(9);
    
    printContainer(unsorted, "Before sort");
    std::sort(unsorted.begin(), unsorted.end());
    printContainer(unsorted, "After sort");
    
    // Binary search (requires sorted container)
    bool exists = std::binary_search(unsorted.begin(), unsorted.end(), 5);
    std::cout << "Binary search for 5: " << (exists ? "found" : "not found") << std::endl;
}

void testCapacityAndMemory() {
    std::cout << "\n=== CAPACITY AND MEMORY MANAGEMENT ===\n";
    
    std::vector<int> vec;
    std::cout << "Initial - size: " << vec.size() << ", capacity: " << vec.capacity() << std::endl;
    
    // Growth pattern
    for (int i = 0; i < 10; ++i) {
        vec.push_back(i);
        std::cout << "After push " << i << " - size: " << vec.size() 
                  << ", capacity: " << vec.capacity() << std::endl;
    }
    
    // Reserve memory
    vec.reserve(50);
    std::cout << "After reserve(50) - size: " << vec.size() 
              << ", capacity: " << vec.capacity() << std::endl;
    
    // Resize
    vec.resize(25, 42); // Fill new elements with 42
    std::cout << "After resize(25, 42) - size: " << vec.size() 
              << ", capacity: " << vec.capacity() << std::endl;
    std::cout << "Last few elements: ";
    for (size_t i = vec.size() - 3; i < vec.size(); ++i) {
        std::cout << vec[i] << " ";
    }
    std::cout << std::endl;
}

void testIteratorInvalidation() {
    std::cout << "\n=== ITERATOR INVALIDATION (DANGEROUS) ===\n";
    
    std::cout << "Testing vector iterator invalidation...\n";
    std::vector<int> vec;
    for (int i = 1; i <= 5; ++i) {
        vec.push_back(i);
    }
    
    std::vector<int>::iterator it = vec.begin() + 2; // Points to 3
    std::cout << "Iterator points to: " << *it << std::endl;
    
    // This might invalidate the iterator due to reallocation
    std::cout << "Capacity before push_back: " << vec.capacity() << std::endl;
    vec.push_back(6);
    std::cout << "Capacity after push_back: " << vec.capacity() << std::endl;
    
    // WARNING: Using 'it' here might be undefined behavior if reallocation occurred
    std::cout << "Iterator after push_back: " << *it << " (may be invalid!)" << std::endl;
    
    // Safe approach: get new iterator
    it = vec.begin() + 2;
    std::cout << "New iterator points to: " << *it << std::endl;
    
    // List iterators are more stable
    std::cout << "\nTesting list iterator stability...\n";
    std::list<int> lst;
    for (int i = 1; i <= 5; ++i) {
        lst.push_back(i);
    }
    
    std::list<int>::iterator lit = lst.begin();
    ++lit; ++lit; // Points to third element
    std::cout << "List iterator points to: " << *lit << std::endl;
    
    lst.push_back(6);
    lst.push_front(0);
    std::cout << "List iterator after modifications: " << *lit << " (still valid)" << std::endl;
}

void testBoundaryConditions() {
    std::cout << "\n=== BOUNDARY CONDITIONS AND EDGE CASES ===\n";
    
    // Empty containers
    std::vector<int> empty_vec;
    std::cout << "Empty vector size: " << empty_vec.size() << std::endl;
    std::cout << "Empty vector begin == end: " << (empty_vec.begin() == empty_vec.end()) << std::endl;
    
    // Single element
    std::vector<int> single;
    single.push_back(42);
    std::cout << "Single element vector - begin to end distance: " 
              << (single.end() - single.begin()) << std::endl;
    
    // Test with Span edge cases
    try {
        Span tiny(1);
        tiny.addNumber(5);
        tiny.shortestSpan(); // Should throw - need at least 2 elements
    } catch (const std::exception& e) {
        std::cout << "Span with 1 element error: " << e.what() << std::endl;
    }
    
    try {
        Span zero(0);
        zero.addNumber(1); // Should throw - no space
    } catch (const std::exception& e) {
        std::cout << "Span with 0 capacity error: " << e.what() << std::endl;
    }
    
    // Stack underflow
    std::stack<int> empty_stack;
    std::cout << "Empty stack size: " << empty_stack.size() << std::endl;
    // empty_stack.top(); // Would cause undefined behavior
    // empty_stack.pop(); // Would cause undefined behavior
}

void testWeirdAndDangerousStuff() {
    std::cout << "\n=== WEIRD AND DANGEROUS STUFF ===\n";
    
    // 1. Iterator past end dereference (UNDEFINED BEHAVIOR)
    std::cout << "1. Iterator past end access:\n";
    std::vector<int> vec;
    vec.push_back(1);
    vec.push_back(2);
    // std::vector<int>::iterator end_it = vec.end();
    // std::cout << *end_it; // NEVER DO THIS - undefined behavior
    std::cout << "   Skipping actual dereference (would crash)\n";
    
    // 2. Modifying container while iterating
    std::cout << "2. Modifying during iteration (DANGEROUS):\n";
    std::vector<int> danger_vec;
    for (int i = 1; i <= 5; ++i) {
        danger_vec.push_back(i);
    }
    
    std::cout << "   Original: ";
    printContainer(danger_vec, "");
    
    // This is dangerous - modifying container during iteration
    for (std::vector<int>::iterator it = danger_vec.begin(); it != danger_vec.end(); /* no ++it */) {
        if (*it % 2 == 0) {
            it = danger_vec.erase(it); // erase returns next valid iterator
        } else {
            ++it;
        }
    }
    std::cout << "   After removing evens: ";
    printContainer(danger_vec, "");
    
    // 3. Comparing iterators from different containers
    std::cout << "3. Cross-container iterator comparison:\n";
    std::vector<int> vec1, vec2;
    vec1.push_back(1); vec2.push_back(1);
    
    std::vector<int>::iterator it1 = vec1.begin();
    std::vector<int>::iterator it2 = vec2.begin();
    
    // This comparison is undefined behavior but might "work"
    std::cout << "   Iterators equal? " << (it1 == it2) << " (undefined behavior!)\n";
    
    // 4. Using invalidated iterator
    std::cout << "4. Using invalidated iterator:\n";
    std::vector<int> inval_vec;
    inval_vec.push_back(1);
    
    // std::vector<int>::iterator old_it = inval_vec.begin();
    inval_vec.clear(); // Invalidates all iterators
    
    // old_it is now invalid - using it is undefined behavior
    std::cout << "   Iterator invalidated after clear() - don't use it!\n";
    
    // 5. Reverse iterator weirdness
    std::cout << "5. Reverse iterator base() behavior:\n";
    std::vector<int> rev_vec;
    for (int i = 1; i <= 5; ++i) {
        rev_vec.push_back(i);
    }
    
    std::vector<int>::reverse_iterator rit = rev_vec.rbegin();
    ++rit; // Now points to 4
    
    std::cout << "   Reverse iterator points to: " << *rit << std::endl;
    std::cout << "   Its base() points to: " << *rit.base() << std::endl; // Points to 5!
    
    // 6. Container adapter stack emptiness
    std::cout << "6. Stack adapter weird behavior:\n";
    std::stack<int> weird_stack;
    
    // Checking empty before operations
    if (weird_stack.empty()) {
        std::cout << "   Stack is empty, size: " << weird_stack.size() << std::endl;
        // weird_stack.top(); // Would be undefined behavior
        // weird_stack.pop(); // Would be undefined behavior
    }
    
    // 7. MutantStack exposing implementation details
    std::cout << "7. MutantStack breaking encapsulation:\n";
    MutantStack<int> mut_stack;
    mut_stack.push(10);
    mut_stack.push(20);
    mut_stack.push(30);
    
    std::cout << "   Stack should be LIFO, but we can iterate: ";
    for (MutantStack<int>::iterator it = mut_stack.begin(); it != mut_stack.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;
    
    // We can even modify elements through iterator (breaking stack semantics)
    *(mut_stack.begin()) = 999;
    std::cout << "   Modified bottom element to 999, top is still: " << mut_stack.top() << std::endl;
    
    // 8. Algorithm with wrong iterator type
    std::cout << "8. Using wrong algorithm for iterator type:\n";
    std::list<int> alg_list;
    for (int i = 1; i <= 5; ++i) {
        alg_list.push_back(i);
    }
    
    // This works but is inefficient - list has its own sort()
    std::cout << "   List before sort: ";
    printContainer(alg_list, "");
    
    alg_list.sort(); // Use list's efficient sort
    std::cout << "   List after sort: ";
    printContainer(alg_list, "");
    
    // std::sort(alg_list.begin(), alg_list.end()); // Won't compile - needs random access
}

int main() {
    std::cout << "=== COMPREHENSIVE CONTAINER TEST SUITE ===\n";
    
    try {
        testBasicContainerUsage();
        testIteratorTypes();
        testCustomImplementations();
        testAlgorithms();
        testCapacityAndMemory();
        testIteratorInvalidation();
        testBoundaryConditions();
        testWeirdAndDangerousStuff();
        
        std::cout << "\n=== ALL TESTS COMPLETED ===\n";
        std::cout << "Note: Some 'dangerous' operations were demonstrated safely.\n";
        std::cout << "In real code, avoid undefined behavior at all costs!\n";
        
    } catch (const std::exception& e) {
        std::cout << "Exception caught: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}