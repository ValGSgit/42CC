#include "Span.hpp"
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>

int main()
{
	std::cout << "=== Subject Test ===" << std::endl;
	{
		Span sp = Span(5);
		sp.addNumber(6);
		sp.addNumber(3);
		sp.addNumber(17);
		sp.addNumber(9);
		sp.addNumber(11);
		std::cout << sp.shortestSpan() << std::endl;
		std::cout << sp.longestSpan() << std::endl;
	}

	std::cout << "\n=== Exception Tests ===" << std::endl;
	{
		// Test SpanFullException
		try
		{
			Span sp(2);
			sp.addNumber(1);
			sp.addNumber(2);
			sp.addNumber(3); // Should throw
		}
		catch (const std::exception& e)
		{
			std::cout << "First exception caught: " << e.what() << std::endl;
		}
		try
		{
			Span sp(5); //Can hold 5 but no numbers in there
			sp.shortestSpan(); // Should throw
		}
		catch (const std::exception& e)
		{
			std::cout << "Second exception caught: " << e.what() << std::endl;
		}
		try
		{
			Span sp(5);
			sp.addNumber(42);
			sp.longestSpan(); // Should throw
		}
		catch (const std::exception& e)
		{
			std::cout << "Third exception caught: " << e.what() << std::endl;
		}
		try
		{
			Span sp;
			sp.addNumber(42);
			sp.longestSpan(); // Should throw
		}
		catch (const std::exception& e)
		{
			std::cout << "Fourth exception caught: " << e.what() << std::endl;
		}
	}

	std::cout << "\n=== Large Test (10000+ numbers) ===" << std::endl;
	{
		const unsigned int size = 15000;
		Span sp(size);
		
		std::srand(std::time(NULL));
		for (unsigned int i = 0; i < size; ++i)
		{
			sp.addNumber(std::rand() % 100000);
		}
		
		std::cout << "Added " << sp.getSize() << " numbers to span" << std::endl;
		std::cout << "Shortest span: " << sp.shortestSpan() << std::endl;
		std::cout << "Longest span: " << sp.longestSpan() << std::endl;
	}

	std::cout << "\n=== Range Iterator Test ===" << std::endl;
	{
		std::vector<int> numbers;
		for (int i = 0; i < 1000; ++i)
		{
			numbers.push_back(i * 2);
		}

		Span sp(1000);
		sp.addNumbers(numbers.begin(), numbers.end());
		
		std::cout << "Added " << sp.getSize() << " numbers using range iterators" << std::endl;
		std::cout << "Shortest span: " << sp.shortestSpan() << std::endl;
		std::cout << "Longest span: " << sp.longestSpan() << std::endl;
	}

	std::cout << "\n=== Copy and Assignment Test ===" << std::endl;
	{
		Span sp1(5);
		sp1.addNumber(1);
		sp1.addNumber(10);
		sp1.addNumber(100);

		Span sp2(sp1);
		Span sp3(10);
		sp3 = sp1;

		std::cout << "Original span shortest: " << sp1.shortestSpan() << std::endl;
		std::cout << "Copied span shortest: " << sp2.shortestSpan() << std::endl;
		std::cout << "Assigned span shortest: " << sp3.shortestSpan() << std::endl;
	}
	// std::cout << "\n=== Array of Spans Test ===" << std::endl;
	// {
	// 	std::vector<Span> spans;
		
	// 	std::srand(46290214);
	// 	for (size_t i = 0; i < 69; i++) {
	// 		spans.push_back(Span(69));
	// 		for (size_t j = 0; j < 69; j++) {
	// 			int value = std::rand();
	// 			spans[i].addNumber(value);
	// 		}
	// 	}
	// 	for (size_t i = 0; i < 69; i++) {
	// 		std::cout << "Span " << i << " shortest: " << spans[i].shortestSpan() 
	// 		<< ", longest: " << spans[i].longestSpan() << std::endl;
	// 	}
		
	// }
	// std::cout << "\n=== Bad Alloc Test (Force Usage) ===" << std::endl;
	// {
	// 	try {
	// 		int i;
	// 		Span sp(1000000000); // 1 billion capacity
	// 		// Only try to add a reasonable number to trigger bad_alloc
	// 		for (i = 0; i < 100000000; ++i) { // 100 million instead of 1 billion
	// 			sp.addNumber(i);
	// 			// Check periodically and break if we're using too much memory
	// 			if (i % 10000000 == 0) {
	// 				std::cout << "Added " << i << " numbers so far..." << std::endl;
	// 			}
	// 		}
	// 		std::cout << "Last succesful number was " << i << std::endl;
	// 		std::cout << "Span filled successfully (unexpected)" << std::endl;
	// 	}
	// 	catch (const std::bad_alloc& e) {
	// 		std::cout << "bad_alloc caught: " << e.what() << std::endl;
	// 	}
	// 	catch (const std::exception& e) {
	// 		std::cout << "Other exception caught: " << e.what() << std::endl;
	// 	}
	// }
	return 0;
}
