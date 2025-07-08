#include "MutantStack.hpp"
#include <iostream>
#include <list>

int main()
{
	std::cout << "=== Subject Test with MutantStack ===" << std::endl;
	{
		MutantStack<int> mstack;
		mstack.push(5);
		mstack.push(17);
		std::cout << mstack.top() << std::endl;
		mstack.pop();
		std::cout << mstack.size() << std::endl;
		mstack.push(3);
		mstack.push(5);
		mstack.push(737);
		mstack.push(0);
		MutantStack<int>::iterator thus = mstack.begin();
		MutantStack<int>::iterator that = mstack.end();
		if (thus != that) {
			++thus;
			--thus;
		}
		while (thus != that)
		{
			std::cout << *thus << std::endl;
			++thus;
		}
		std::stack<int> s(mstack);
	}

	std::cout << "\n=== Same Test with std::list ===" << std::endl;
	{
		std::list<int> mstack;
		mstack.push_back(5);
		mstack.push_back(17);
		std::cout << mstack.back() << std::endl;
		mstack.pop_back();
		std::cout << mstack.size() << std::endl;
		mstack.push_back(3);
		mstack.push_back(5);
		mstack.push_back(737);
		mstack.push_back(0);
		std::list<int>::iterator it = mstack.begin();
		std::list<int>::iterator that = mstack.end();
		if (it != that) {
			++it;
			--it;
		}
		while (it != that)
		{
			std::cout << *it << std::endl;
			++it;
		}
	}

	std::cout << "\n=== Additional Tests ===" << std::endl;
	{
		MutantStack<int> mstack;
		std::cout << "Empty stack size: " << mstack.size() << std::endl;
		std::cout << "Empty stack iterators equal: " << (mstack.begin() == mstack.end()) << std::endl;
		for (int i = 1; i <= 5; ++i)
			mstack.push(i * 10);

		std::cout << "Stack size after adding elements: " << mstack.size() << std::endl;
		std::cout << "Top element: " << mstack.top() << std::endl;

		std::cout << "Forward iteration: ";
		for (MutantStack<int>::iterator it = mstack.begin(); it != mstack.end(); ++it)
			std::cout << *it << " ";
		std::cout << std::endl;

		std::cout << "Reverse iteration: ";
		for (MutantStack<int>::reverse_iterator it = mstack.rbegin(); it != mstack.rend(); ++it)
			std::cout << *it << " ";
		std::cout << std::endl;

		const MutantStack<int>& const_mstack = mstack;
		std::cout << "Const iteration: ";
		for (MutantStack<int>::const_iterator it = const_mstack.begin(); it != const_mstack.end(); ++it)
			std::cout << *it << " ";
		std::cout << std::endl;
		
		std::cout << "Const Reverse iteration: ";
		for (MutantStack<int>::const_reverse_iterator it = mstack.rbegin(); it != mstack.rend(); ++it)
			std::cout << *it << " ";
		std::cout << std::endl;
	}

	std::cout << "\n=== Copy and Assignment Tests ===" << std::endl;
	{
		MutantStack<int> original;
		original.push(100);
		original.push(200);
		original.push(300);

		MutantStack<int> copy(original);
		std::cout << "Original top: " << original.top() << ", Copy top: " << copy.top() << std::endl;

		MutantStack<int> assigned;
		assigned = original;
		std::cout << "Assigned top: " << assigned.top() << std::endl;

		original.push(400);
		original.top() = 69;
		copy.top() = 420;
		std::cout << "After modifying original:" << std::endl;
		std::cout << "Original size: " << original.size() << ", Copy size: " << copy.size() << std::endl;
		std::cout << "Original top: " << original.top() << ", Copy top: " << copy.top() << std::endl;
	}

	return 0;
}
