#include "PmergeMe.hpp"
#include <string>

/*
    Part 1: Pairing and Initial Sorting

    Groups elements into pairs
    Sorts each pair individually
    If there's an odd number of elements, the last element becomes unpaired


    Part 2: Recursive Sorting of Larger Elements

    Takes the larger element from each pair
    Recursively sorts this sequence using the same Ford-Johnson algorithm
    This creates the main sorted chain


    Part 3: Binary Insertion of Smaller Elements

    Uses the Jacob sequence (Jacobsthal numbers) to determine optimal insertion order
    For each smaller element from the pairs, performs binary search to find insertion point
    Inserts elements into the main chain maintaining sorted order


    Part 4. Performance Measurement
    Records execution time for both container implementations measured in microseconds

    
    Part 5. Output Display
    The program outputs:

    Before: Original unsorted sequence
    After: Final sorted sequence
    Time: Processing time for std::vector<int> container
    Time: Processing time for std::deque<int> container
*/
int main(int argc, char** argv) {
    PmergeMe sorter;
    
    bool skipCountCheck = false;
    
#ifdef COUNT_COMPARISONS
    if (!skipCountCheck) {
        sorter.setCountComparisons(true);
    }
#else
    if (!skipCountCheck) {
        sorter.setCountComparisons(false);
    }
#endif
    
    sorter.process(argc, argv);
    return 0;
}
