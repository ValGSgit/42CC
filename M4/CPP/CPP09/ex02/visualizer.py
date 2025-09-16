#!/usr/bin/env python3
# filepath: pmergeme_visualizer.py

import random
import argparse
import time
import sys

class PmergeMeVisualizer:
    def __init__(self, numbers, delay=0.5, colored=True):
        self.numbers = numbers.copy()
        self.delay = delay
        self.colored = colored
        self.step_count = 0
        
        # ANSI color codes
        self.RESET = "\033[0m"
        self.BOLD = "\033[1m"
        self.RED = "\033[91m"
        self.GREEN = "\033[92m"
        self.YELLOW = "\033[93m"
        self.BLUE = "\033[94m"
        self.MAGENTA = "\033[95m"
        self.CYAN = "\033[96m"
    
    def print_step(self, title, arr, highlights=None, pairs=None):
        """Print a visualization step with optional highlights and pair indicators"""
        self.step_count += 1
        print(f"\nStep {self.step_count}: {title}")
        
        # Default empty highlights/pairs if none provided
        if highlights is None:
            highlights = {}
        if pairs is None:
            pairs = []
        
        # Print the array elements with appropriate formatting
        output = []
        for i, num in enumerate(arr):
            item = str(num).rjust(3)
            
            # Apply color/formatting based on highlights
            if i in highlights:
                color = highlights[i]
                if self.colored:
                    item = f"{color}{item}{self.RESET}"
                else:
                    item = f"*{item}*"
            
            # Add pair indicators
            for pair in pairs:
                if i in pair:
                    pair_idx = pair.index(i)
                    if pair_idx == 0:
                        item = f"[{item}"
                    else:
                        item = f"{item}]"
            
            output.append(item)
        
        print(" ".join(output))
        time.sleep(self.delay)
    
    def generate_jacobsthal(self, n):
        """Generate Jacobsthal sequence up to n"""
        jacobsthal = [0, 1]
        while jacobsthal[-1] < n:
            jacobsthal.append(jacobsthal[-1] + 2 * jacobsthal[-2])
        return jacobsthal
    
    def binary_search(self, arr, val, left, right):
        """Visualize binary search for insertion point"""
        original_left, original_right = left, right
        
        # Show initial search range
        highlights = {i: self.YELLOW for i in range(left, right)}
        highlights[left] = self.GREEN
        highlights[right-1] = self.RED if right > 0 and right-1 < len(arr) else self.GREEN
        self.print_step(f"Binary search for {val} between indices {left}—{right-1}", 
                        arr, highlights)
        
        # Perform binary search with visualization
        while left < right:
            mid = left + (right - left) // 2
            
            # Highlight comparison
            highlights = {i: self.YELLOW for i in range(original_left, original_right)}
            highlights[mid] = self.MAGENTA
            self.print_step(f"Compare {val} with arr[{mid}]={arr[mid]}", 
                            arr, highlights)
            
            if arr[mid] < val:
                left = mid + 1
                # Highlight new range
                highlights = {i: self.YELLOW for i in range(left, right)}
                self.print_step(f"{arr[mid]} < {val}, new range: {left}—{right-1}", 
                                arr, highlights)
            else:
                right = mid
                # Highlight new range
                highlights = {i: self.YELLOW for i in range(left, right)}
                self.print_step(f"{arr[mid]} >= {val}, new range: {left}—{right-1}", 
                                arr, highlights)
        
        return left
    
    def ford_johnson_sort(self, arr):
        """Visualize Ford-Johnson sort (merge-insertion sort)"""
        if len(arr) <= 1:
            return arr
        
        # Step 1: Show original array
        self.print_step("Original array", arr)
        
        # Step 2: Create pairs
        pairs = []
        pair_indices = []
        straggler = None
        
        for i in range(0, len(arr) - 1, 2):
            pairs.append((max(arr[i], arr[i+1]), min(arr[i], arr[i+1])))
            pair_indices.append((i, i+1))
        
        # Handle straggler if odd length
        if len(arr) % 2 == 1:
            straggler = arr[-1]
            
        # Visualize pairing
        display_arr = arr.copy()
        pair_highlights = {}
        for i, (idx1, idx2) in enumerate(pair_indices):
            pair_highlights[idx1] = self.BLUE
            pair_highlights[idx2] = self.CYAN
        self.print_step("Pairing elements", display_arr, pair_highlights, pair_indices)
        
        # Step 3: Extract larger elements for recursive sort
        larger_elements = [p[0] for p in pairs]
        self.print_step("Larger elements from each pair", larger_elements)
        
        # Step 4: Recursively sort larger elements
        if len(larger_elements) > 1:
            self.print_step("Recursively sorting larger elements...", larger_elements)
            larger_elements = self.ford_johnson_sort(larger_elements)
            self.print_step("Recursively sorted larger elements", larger_elements)
        
        # Step 5: Create main chain with first smaller element and all larger elements
        main_chain = []
        if pairs:
            # Find the smaller element paired with the smallest larger element
            for i, l in enumerate(larger_elements):
                for j, (large, small) in enumerate(pairs):
                    if large == l:
                        if i == 0:  # First pair
                            main_chain.append(small)  # Add first smaller element
                        break
            
            # Add all larger elements
            main_chain.extend(larger_elements)
            self.print_step("Main chain with first smaller element and all larger elements", 
                           main_chain)
        
        # Step 6: Collect pending elements (remaining smaller elements)
        pend_elements = []
        for i, l in enumerate(larger_elements):
            if i > 0:  # Skip the first pair's smaller element (already in main chain)
                for large, small in pairs:
                    if large == l:
                        pend_elements.append(small)
                        break
        
        # Add straggler if exists
        if straggler is not None:
            pend_elements.append(straggler)
            self.print_step("Pending elements (remaining smaller elements + straggler)", 
                           pend_elements)
        else:
            self.print_step("Pending elements (remaining smaller elements)", 
                           pend_elements)
        
        # Step 7: Insert pend elements using Jacobsthal order
        if pend_elements:
            # Generate Jacobsthal-based insertion order
            jacobsthal = self.generate_jacobsthal(len(pend_elements))
            insertion_order = []
            
            # Build insertion order based on Jacobsthal sequence
            self.print_step(f"Jacobsthal sequence: {jacobsthal}", [])
            
            for j in range(2, len(jacobsthal)):
                current = jacobsthal[j]
                previous = jacobsthal[j-1]
                
                # Add indices in decreasing order within this Jacobsthal range
                for k in range(min(current, len(pend_elements)+1), previous, -1):
                    if k - 2 < len(pend_elements):
                        insertion_order.append(k - 2)
            
            # Add any remaining indices
            for i in range(len(pend_elements)):
                if i not in insertion_order:
                    insertion_order.append(i)
            
            self.print_step(f"Insertion order: {insertion_order}", [])
            
            # Insert elements in the computed order
            for i, idx in enumerate(insertion_order):
                if idx < len(pend_elements):
                    element = pend_elements[idx]
                    
                    # Visualize what we're inserting
                    highlights = {j: self.GREEN for j in range(len(main_chain))}
                    self.print_step(f"Inserting element {element} (pending element {idx+1})", 
                                   main_chain, highlights)
                    
                    # Find insertion point using binary search
                    pos = self.binary_search(main_chain, element, 0, len(main_chain))
                    
                    # Visualize insertion
                    main_chain.insert(pos, element)
                    highlights = {pos: self.MAGENTA}
                    self.print_step(f"Inserted {element} at position {pos}", 
                                   main_chain, highlights)
        
        return main_chain
    
    def visualize(self):
        """Run the visualization"""
        print(f"\n{self.BOLD}Ford-Johnson Merge-Insertion Sort Visualization{self.RESET}")
        print(f"Input sequence: {self.numbers}")
        
        sorted_arr = self.ford_johnson_sort(self.numbers)
        
        print(f"\n{self.BOLD}Final sorted result:{self.RESET}")
        print(sorted_arr)
        print(f"\nVisualization completed in {self.step_count} steps")

