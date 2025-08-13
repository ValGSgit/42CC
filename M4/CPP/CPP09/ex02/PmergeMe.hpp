#ifndef PMERGEME_HPP
#define PMERGEME_HPP

#include <vector>
#include <deque>
#include <iostream>
#include <string>
#include <sstream>
#include <sys/time.h>
#include <iomanip>

class PmergeMe {
private:
    std::vector<int> _vector;
    std::deque<int> _deque;

    int comps;
    bool countComparisons;
    
    // Validation and parsing
    bool isValidInteger(const std::string& str);
    void parseInput(int argc, char** argv);
    
    
    // Vector functions
    void fordJohnsonSortVector(std::vector<int>& container);
    size_t binarySearchVector(const std::vector<int>& chain, int value, size_t left, size_t right);
    std::vector<size_t> generateJacobsthalOrder(size_t n);
    
    // Deque functions  
    void fordJohnsonSortDeque(std::deque<int>& container);
    size_t binarySearchDeque(const std::deque<int>& chain, int value, size_t left, size_t right);
    std::deque<size_t> generateJacobsthalOrderDe(size_t n);

    double getElapsedTime(struct timeval start, struct timeval end);
    
    template<typename Container>
    void printContainer(const Container& container, const std::string& label);
    
    template<typename Container>
    bool isSorted(const Container& container);

public:
    PmergeMe();
    PmergeMe(const PmergeMe& other);
    PmergeMe& operator=(const PmergeMe& other);
    ~PmergeMe();
    
    void process(int argc, char** argv);
    void setCountComparisons(bool count = true);
};

template<typename Container>
bool PmergeMe::isSorted(const Container& container) {
    bool isSorted = true;
    for (size_t i = 0; i < container.size() - 1; i++) {
        if (container[i] > container[i + 1]) {
            isSorted = false;
            break;
        }
    }
    return isSorted;
}

template<typename Container>
void PmergeMe::printContainer(const Container& container, const std::string& label) {
    std::cout << label;
    for (size_t i = 0; i < container.size(); ++i) {
        if (i > 0) std::cout << " ";
        std::cout << container[i];
    }
    std::cout << std::endl;
}

#endif
