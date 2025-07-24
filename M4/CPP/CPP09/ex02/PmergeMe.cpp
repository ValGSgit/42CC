#include "PmergeMe.hpp"

PmergeMe::PmergeMe() : _comps(0) {}

PmergeMe &PmergeMe::operator=(const PmergeMe &other) {
    if (this != &other) {
        _vectorContainer = other._vectorContainer;
        _dequeContainer = other._dequeContainer;
        _comps = other._comps;
    }
    return *this;
}

PmergeMe::PmergeMe(const PmergeMe& other) {
    _vectorContainer = other._vectorContainer;
    _dequeContainer = other._dequeContainer;
    _comps = other._comps;
}

PmergeMe::~PmergeMe() {}

bool PmergeMe::isValidInteger(const std::string& str) {
    if (str.empty()) {
        return false;
    }
    if (str.length() > 1 && str[0] == '0') {
        return false;
    }
    for (size_t i = 0; i < str.length(); ++i) {
        if (!std::isdigit(str[i])) {
            return false;
        }
    }
    std::stringstream ss(str);
    long num;
    ss >> num;
    if (ss.fail() || !ss.eof() || num < 0 || num > 2147483647) {
        return false;
    }
    return true;
}

void PmergeMe::parseArguments(int argc, char** argv) {
    if (argc < 2) {
        throw std::runtime_error("Error: No arguments provided");
    }
    
    for (int i = 1; i < argc; i++) {
        std::string arg(argv[i]);
        
        if (!isValidInteger(arg)) {
            throw std::runtime_error("Error: Invalid argument");
        }
        
        std::stringstream ss(arg);
        int num;
        ss >> num;
        
        _vectorContainer.push_back(num);
        _dequeContainer.push_back(num);
    }
    
    if (_vectorContainer.empty()) {
        throw std::runtime_error("Error: No valid integers provided");
    }
}

void PmergeMe::fordJohnsonSortVect(std::vector<int>& container) {
    if (container.size() <= 1) {
        return;
    }
    
    // Step 1: Create groups of 2 elements each (initially)
    std::vector<int> ungrouped;
    std::vector<std::vector<int> > groups;
    createGroupsHelper(container, groups, ungrouped);
    
    // Step 2: Recursively sort groups using progressive grouping
    recursiveGroupSort(groups);
    
    // Step 3: Build main chain and extract smaller elements
    std::vector<int> mainChain, smallerElements;
    buildMainChain(groups, mainChain, smallerElements);
    
    // Step 4: Insert smaller elements using Jacobsthal sequence
    insertElementsWithJacobsthalVec(mainChain, smallerElements);
    
    // Step 5: Insert ungrouped elements if they exist
    for (size_t i = 0; i < ungrouped.size(); i++) {
        insertUnpairedElement(mainChain, ungrouped[i]);
    }
    
    container = mainChain;
}

void PmergeMe::fordJohnsonSortDeque(std::deque<int>& container) {
    if (container.size() <= 1) {
        return;
    }
    
    // Step 1: Create groups of 2 elements each (initially)
    std::deque<int> ungrouped;
    std::deque<std::deque<int> > groups;
    createGroupsHelper(container, groups, ungrouped);
    
    // Step 2: Recursively sort groups using progressive grouping
    recursiveGroupSort(groups);
    
    // Step 3: Build main chain and extract smaller elements
    std::deque<int> mainChain, smallerElements;
    buildMainChain(groups, mainChain, smallerElements);
    
    // Step 4: Insert smaller elements using Jacobsthal sequence
    insertElementsWithJacobsthalDeque(mainChain, smallerElements);
    
    // Step 5: Insert ungrouped elements if they exist
    for (size_t i = 0; i < ungrouped.size(); i++) {
        insertUnpairedElement(mainChain, ungrouped[i]);
    }
    
    container = mainChain;
}

double PmergeMe::getTimeInMs(struct timeval start, struct timeval end) {
    return (end.tv_sec - start.tv_sec) * 1000000.0 + (end.tv_usec - start.tv_usec);
}

void PmergeMe::processAndSort(int argc, char** argv) {
    try {
        parseArguments(argc, argv);
        printContainer(_vectorContainer, "Before: ");
        
        _comps = 0;
        struct timeval start, end;
        gettimeofday(&start, NULL);
        fordJohnsonSortVect(_vectorContainer);
        // printContainer(_vectorContainer, "After: ");
        std::cout << "Total vector comparisons: " << _comps << std::endl;
        gettimeofday(&end, NULL);
        double vectorTime = getTimeInMs(start, end);


        _comps = 0;
        gettimeofday(&start, NULL);
        // printContainer(_dequeContainer, "Before: ");
        fordJohnsonSortDeque(_dequeContainer);
        std::cout << "Total deque comparisons: " << _comps << std::endl;
        gettimeofday(&end, NULL);
        double dequeTime = getTimeInMs(start, end);
        
        // printContainer(_dequeContainer, "After: ");
        printContainer(_vectorContainer, "After: ");
        
        
        std::cout << std::fixed << std::setprecision(5);
        std::cout << "Time to process a range of " << _vectorContainer.size() 
            << " elements with std::vector : " << vectorTime << " us" << std::endl;
        std::cout << "Time to process a range of " << _dequeContainer.size() 
            << " elements with std::deque : " << dequeTime << " us" << std::endl;
        if (isSorted(_vectorContainer))
            std::cout << "Vector sorted successfully" << std::endl;
        if (isSorted(_dequeContainer))
            std::cout << "Deque sorted successfully" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}
