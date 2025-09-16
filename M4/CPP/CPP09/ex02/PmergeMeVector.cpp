#include "PmergeMe.hpp"
#include <cstdlib>

std::vector<size_t> PmergeMe::generateJacobsthalOrder(size_t n) {
    std::vector<size_t> order;

    if (n == 0)
		return order;

    // Jacobsthal sequence starting from J0=0, J1=1
    std::vector<size_t> J;
    J.push_back(0);
    J.push_back(1);
    while (true) {
        size_t next = J.back() + 2 * J[J.size() - 2];
        J.push_back(next);
        if (next > n + 1)
			break;  // n+1 is max original b-index (b1 pre-inserted)
    }

	size_t prevBound = 1;  // After pre-inserted b1
	for (size_t jacIdx = 2; jacIdx < J.size(); ++jacIdx) {  // Start from idx=2 (J2=1)
		size_t currJacobsthal = J[jacIdx];
		size_t upperBound = std::min(currJacobsthal, n + 1); //If jacobsthal value is bigger than amount of pending we just take the last pos
		for (size_t position = upperBound; position > prevBound; --position) {
			size_t pendingIndex = position - 2;
			order.push_back(pendingIndex);
		}
		prevBound = upperBound;
	}

    return order;
}


void PmergeMe::fordJohnsonSortVector(std::vector<int>& container) {
    if (container.size() <= 1)
		return;
    
    // Step 1: Create pairs and store the Loner if odd number
    std::vector<std::pair<int, int> > pairs;
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
    std::vector<int> largerElements;
    for (size_t i = 0; i < pairs.size(); ++i) {
        largerElements.push_back(pairs[i].first);
    }
    
    fordJohnsonSortVector(largerElements);
    
    // Step 3: Reorder pairs based on sorted larger elements using optimized approach
    std::vector<std::pair<int, int> > sortedPairs;
    
    // Build sortedPairs directly by finding matching pairs
    for (size_t i = 0; i < largerElements.size(); i++) {
        for (size_t j = 0; j < pairs.size(); j++) {
            if (pairs[j].first == largerElements[i] && pairs[j].first != -1) { //Not sure if i should count this
                sortedPairs.push_back(pairs[j]);
                pairs[j].first = -1; // Mark as used
                break;
            }
        }
    }

    std::vector<int> mainChain;
    std::vector<int> pendElements;
    
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

    // Pre-compute positions of larger elements in mainChain for optimization
    std::vector<size_t> largerPositions(sortedPairs.size());
    for (size_t i = 0; i < sortedPairs.size(); i++) {
        largerPositions[i] = i + 1; // +1 because mainChain starts with first smaller element
    }
    
    // Step 5: Insert pend elements using canonical Jacobsthal order with tight bounds
    if (!pendElements.empty()) {
        std::vector<size_t> insertionOrder = generateJacobsthalOrder(pendElements.size());
        
        for (size_t i = 0; i < insertionOrder.size(); ++i) {
            size_t pendIndex = insertionOrder[i];
            if (pendIndex >= pendElements.size())
				continue;
            
            int elementToInsert = pendElements[pendIndex];

            size_t upper = mainChain.size();
			if (pendIndex + 1 < largerPositions.size())
				upper = largerPositions[pendIndex + 1]; // Winner paired with this pend element
            
            size_t pos = binarySearchVector(mainChain, elementToInsert, 0, upper);
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

size_t PmergeMe::binarySearchVector(const std::vector<int>& chain, int value, size_t left, size_t right) {
    if (right < left)
		return left;
    size_t l = left, r = right;
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

