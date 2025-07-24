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
#include <climits>

class PmergeMe {
	private:
		std::vector<int> _vectorContainer;
		std::deque<int> _dequeContainer;
		int	_comps;

		bool isValidInteger(const std::string& str);
		void parseArguments(int argc, char** argv);
		
		void fordJohnsonSortVect(std::vector<int>& container);
		void fordJohnsonSortDeque(std::deque<int>& container);
    
		template<typename Cunt>
		void createGroupsHelper(const Cunt& input, std::vector<std::vector<int> >& groups, Cunt& ungrouped);
		
		template<typename Cunt>
		void createGroupsHelper(const Cunt& input, std::deque<std::deque<int> >& groups, Cunt& ungrouped);
		
        template<typename GroupCunt>
        void recursiveGroupSort(GroupCunt& groups);

		template<typename GroupCunt>
		void mergeGroups(GroupCunt& groups, size_t groupSize);
		
		template<typename InnerCunt>
		void sortSingleGroup(InnerCunt& group);
		
		template<typename Cunt, typename GroupCunt>
		void buildMainChain(const GroupCunt& groups, Cunt& mainChain, Cunt& smallerElements);
		
		template<typename Cunt>
		void insertElementsWithJacobsthalVec(Cunt& mainChain, const Cunt& smallerElements);
		
        template<typename Cunt>
		void insertElementsWithJacobsthalDeque(Cunt& mainChain, const Cunt& smallerElements);
		
		template<typename Cunt>
		void insertUnpairedElement(Cunt& mainChain, int unpaired);
		
		template<typename Cunt>
		size_t binarySearchInsert(const Cunt& mainChain, int value, size_t maxPos);
		
		template<typename Cunt>
		void printContainer(const Cunt& container, const std::string& prefix);
		
		double getTimeInMs(struct timeval start, struct timeval end);
		
	public:
		PmergeMe();
		PmergeMe(const PmergeMe& other);
		PmergeMe& operator=(const PmergeMe& other);
		~PmergeMe();
		
		template<typename Cunt>
		bool isSorted(const Cunt& container);
		
		template<typename Cunt>
		Cunt generateJacobsthalSequence(size_t n);
		
		void processAndSort(int argc, char** argv);
};