def generate_numbers(count, min_val=1, max_val=100, unique=True):
    """Generate random numbers for sorting"""
    if unique:
        if count > max_val - min_val + 1:
            raise ValueError("Cannot generate that many unique numbers in the given range")
        return random.sample(range(min_val, max_val + 1), count)
    else:
        return [random.randint(min_val, max_val) for _ in range(count)]

def main():
    parser = argparse.ArgumentParser(description='Visualize Ford-Johnson sort algorithm (PmergeMe)')
    parser.add_argument('count', type=int, help='Number of elements to sort')
    parser.add_argument('--min', type=int, default=1, help='Minimum value (default: 1)')
    parser.add_argument('--max', type=int, default=100, help='Maximum value (default: 100)')
    parser.add_argument('--delay', type=float, default=0.5, help='Delay between steps in seconds (default: 0.5)')
    parser.add_argument('--no-color', action='store_true', help='Disable colored output')
    parser.add_argument('--non-unique', action='store_true', help='Allow duplicate values')
    parser.add_argument('--manual', type=str, help='Manually specify numbers (comma-separated)')
    
    args = parser.parse_args()
    
    try:
        if args.manual:
            try:
                numbers = [int(x.strip()) for x in args.manual.split(',')]
                if len(numbers) != args.count:
                    print(f"Warning: {len(numbers)} numbers provided, but count is {args.count}")
                    args.count = len(numbers)
            except ValueError:
                print("Error: Invalid manual input. Please provide comma-separated integers.")
                sys.exit(1)
        else:
            numbers = generate_numbers(args.count, args.min, args.max, not args.non_unique)
        
        visualizer = PmergeMeVisualizer(numbers, args.delay, not args.no_color)
        visualizer.visualize()
        
    except ValueError as e:
        print(f"Error: {e}")
        sys.exit(1)

if __name__ == "__main__":
    main()