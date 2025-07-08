#include "Span.hpp"
#include <limits>
#include <iostream>

Span::Span() : _maxSize(0) {}

Span::Span(unsigned int N) : _maxSize(N)
{
	try {
		_numbers.reserve(N); //Attempts to prealloc memory
		/*The std::length_error exception is thrown if the requested capacity n exceeds the maximum size that the std::vector can hold, as determined by vector::max_size().
		In C++98, max_size() is implementation-defined but typically reflects the maximum addressable memory for the vector’s element type 
			(e.g., std::numeric_limits<size_t>::max() / sizeof(T) or similar).
		If n <= max_size() but the system cannot allocate the requested memory 
			(e.g., due to insufficient memory), reserve will typically throw std::bad_alloc 
		(or return nullptr if using a nothrow allocator, though this is less common in C++98). */
	}
	catch (std::exception &e)
	{
	// 	// length_error is C++98, and it throws when vector reaches max_size()
	// 	// So basically the exceptions they asked for already exist but yk,
	// 	// 42 wants SpanFullException so we just skip more usefull things
		std::cout << "If size fails you get -> " << e.what() << std::endl;
		// It just doesnt reach this point cuz new fails at reserve and valgrind cannot throw lmao
	}
}

Span::Span(const Span& other) : _maxSize(other._maxSize), _numbers(other._numbers) {}

Span& Span::operator=(const Span& other)
{
	if (this != &other)
	{
		_maxSize = other._maxSize;
		_numbers = other._numbers;
	}
	return *this;
}

Span::~Span() {}

void Span::addNumber(int number)
{
	if (_numbers.size() >= _maxSize)
		throw SpanFullException();
	_numbers.push_back(number);
}

unsigned int Span::shortestSpan()
{
	if (_numbers.size() < 2)
		throw NoSpanException();

	std::vector<int> sorted = _numbers; 
	std::sort(sorted.begin(), sorted.end());

	unsigned int minSpan = std::numeric_limits<unsigned int>::max();
	for (size_t i = 1; i < sorted.size(); ++i)
	{
		unsigned int span = static_cast<unsigned int>(sorted[i] - sorted[i - 1]);
		if (span < minSpan)
			minSpan = span;
	}
	return minSpan;
}

unsigned int Span::longestSpan()
{
	if (_numbers.size() < 2)
		throw NoSpanException();

	std::vector<int>::iterator minIt = std::min_element(_numbers.begin(), _numbers.end());
	std::vector<int>::iterator maxIt = std::max_element(_numbers.begin(), _numbers.end());

	return static_cast<unsigned int>(*maxIt - *minIt);
}

const char* Span::SpanFullException::what() const throw()
{
	return "Span is already full, cannot add more numbers";
}

const char* Span::NoSpanException::what() const throw()
{
	return "Cannot calculate span with less than 2 numbers";
}

unsigned int Span::getSize() const
{
	return _numbers.size();
}

unsigned int Span::getMaxSize() const
{
	return _maxSize;
}
