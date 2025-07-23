#ifndef PMERGEME_HPP
#define PMERGEME_HPP

#include <vector>
#include <deque>
#include <iostream>
#include <string>
#include <sstream>
#include <ctime>
#include <sys/time.h>
#include <iomanip>
#include <cctype>
#include <algorithm>

class PmergeMe {
	private:
		std::vector<int> _vectorContainer;
		std::deque<int> _dequeContainer;
		int 	numofnums;
		int	_comps;

		bool isValidInteger(const std::string& str);
		void parseArguments(int argc, char** argv);
		
		void fordJohnsonSortVect(std::vector<int>& container);
		void fordJohnsonSortDeque(std::deque<int>& container);
    
		template<typename Container>
		void createPairsHelper(const Container& input, std::vector<std::pair<int, int> >& pairs, int& unpaired, bool& hasUnpaired);
		
		template<typename Container>
		void createPairsHelper(const Container& input, std::deque<std::pair<int, int> >& pairs, int& unpaired, bool& hasUnpaired);
		
		template<typename PairContainer>
		void recursivePairSortVect(PairContainer& pairs);

        template<typename PairContainer>
        void recursivePairSortDeque(PairContainer& pairs);
		
		template<typename Container, typename PairContainer>
		void buildMainChain(const PairContainer& pairs, Container& mainChain, Container& smallerElements);
		
		template<typename Container>
		void insertElementsWithJacobsthal(Container& mainChain, const Container& smallerElements);
		
		template<typename Container>
		void insertUnpairedElement(Container& mainChain, int unpaired);
		
		// Utility functions
		template<typename Container>
		size_t binarySearchInsert(const Container& mainChain, int value, size_t maxPos);
		
		template<typename Container>
		void printContainer(const Container& container, const std::string& prefix);
		
		double getTimeInMs(struct timeval start, struct timeval end);
		
	public:
		PmergeMe();
		PmergeMe(const PmergeMe& other);
		PmergeMe& operator=(const PmergeMe& other);
		~PmergeMe();
		
		template<typename Container>
		bool isSorted(const Container& container);
		
		template<typename Container>
		Container generateJacobsthalSequence(size_t n);
		
		void processAndSort(int argc, char** argv);
};

template<typename Container>
bool PmergeMe::isSorted(const Container& container) {
	if (container.size() <= 1) {
		return true;
	}
	
	for (size_t i = 1; i < container.size(); ++i) {
		if (container[i - 1] > container[i]) {
			return false;
		}
	}
	
	return true;
}

template<typename Container>
Container PmergeMe::generateJacobsthalSequence(size_t n) {
    Container seq;
    if (n == 0)
        return (seq.push_back(0), seq);
    
    seq.push_back(1);
    if (n == 1)
        return (seq);
    seq.push_back(3);
    size_t i = 2;
    while (true) {
        size_t next = seq[i-1] + 2 * seq[i-2];
        if (next > n)
            break;
        seq.push_back(next);
        i++;
    }
    return seq;
}

template<typename Container>
void PmergeMe::createPairsHelper(const Container& input, std::vector<std::pair<int, int> >& pairs, int& unpaired, bool& hasUnpaired) {
    pairs.clear();
    hasUnpaired = false;
    
    for (size_t i = 0; i + 1 < input.size(); i += 2) {
        _comps++;
        if (input[i] > input[i + 1]) {
            pairs.push_back(std::make_pair(input[i], input[i + 1]));
        } else {
            pairs.push_back(std::make_pair(input[i + 1], input[i]));
        }
    }
    
    if (input.size() % 2 == 1) {
        unpaired = input.back();
        hasUnpaired = true;
    }
}

template<typename Container>
void PmergeMe::createPairsHelper(const Container& input, std::deque<std::pair<int, int> >& pairs, int& unpaired, bool& hasUnpaired) {
    pairs.clear();
    hasUnpaired = false;
    
    for (size_t i = 0; i + 1 < input.size(); i += 2) {
        _comps++;
        if (input[i] > input[i + 1]) {
            pairs.push_back(std::make_pair(input[i], input[i + 1]));
        } else {
            pairs.push_back(std::make_pair(input[i + 1], input[i]));
        }
    }
    
    if (input.size() % 2 == 1) {
        unpaired = input.back();
        hasUnpaired = true;
    }
}

template<typename PairContainer>
void PmergeMe::recursivePairSortVect(PairContainer& pairs) {
    if (pairs.size() <= 1) {
        return;
    }
    
    // Extract the larger elements for recursive sorting - use vector for simplicity
    std::vector<int> largerElements;
    for (size_t i = 0; i < pairs.size(); i++) {
        largerElements.push_back(pairs[i].first);
    }
    
    // Recursively sort the larger elements if there are more than 1
    // Create a temporary PmergeMe object to avoid recursion issues
    if (largerElements.size() > 1) {
        PmergeMe tempSorter;
        tempSorter._comps = 0;
        tempSorter.fordJohnsonSortVect(largerElements);
        // Add comparisons from recursive call
        _comps += tempSorter._comps;
    }
    
    // Rebuild pairs according to the sorted order of larger elements
    PairContainer sortedPairs;
    std::vector<bool> used(pairs.size(), false);
    
    for (size_t i = 0; i < largerElements.size(); i++) {
        for (size_t j = 0; j < pairs.size(); j++) {
            if (!used[j] && pairs[j].first == largerElements[i]) {
                sortedPairs.push_back(pairs[j]);
                used[j] = true;
                break;
            }
        }
    }
    
    pairs = sortedPairs;
}

