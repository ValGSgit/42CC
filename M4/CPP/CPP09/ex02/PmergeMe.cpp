#include "PmergeMe.hpp"
#include <cstdlib>
#include <algorithm>

PmergeMe::PmergeMe() : comps(0), countComparisons(true) {}

PmergeMe::PmergeMe(const PmergeMe& other) : _vector(other._vector),
    _deque(other._deque), 
    comps(other.comps), countComparisons(other.countComparisons) {}

PmergeMe& PmergeMe::operator=(const PmergeMe& other) {
    if (this != &other) {
        _vector = other._vector;
        _deque = other._deque;
        comps = other.comps;
        countComparisons = other.countComparisons;
    }
    return *this;
}

PmergeMe::~PmergeMe() {}

void PmergeMe::setCountComparisons(bool count) {
    countComparisons = count;
}

bool PmergeMe::isValidInteger(const std::string& str) {
    if (str.empty() || (str.length() > 1 && str[0] == '0')) {
        return false;
    }
    
    for (size_t i = 0; i < str.length(); ++i) {
        if (!std::isdigit(str[i])) return false;
    }
    
    std::stringstream ss(str);
    long num;
    ss >> num;
    return !ss.fail() && ss.eof() && num >= 0 && num <= 2147483647;
}

void PmergeMe::parseInput(int argc, char** argv) {
    if (argc < 2) {
        throw std::runtime_error("Error: No arguments provided");
    }
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (!isValidInteger(arg)) {
            throw std::runtime_error("Error: Invalid argument '" + arg + "'");
        }
        
        int num = atoi(arg.c_str());
        _vector.push_back(num);
        _deque.push_back(num);
    }
    
    if (_vector.empty()) {
        throw std::runtime_error("Error: No valid integers provided");
    }
}

double PmergeMe::getElapsedTime(struct timeval start, struct timeval end) {
    return (end.tv_sec - start.tv_sec) * 1000000.0 + (end.tv_usec - start.tv_usec);
}

void PmergeMe::process(int argc, char** argv) {
    try {
        parseInput(argc, argv);
        
        printContainer(_vector, "Before: ");
        
        struct timeval start, end;
        
        std::vector<int> vectorCopy = _vector;
        gettimeofday(&start, NULL);
        fordJohnsonSortVector(vectorCopy);
        gettimeofday(&end, NULL);
        double vectorTime = getElapsedTime(start, end);
        int vectorComps = comps;
        
        std::deque<int> dequeCopy = _deque;
        comps = 0;
        gettimeofday(&start, NULL);
        fordJohnsonSortDeque(dequeCopy);
        gettimeofday(&end, NULL);
        double dequeTime = getElapsedTime(start, end);
        int dequeComps = comps;
        
        printContainer(vectorCopy, "After:  ");
        printContainer(dequeCopy, "After deq:  ");
        
        if (!isSorted(dequeCopy)) {
            throw std::runtime_error("Error: Sorting failed for deque");
        }
        if (!isSorted(vectorCopy)) {
            throw std::runtime_error("Error: Sorting failed for vector");
        }
        
        if (countComparisons) {
            std::cout << "comparisons with std::vector: " << vectorComps << std::endl;
            std::cout << "comparisons with std::deque:  " << dequeComps << std::endl;
        } else {
            std::cout << "Comparison counting disabled" << std::endl;
        }
        std::cout << "Time to process a range of " << _vector.size() 
                  << " elements with std::vector : " << std::fixed << std::setprecision(5) 
                  << vectorTime << " us" << std::endl;
        std::cout << "Time to process a range of " << _deque.size() 
                  << " elements with std::deque  : " << std::fixed << std::setprecision(5) 
                  << dequeTime << " us" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}
