#include "RPN.hpp"

int main (int a, char **v) {
	if (a != 2) {
		std::cout << "Error" << std::endl;
		return 1;
	}
	std::string input = v[1];
	
	try {
		std::stack<std::string> tokens = parseRPNTokens(v[1]);
		double result = evaluateRPN(tokens);
		std::cout << result << std::endl;
		
	} catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return 1;
	}
	return 0;
}

// If input looks like -> "1 2 + 3 4 + *":
// 1. Push 1     → [1]
// 2. Push 2     → [1, 2]
// 3. Pop 2,1 → 1+2=3 → [3]        ← First addition: 1+2
// 4. Push 3     → [3, 3]
// 5. Push 4     → [3, 3, 4]
// 6. Pop 4,3 → 3+4=7 → [3, 7]     ← Second addition: 3+4
// 7. Pop 7,3 → 3*7=21 → [21]      ← Multiplication: (1+2)*(3+4)