template<typename PairContainer>
void PmergeMe::recursivePairSortDeque(PairContainer& pairs) {
    if (pairs.size() <= 1) {
        return;
    }
    
    // Extract the larger elements for recursive sorting - use vector for simplicity
    std::deque<int> largerElements;
    for (size_t i = 0; i < pairs.size(); i++) {
        largerElements.push_back(pairs[i].first);
    }
    
    // Recursively sort the larger elements if there are more than 1
    // Create a temporary PmergeMe object to avoid recursion issues
    if (largerElements.size() > 1) {
        PmergeMe tempSorter;
        tempSorter._comps = 0;
        tempSorter.fordJohnsonSortDeque(largerElements);
        // Add comparisons from recursive call
        _comps += tempSorter._comps;
    }
    
    // Rebuild pairs according to the sorted order of larger elements
    PairContainer sortedPairs;
    std::deque<bool> used(pairs.size(), false);
    
    for (size_t i = 0; i < largerElements.size(); i++) {
        for (size_t j = 0; j < pairs.size(); j++) {
            if (!used[j] && pairs[j].first == largerElements[i]) {
                sortedPairs.push_back(pairs[j]);
                used[j] = true;
                break;
            }
        }
    }
    
    pairs = sortedPairs;
}

template<typename Container, typename PairContainer>
void PmergeMe::buildMainChain(const PairContainer& pairs, Container& mainChain, Container& smallerElements) {
    mainChain.clear();
    smallerElements.clear();
    
    if (pairs.empty()) {
        return;
    }
    
    // First smaller element goes at the beginning
    mainChain.push_back(pairs[0].second);
    
    // Add all larger elements to main chain
    for (size_t i = 0; i < pairs.size(); i++) {
        mainChain.push_back(pairs[i].first);
    }
    
    // Collect smaller elements (skip the first one)
    for (size_t i = 1; i < pairs.size(); i++) {
        smallerElements.push_back(pairs[i].second);
    }
}

template<typename Container>
void PmergeMe::insertElementsWithJacobsthal(Container& mainChain, const Container& smallerElements) {
    if (smallerElements.empty()) {
        return;
    }
    
    Container jacobsthal = generateJacobsthalSequence<Container>(smallerElements.size());
    Container inserted(smallerElements.size(), false);
    
    for (size_t j = 0; j < jacobsthal.size(); j++) {
        size_t jacobsthalIndex = jacobsthal[j] - 1;
        
        size_t startIdx = 0;
        if (j > 0) {
            startIdx = jacobsthal[j-1];
        }
        
        for (size_t idx = jacobsthalIndex; idx >= startIdx && idx < smallerElements.size(); idx--) {
            if (!inserted[idx]) {
                int valueToInsert = smallerElements[idx];
                size_t maxSearchPos = idx + 1 + mainChain.size() - smallerElements.size();
                size_t insertPos = binarySearchInsert(mainChain, valueToInsert, maxSearchPos);
                mainChain.insert(mainChain.begin() + insertPos, valueToInsert);
                inserted[idx] = true;
            }
            if (idx == 0) break;
        }
    }
    
    for (size_t i = 0; i < smallerElements.size(); i++) {
        if (!inserted[i]) {
            int valueToInsert = smallerElements[i];
            size_t insertPos = binarySearchInsert(mainChain, valueToInsert, mainChain.size());
            mainChain.insert(mainChain.begin() + insertPos, valueToInsert);
        }
    }
}

template<typename Container>
void PmergeMe::insertUnpairedElement(Container& mainChain, int unpaired) {
    size_t insertPos = binarySearchInsert(mainChain, unpaired, mainChain.size());
    mainChain.insert(mainChain.begin() + insertPos, unpaired);
}

template<typename Container>
void PmergeMe::printContainer(const Container& container, const std::string& prefix) {
    std::cout << prefix;
    for (size_t i = 0; i < container.size(); ++i) {
        if (i > 0) 
            std::cout << " ";
        std::cout << container[i];
        
        if (i >= 12 && container.size() > 15) {
            std::cout << " [...]";
            break;
        }
    }
    std::cout << std::endl;
}

template<typename Container>
size_t PmergeMe::binarySearchInsert(const Container& mainChain, int value, size_t maxPos) {
    size_t left = 0;
    size_t right = std::min(maxPos, mainChain.size()); 
    
    while (left < right) {
        _comps++;
        size_t mid = left + (right - left) / 2;
        
        if (mainChain[mid] < value) {
            left = mid + 1;
        } else {
            right = mid;
        }
    }
    return left;
}

#endif
