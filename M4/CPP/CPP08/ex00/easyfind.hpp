#ifndef EASYFIND_HPP
#define EASYFIND_HPP

#include <algorithm>
#include <exception>

class NotFoundException : public std::exception
{
	public:
		virtual const char* what() const throw()
		{
			return "Element not found in container";
		}
};

// Better version but the subject asks to shittify it
// template<typename T, typename U>
// typename T::iterator easyfind(T& container, const U& value)
// {
// 	typename T::iterator it = std::find(container.begin(), container.end(), value);
// 	if (it == container.end())
// 		throw NotFoundException();
// 	return it;
// }

template<typename T>
typename T::iterator easyfind(T& container, int value)
{
	typename T::iterator it = std::find(container.begin(), container.end(), value);
	if (it == container.end())
		throw NotFoundException();
	return it;
}

#endif
