#include "PmergeMe.hpp"
#include <cstdlib>

std::vector<size_t> PmergeMe::generateJacobsthalOrder(size_t n) {
    std::vector<size_t> order;

    if (n == 0)
		return order;

    std::vector<size_t> J;
    J.push_back(0);
    J.push_back(1);
    while (true) {
        size_t next = J.back() + 2 * J[J.size() - 2]; //J(n) = J(n-1) + 2 * J(n-2)
        J.push_back(next);
        if (next > n + 1)
			break;
    }

	size_t prevBound = 1;  // After b1
	for (size_t jacIdx = 2; jacIdx < J.size(); ++jacIdx) {
		size_t currJacobsthal = J[jacIdx];
		size_t upperBound = std::min(currJacobsthal, n + 1);
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
    
    // Step 1: Create pairs and store the loner if odd number
    std::vector<std::pair<int, int> > pairs;
    int loner = -1;
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
        loner = container.back();
    }
    
    // Step 2: Recursively sort the larger elements (first elements of pairs)
    std::vector<int> largerElements;
    for (size_t i = 0; i < pairs.size(); ++i) {
        largerElements.push_back(pairs[i].first);
    }
    
    fordJohnsonSortVector(largerElements);
    
    // Step 3: Reorder pairs based on sorted larger elements
    std::vector<std::pair<int, int> > sortedPairs;
    sortedPairs.reserve(largerElements.size());

    for (size_t i = 0; i < largerElements.size(); i++) {
        int target = largerElements[i];
        
        for (size_t j = 0; j < pairs.size(); j++) {
            if (pairs[j].first != -1 && (pairs[j].first - target) == 0) {
                sortedPairs.push_back(pairs[j]);
                pairs[j].first = -1;
                break;
            }
        }
    }


    std::vector<int> mainChain;
    std::vector<int> pendElements;
    
    if (!sortedPairs.empty()) {
        mainChain.push_back(sortedPairs[0].second); //push b1 as first
        for (size_t i = 0; i < sortedPairs.size(); ++i) {
            mainChain.push_back(sortedPairs[i].first);
			if (i > 0) //put all smaller elements into pending
				pendElements.push_back(sortedPairs[i].second);
        }

    }
    
	if (hasLoner) {
		pendElements.push_back(loner); //adding the loner at the very end of pending just as the algo asks
	}

    // saving the positions of larger elements in mainChain for insertion bounds
    std::vector<size_t> largerPositions(sortedPairs.size());
    for (size_t i = 0; i < sortedPairs.size(); i++) {
        largerPositions[i] = i + 1; // +1 because mainChain starts with first smaller element
    }
    
    // Step 5: Insert pend elements using Jacobsthal order with bounds
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

size_t PmergeMe::binarySearchVector(const std::vector<int>& chain, int value, size_t lowerBound, size_t upperBound) {
    if (upperBound < lowerBound)
		return lowerBound;
    size_t left = lowerBound;
    size_t right = upperBound;
    while (left < right) {
        // could bee just (right - left) / 2 but this way i can prevent underflow
        size_t mid = left + (right - left) / 2;
        if (countComparisons) comps++;
        if (chain[mid] < value) {
            left = mid + 1;
        } else {
            right = mid;
        }
    }
    return left;
}

