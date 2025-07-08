#ifndef SPAN_HPP
#define SPAN_HPP

#include <vector>
#include <exception>
#include <algorithm>

class Span
{
	private:
		unsigned int _maxSize;
		std::vector<int> _numbers;

	public:
		//OCF
		Span();
		Span(unsigned int N);
		Span(const Span& other);
		Span& operator=(const Span& other);
		~Span();

		void addNumber(int number);
		unsigned int shortestSpan();
		unsigned int longestSpan();

		template<typename Iterator>
		void addNumbers(Iterator begin, Iterator end);

		class SpanFullException : public std::exception
		{
		public:
			virtual const char* what() const throw();
		};

		class NoSpanException : public std::exception
		{
		public:
			virtual const char* what() const throw();
		};

		unsigned int getSize() const;
		unsigned int getMaxSize() const;
};

template<typename Iterator>
void Span::addNumbers(Iterator begin, Iterator end)
{
	while (begin != end)
	{
		addNumber(*begin);
		++begin;
	}
}

#endif