template<typename Cunt>
bool PmergeMe::isSorted(const Cunt& container) {
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

template<typename Cunt>
Cunt PmergeMe::generateJacobsthalSequence(size_t n) {
    Cunt seq;
    if (n == 0)
        return (seq.push_back(0), seq);
    
    seq.push_back(1);
    if (n == 1)
        return (seq);
    seq.push_back(3);
    size_t i = 2;
    while (true) {
        size_t prev1 = seq[i-1];
        size_t prev2 = seq[i-2];
        
        // Check if 2 * prev2 would overflow or if addition would overflow
        const size_t max_val = static_cast<size_t>(-1); //Wraparound to get max u_long
        if (prev2 > max_val / 2 || prev1 > max_val - 2 * prev2) {
            break;
        }
        
        size_t next = prev1 + 2 * prev2;
        if (next > n)
            break;
        seq.push_back(next);
        i++;
    }
    return seq;
}

// ------------------- Two different templates to make helper container be of same time --------------
template<typename Cunt>
void PmergeMe::createGroupsHelper(const Cunt& input, std::vector<std::vector<int> >& groups, Cunt& ungrouped) {
    groups.clear();
    ungrouped.clear();
    
    for (size_t i = 0; i + 1 < input.size(); i += 2) {
        std::vector<int> group;
        _comps++;
        if (input[i] > input[i + 1]) {
            group.push_back(input[i]);
            group.push_back(input[i + 1]);
        } else {
            group.push_back(input[i + 1]);
            group.push_back(input[i]);
        }
        groups.push_back(group);
    }
    
    if (input.size() % 2 == 1) {
        ungrouped.push_back(input.back());
    }
}

template<typename Cunt>
void PmergeMe::createGroupsHelper(const Cunt& input, std::deque<std::deque<int> >& groups, Cunt& ungrouped) {
    groups.clear();
    ungrouped.clear();
    
    for (size_t i = 0; i + 1 < input.size(); i += 2) {
        std::deque<int> group;
        _comps++;
        if (input[i] > input[i + 1]) {
            group.push_back(input[i]);
            group.push_back(input[i + 1]);
        } else {
            group.push_back(input[i + 1]);
            group.push_back(input[i]);
        }
        groups.push_back(group);
    }
    
    if (input.size() % 2 == 1) {
        ungrouped.push_back(input.back());
    }
}
// -----------------------------------------------------------------------------------------------


// ----------VVV--------------- The Crown Jewel -----------------VVV------------------------------

/**
 * @brief Recursively sorts groups using the Ford-Johnson merge-insertion algorithm
 * 
 * This function implements the recursive part of the Ford-Johnson algorithm by:
 * 1. Extracting the largest element from each group (these represent the groups)
 * 2. Recursively applying Ford-Johnson sorting to these larger elements
 * 3. Rebuilding the main chain from the sorted larger elements
 * 4. Inserting any ungrouped elements into their proper positions
 * 5. Reorganizing the original groups based on the sorted order of their largest elements
 * 
 * The algorithm works for each group's largest element,
 * sorting these recursively, and then rearranging the original groups
 * according to this sorted order.
 * 
 * @tparam Container type that holds groups of elements
 * @param groups Reference to the container of groups to be sorted in-place
 * 
 */
template<typename GroupCunt>
void PmergeMe::recursiveGroupSort(GroupCunt& groups)
{
    if (groups.size() <= 1) {
        return;
    }
    for (size_t i = 1; i < groups.size(); i++) {
        typename GroupCunt::value_type key = groups[i];
        size_t j = i;
        while (j > 0 && !groups[j - 1].empty() && !key.empty() && groups[j - 1][0] > key[0])
        {
            // _comps++;
            groups[j] = groups[j - 1];
            j--;
        }
        if (j > 0 && !groups[j - 1].empty() && !key.empty()) {
            _comps++; // Only count comparison if we actually made one to exit the loop
        }
        groups[j] = key;
    }
}

//-----------------------------------------------------------------------------------------------


template<typename GroupCunt>
void PmergeMe::mergeGroups(GroupCunt& groups, size_t currentSize) {
    if (currentSize >= groups.size()) {
        return;
    }
    
    size_t n = groups.size();
    
    // Current merge size is from currentSize and doubles each iteration
    for (size_t mergeSize = currentSize; mergeSize < n; mergeSize *= 2)
    {
        for (size_t leftStart = 0; leftStart < n; leftStart += 2 * mergeSize)
        {
            size_t mid = leftStart + mergeSize < n ? leftStart + mergeSize : n;
            size_t rightEnd = leftStart + 2 * mergeSize < n ? leftStart + 2 * mergeSize : n;
            
            if (mid >= rightEnd)
                continue;
            
            // Merge groups[leftStart...mid-1] with groups[mid...rightEnd-1]
            GroupCunt temp;
            size_t i = leftStart, j = mid;
            
            while (i < mid && j < rightEnd)
            {
                _comps++;
                if (!groups[i].empty() && !groups[j].empty() && 
                    groups[i][0] <= groups[j][0])
                {
                    temp.push_back(groups[i]);
                    i++;
                }
                else
                {
                    temp.push_back(groups[j]);
                    j++;
                }
            }
            while (i < mid) {
                temp.push_back(groups[i]);
                i++;
            }
            while (j < rightEnd) {
                temp.push_back(groups[j]);
                j++;
            }
            for (size_t k = 0; k < temp.size(); k++) {
                groups[leftStart + k] = temp[k];
            }
        }
    }
}

template<typename InnerCunt>
void PmergeMe::sortSingleGroup(InnerCunt& group) {
    if (group.size() <= 1) {
        return;
    }
    
    for (size_t i = 1; i < group.size(); i++)
    {
        int key = group[i];
        size_t j = i;
        while (j > 0 && group[j - 1] > key)
        {
            _comps++;
            group[j] = group[j - 1];
            j--;
        }
        // Only count comparison if we actually made one to exit the loop
        if (j > 0)
            _comps++;
        group[j] = key;
    }
}

template<typename Cunt, typename GroupCunt>
void PmergeMe::buildMainChain(const GroupCunt& groups, Cunt& mainChain, Cunt& smallerElements) {
    mainChain.clear();
    smallerElements.clear();
    
    if (groups.empty()) {
        return;
    }

    // main chain: add the smaller element from first pair
    if (!groups[0].empty() && groups[0].size() > 1) {
        mainChain.push_back(groups[0][1]);
    }
    
    // Then add all the larger elements
    for (size_t i = 0; i < groups.size(); i++)
    {
        if (!groups[i].empty())
        {
            mainChain.push_back(groups[i][0]); // larger element are already sorted so we just push em
        }
    }
    
    // Save smaller elements for insert
    for (size_t i = 1; i < groups.size(); i++)
    {
        if (!groups[i].empty() && groups[i].size() > 1)
        {
            smallerElements.push_back(groups[i][1]);
        }
    }
}

template<typename Cunt>
void PmergeMe::insertElementsWithJacobsthalDeque(Cunt& mainChain, const Cunt& smallerElements) {
    if (smallerElements.empty()) {
        return;
    }
    
    // Generate Jacobsthal sequence
    Cunt jacobsthal = generateJacobsthalSequence<Cunt>(smallerElements.size());
    
    // Track which elements have been inserted
    std::deque<bool> inserted(smallerElements.size(), false);
    
    // Process elements using Jacobsthal sequence order
    for (size_t j = 0; j < jacobsthal.size(); j++) {
        size_t jacobIdx = static_cast<size_t>(jacobsthal[j]);
        
        if (jacobIdx > 0 && jacobIdx <= smallerElements.size()) {
            size_t index = jacobIdx - 1; // Convert to 0-based
            
            if (index < smallerElements.size() && !inserted[index]) {
                // Insert element using binary search on the entire main chain
                size_t insertPos = binarySearchInsert(mainChain, smallerElements[index], mainChain.size());
                mainChain.insert(mainChain.begin() + insertPos, smallerElements[index]);
                inserted[index] = true;
            }
            
            // Insert elements in reverse order between previous and current Jacobsthal
            size_t prevJacobIdx;
            if (j > 0) {
                prevJacobIdx = static_cast<size_t>(jacobsthal[j-1]);
            } else {
                prevJacobIdx = 1;
            }
            
            // Insert in descending order from current-1 down to prevJacobIdx
            for (long long i = static_cast<long long>(jacobIdx) - 2; i >= static_cast<long long>(prevJacobIdx) - 1; i--) {
                if (i < 0) break;
                
                size_t idx = static_cast<size_t>(i);
                if (idx < smallerElements.size() && !inserted[idx]) {
                    size_t insertPos = binarySearchInsert(mainChain, smallerElements[idx], mainChain.size());
                    mainChain.insert(mainChain.begin() + insertPos, smallerElements[idx]);
                    inserted[idx] = true;
                }
            }
        }
    }
    
    for (size_t i = 0; i < smallerElements.size(); i++) {
        if (!inserted[i]) {
            insertUnpairedElement(mainChain, smallerElements[i]);
            // size_t insertPos = binarySearchInsert(mainChain, smallerElements[i], mainChain.size());
            // mainChain.insert(mainChain.begin() + insertPos, smallerElements[i]);
        }
    }
}

template<typename Cunt>
void PmergeMe::insertElementsWithJacobsthalVec(Cunt& mainChain, const Cunt& smallerElements) {
    if (smallerElements.empty()) {
        return;
    }
    
    // Generate Jacobsthal sequence
    Cunt jacobsthal = generateJacobsthalSequence<Cunt>(smallerElements.size());
    
    // Track which elements have been inserted
    std::vector<bool> inserted(smallerElements.size(), false);
    
    // Process elements using Jacobsthal sequence order
    for (size_t j = 0; j < jacobsthal.size(); j++) {
        size_t jacobIdx = static_cast<size_t>(jacobsthal[j]);
        
        if (jacobIdx > 0 && jacobIdx <= smallerElements.size()) {
            size_t index = jacobIdx - 1; // Convert to 0-based
            
            if (index < smallerElements.size() && !inserted[index]) {
                // Insert element using binary search on the entire main chain
                size_t insertPos = binarySearchInsert(mainChain, smallerElements[index], mainChain.size());
                mainChain.insert(mainChain.begin() + insertPos, smallerElements[index]);
                inserted[index] = true;
            }
            
            // Insert elements in reverse order between previous and current Jacobsthal
            size_t prevJacobIdx;
            if (j > 0) {
                prevJacobIdx = static_cast<size_t>(jacobsthal[j-1]);
            } else {
                prevJacobIdx = 1;
            }
            
            // Insert in descending order from current-1 down to prevJacobIdx
            for (long long i = static_cast<long long>(jacobIdx) - 2; i >= static_cast<long long>(prevJacobIdx) - 1; i--) {
                if (i < 0) break;
                
                size_t idx = static_cast<size_t>(i);
                if (idx < smallerElements.size() && !inserted[idx]) {
                    size_t insertPos = binarySearchInsert(mainChain, smallerElements[idx], mainChain.size());
                    mainChain.insert(mainChain.begin() + insertPos, smallerElements[idx]);
                    inserted[idx] = true;
                }
            }
        }
    }
    
    for (size_t i = 0; i < smallerElements.size(); i++) {
        if (!inserted[i]) {
            insertUnpairedElement(mainChain, smallerElements[i]);
            // size_t insertPos = binarySearchInsert(mainChain, smallerElements[i], mainChain.size());
            // mainChain.insert(mainChain.begin() + insertPos, smallerElements[i]);
        }
    }
}

//Mini helper to call in a loop
template<typename Cunt>
void PmergeMe::insertUnpairedElement(Cunt& mainChain, int unpaired) {
    size_t insertPos = binarySearchInsert(mainChain, unpaired, mainChain.size());
    mainChain.insert(mainChain.begin() + insertPos, unpaired);
}

template<typename Cunt>
void PmergeMe::printContainer(const Cunt& container, const std::string& prefix) {
    std::cout << prefix;
    for (size_t i = 0; i < container.size(); ++i) {
        if (i > 0) 
            std::cout << " ";
        std::cout << container[i];
        
        // if (i >= 12 && container.size() > 15) {
        //     std::cout << " [...]";
        //     break;
        // } //Unquote if tested with a lot of numbers
    }
    std::cout << std::endl;
}

template<typename Cunt>
size_t PmergeMe::binarySearchInsert(const Cunt& mainChain, int value, size_t maxPos) {
    size_t left = 0;
    size_t right = 0;
    
    if (maxPos < mainChain.size())
        right = maxPos;
    else
        right = mainChain.size();
    
    if (right == 0) {
        return 0;
    }
    if (right == 1) {
        _comps++;
        if (value <= mainChain[0]) {
            return 0;
        } else {
            return 1;
        }
    }
    while (left < right) {
        size_t mid = left + (right - left) / 2;
        
        _comps++;
        if (mainChain[mid] < value) {
            left = mid + 1;
        } else {
            right = mid;
        }
    }
    return left;
}

#endif
