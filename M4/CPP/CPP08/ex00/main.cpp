#include "easyfind.hpp"
#include <iostream>
#include <vector>
#include <list>
#include <deque>
#include <string>

int main()
{
	std::cout << "=== Testing easyfind with std::vector ===" << std::endl;
	std::vector<int> vec;
	for (int i = 1; i <= 10; ++i)
		vec.push_back(i * 2);

	try
	{
		std::vector<int>::iterator it = easyfind(vec, 8);
		std::cout << "Found: " << *it << " at position " << (it - vec.begin()) << std::endl;
	}
	catch (const std::exception& e)
	{
		std::cout << "Exception: " << e.what() << std::endl;
	}

	try
	{
		easyfind(vec, 27);
	}
	catch (const std::exception& e)
	{
		std::cout << "Exception: " << e.what() << std::endl;
	}

	std::cout << "\n=== Testing easyfind with std::list ===" << std::endl;
	std::list<int> lst;
	for (int i = 0; i < 5; ++i)
		lst.push_back(i * 3);

	try
	{
		std::list<int>::iterator it = easyfind(lst, 9);
		std::cout << "Found: " << *it << std::endl;
	}
	catch (const std::exception& e)
	{
		std::cout << "Exception: " << e.what() << std::endl;
	}

	try
	{
		easyfind(lst, 42);
	}
	catch (const std::exception& e)
	{
		std::cout << "Exception: " << e.what() << std::endl;
	}

	std::cout << "\n=== Testing easyfind with std::deque ===" << std::endl;
	std::deque<int> deq;
	deq.push_back(100);
	deq.push_back(200);
	deq.push_back(300);

	try
	{
		std::deque<int>::iterator it = easyfind(deq, 200);
		std::cout << "Found: " << *it << std::endl;
	}
	catch (const std::exception& e)
	{
		std::cout << "Exception: " << e.what() << std::endl;
	}

	try
	{
		easyfind(deq, 150);
	}
	catch (const std::exception& e)
	{
		std::cout << "Exception: " << e.what() << std::endl;
	}

	//Note: Subject says second param of easyfind has to be int, so instead of just making one i did 2
	// one as the subject requires, and the second is set up to handle anything to find...
	// Will remain quoted just in case ;)
	// std::cout << "\n=== Testing easyfind with std::vector<std::string> ===" << std::endl;
	// std::vector<std::string> strVec;
	// strVec.push_back("thing0");
	// strVec.push_back("thing2");
	// strVec.push_back("thing1");
	// strVec.push_back("thing4");
	// strVec.push_back("thing3");
	// strVec.push_back("thing5");

	// for (std::vector<std::string>::iterator it = strVec.begin(); it != strVec.end(); it++)
	// 	std::cout << "Position[" << (it - strVec.begin()) << "]: " << *it << std::endl;
	// try
	// {
	// 	std::vector<std::string>::iterator it = easyfind(strVec, std::string("thing1"));
	// 	std::cout << "Found: " << *it << " at position " << (it - strVec.begin()) << std::endl;
	// }
	// catch (const std::exception& e)
	// {
	// 	std::cout << "Exception: " << e.what() << std::endl;
	// }

	// try
	// {
	// 	easyfind(strVec, std::string("notfound"));
	// }
	// catch (const std::exception& e)
	// {
	// 	std::cout << "Exception: " << e.what() << std::endl;
	// }

	return 0;
}
