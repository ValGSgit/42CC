#include "PmergeMe.hpp"

PmergeMe::PmergeMe() : numofnums(0), _comps(0) {}

PmergeMe &PmergeMe::operator=(const PmergeMe &other) {
    if (this != &other) {
        _vectorContainer = other._vectorContainer;
        _dequeContainer = other._dequeContainer;
        // loneNum = other.loneNum;
        numofnums = other.numofnums;
        _comps = other._comps;
    }
    return *this;
}

PmergeMe::PmergeMe(const PmergeMe& other) {
    _vectorContainer = other._vectorContainer;
    _dequeContainer = other._dequeContainer;
    // loneNum = other.loneNum;
    numofnums = other.numofnums;
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
    
    this->numofnums = argc - 1;
}

void PmergeMe::fordJohnsonSortVect(std::vector<int>& container) {
    std::cout << "\n=== Ford-Johnson Algorithm (Vector) ===" << std::endl;
    std::cout << "Input: ";
    printContainer(container, "");
    
    // Use template specialization approach
    if (container.size() <= 1) {
        return;
    }
    
    // Step 1: Create pairs
    int unpaired;
    bool hasUnpaired;
    std::vector<std::pair<int, int> > pairs;
    createPairsHelper(container, pairs, unpaired, hasUnpaired);
    
    // Step 2: Recursively sort pairs by their larger elements
    recursivePairSortVect(pairs);
    
    // Step 3: Build main chain and extract smaller elements
    std::vector<int> mainChain, smallerElements;
    buildMainChain(pairs, mainChain, smallerElements);
    
    // Step 4: Insert smaller elements using Jacobsthal sequence
    insertElementsWithJacobsthal(mainChain, smallerElements);
    
    // Step 5: Insert unpaired element if exists
    if (hasUnpaired) {
        insertUnpairedElement(mainChain, unpaired);
    }
    
    container = mainChain;
    
    std::cout << "Final result: ";
    printContainer(container, "");
    std::cout << "Total comparisons: " << _comps << std::endl;
}

void PmergeMe::fordJohnsonSortDeque(std::deque<int>& container) {
    std::cout << "\n=== Ford-Johnson Algorithm (Deque) ===" << std::endl;
    std::cout << "Input: ";
    printContainer(container, "");
    
    // Use template specialization approach
    if (container.size() <= 1) {
        return;
    }
    
    // Step 1: Create pairs
    int unpaired;
    bool hasUnpaired;
    std::deque<std::pair<int, int> > pairs;
    createPairsHelper(container, pairs, unpaired, hasUnpaired);
    
    // Step 2: Recursively sort pairs by their larger elements
    recursivePairSortDeque(pairs);
    
    // Step 3: Build main chain and extract smaller elements
    std::deque<int> mainChain, smallerElements;
    buildMainChain(pairs, mainChain, smallerElements);
    
    // Step 4: Insert smaller elements using Jacobsthal sequence
    insertElementsWithJacobsthal(mainChain, smallerElements);
    
    // Step 5: Insert unpaired element if exists
    if (hasUnpaired) {
        insertUnpairedElement(mainChain, unpaired);
    }
    
    container = mainChain;
    
    std::cout << "Final result: ";
    printContainer(container, "");
    std::cout << "Total comparisons: " << _comps << std::endl;
}

double PmergeMe::getTimeInMs(struct timeval start, struct timeval end) {
    return (end.tv_sec - start.tv_sec) * 1000000.0 + (end.tv_usec - start.tv_usec);
}

void PmergeMe::processAndSort(int argc, char** argv) {
    try {
        parseArguments(argc, argv);
        printContainer(_vectorContainer, "Before: ");
        
        // Process with vector
        _comps = 0;
        struct timeval start, end;
        gettimeofday(&start, NULL);
        fordJohnsonSortVect(_vectorContainer);
        gettimeofday(&end, NULL);
        double vectorTime = getTimeInMs(start, end);
        
        // Process with deque
        _comps = 0;
        gettimeofday(&start, NULL);
        fordJohnsonSortDeque(_dequeContainer);
        gettimeofday(&end, NULL);
        double dequeTime = getTimeInMs(start, end);
        
        printContainer(_vectorContainer, "After: ");
        
        if (isSorted(_vectorContainer))
            std::cout << "Vector sorted successfully" << std::endl;
        if (isSorted(_dequeContainer))
            std::cout << "Deque sorted successfully" << std::endl;
            
        std::cout << std::fixed << std::setprecision(5);
        std::cout << "Time to process a range of " << _vectorContainer.size() 
                  << " elements with std::vector : " << vectorTime << " us" << std::endl;
        std::cout << "Time to process a range of " << _dequeContainer.size() 
                  << " elements with std::deque : " << dequeTime << " us" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}
