#include "PmergeMe.hpp"
#include <cstdlib>


std::deque<size_t> PmergeMe::generateJacobsthalOrderDe(size_t n) {
    std::deque<size_t> order;

    if (n == 0)
		return order;

    // Jacobsthal sequence starting from J0=0, J1=1
    std::deque<size_t> J;
    J.push_back(0);
    J.push_back(1);
    while (true) {
        size_t next = J.back() + 2 * J[J.size() - 2];
        J.push_back(next);
        if (next > n + 1)
			break;  // n+1 is max original b-index (b1 pre-inserted)
    }

    size_t prev = 1;  // After pre-inserted b1
    for (size_t t = 2; t < J.size(); ++t) {  // Start from t=2 (J2=1), but effective groups from t=3
        size_t curr = J[t];
        size_t hi = std::min(curr, n + 1);
        for (size_t k = hi; k > prev; --k) {
            size_t pendIdx = k - 2;
            order.push_back(pendIdx);
        }
        prev = hi;
    }

    return order;
}

void PmergeMe::fordJohnsonSortDeque(std::deque<int>& container) {
    if (container.size() <= 1)
		return;
    
    // Step 1: Create pairs and store the Loner if odd number
    std::deque<std::pair<int, int> > pairs;
    int Loner = -1;
    bool hasLoner = (container.size() % 2 == 1);
    
    for (size_t i = 0; i + 1 < container.size(); i += 2) {
        if (countComparisons) comps++;
        if (container[i] > container[i + 1]) {
            pairs.push_back(std::make_pair(container[i], container[i + 1])); // (larger, smaller)
        } else {
            pairs.push_back(std::make_pair(container[i + 1], container[i])); // (larger, smaller)
        }
    }
    
    if (hasLoner) {
        Loner = container.back();
    }
    
    // Step 2: Recursively sort the larger elements (first elements of pairs)
    std::deque<int> largerElements;
    for (size_t i = 0; i < pairs.size(); ++i) {
        largerElements.push_back(pairs[i].first);
    }
    
    fordJohnsonSortDeque(largerElements);
    
    // Step 3: Reorder pairs based on sorted larger elements using optimized approach
    std::deque<std::pair<int, int> > sortedPairs;
    
    for (size_t i = 0; i < largerElements.size(); i++) {
        for (size_t j = 0; j < pairs.size(); j++) {
            if (pairs[j].first == largerElements[i] && pairs[j].first != -1) {
                sortedPairs.push_back(pairs[j]);
                pairs[j].first = -1;
                break;
            }
        }
    }
	
    
    // Step 4: Create main chain starting with first smaller element and all larger elements
    std::deque<int> mainChain;
    std::deque<int> pendElements;
    
    if (!sortedPairs.empty()) {
        mainChain.push_back(sortedPairs[0].second);
        
        for (size_t i = 0; i < sortedPairs.size(); ++i) {
            mainChain.push_back(sortedPairs[i].first);
			if (i > 0)
				pendElements.push_back(sortedPairs[i].second);
        }
    }
    
	if (hasLoner) {
		pendElements.push_back(Loner);
	}

    std::deque<size_t> largerPositions(sortedPairs.size());
    for (size_t i = 0; i < sortedPairs.size(); i++) {
        largerPositions[i] = i + 1; // +1 because mainChain starts with first smaller element
    }
	
    if (!pendElements.empty()) {
        std::deque<size_t> insertionOrder = generateJacobsthalOrderDe(pendElements.size());
        
        for (size_t i = 0; i < insertionOrder.size(); ++i) {
            size_t pendIndex = insertionOrder[i];
            if (pendIndex >= pendElements.size())
				continue;
            
            int elementToInsert = pendElements[pendIndex];

            //  pendIndex + 1 is the position of the winner paired with this pend element
            size_t upper = mainChain.size();
			if (pendIndex + 1 < largerPositions.size())
				upper = largerPositions[pendIndex + 1];
            
            size_t pos = binarySearchDeque(mainChain, elementToInsert, 0, upper);
            mainChain.insert(mainChain.begin() + pos, elementToInsert);
            
            for (size_t j = 0; j < largerPositions.size(); j++) {
                if (largerPositions[j] >= pos) {
                    largerPositions[j]++;
                }
            }
        }
    }
    
    container = mainChain;
}

size_t PmergeMe::binarySearchDeque(const std::deque<int>& chain, int value, size_t left, size_t right) {
    if (right < left)
		return left;

    size_t l = left;
	size_t r = right;
    while (l < r) {
        size_t mid = l + (r - l) / 2;
        if (countComparisons) comps++;
        if (chain[mid] < value) {
            l = mid + 1;
        } else {
            r = mid;
        }
    }
    return l;
}
