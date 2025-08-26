#include "RPN.hpp"

std::string ft_trim(const std::string& str) 
{
	size_t start = str.find_first_not_of(" \t\r\n");
	if (start == std::string::npos)
		return "";
	size_t end = str.find_last_not_of(" \t\r\n");
	return str.substr(start, end - start + 1);
}

std::stack<std::string> parseRPNTokens(const std::string& input) {
	std::stack<std::string> tokens;
	std::string trimmed = ft_trim(input);
	
	if (trimmed.empty()) {
		throw std::invalid_argument("Error: empty expression");
	}
	
	int numberCount = 0;
	int operatorCount = 0;
	
	size_t i = 0;
	while (i < trimmed.length()) {
		while (i < trimmed.length() && (trimmed[i] == ' ' || trimmed[i] == '\t' || 
			   trimmed[i] == '\r' || trimmed[i] == '\n')) {
			i++;
		}
		
		if (i >= trimmed.length())
			break;
		
		char currentChar = trimmed[i];
		if (std::isdigit(currentChar)) {
			if (i + 1 < trimmed.length() && std::isdigit(trimmed[i + 1])) {
				throw std::invalid_argument("Error: numbers must be single digits (0-9)");
			}
			tokens.push(std::string(1, currentChar));
			numberCount++;
			i++;
		}
		else if (currentChar == '+' || currentChar == '-' || 
				 currentChar == '*' || currentChar == '/') {
			tokens.push(std::string(1, currentChar));
			operatorCount++;
			i++;
		}
		else {
			std::string errorMsg = "Error: invalid character '";
			errorMsg += currentChar;
			errorMsg += "'";
			throw std::invalid_argument(errorMsg);
		}
	}
	if (tokens.empty()) {
		throw std::invalid_argument("Error: no valid tokens found");
	}
	if (numberCount < 2) {
		throw std::invalid_argument("Error: need at least 2 numbers for RPN calculation");
	}
	if (operatorCount < 1) {
		throw std::invalid_argument("Error: need at least 1 operator for RPN calculation");
	}
	if (operatorCount != numberCount - 1) {
		throw std::invalid_argument("Error: invalid RPN expression - need exactly (numbers - 1) operators");
	}
	return tokens;
}

double evaluateRPN(std::stack<std::string> tokens) {
	std::stack<double> numberStack;
	std::stack<std::string> tempStack;
	while (!tokens.empty()) {
		tempStack.push(tokens.top()); // Flip. That. Thiiiiing
		tokens.pop();
	}

	while (!tempStack.empty()) {
		std::string token = tempStack.top();
		tempStack.pop();
		if (token.length() == 1 && std::isdigit(token[0])) {
			double number = token[0] - '0';
			numberStack.push(number);
		}
		else if (token == "+" || token == "-" || token == "*" || token == "/") {
			if (numberStack.size() < 2) {
				throw std::invalid_argument("Error: insufficient operands for operator");
			}
			// note: second operand is popped first
			double num2 = numberStack.top();
			numberStack.pop();
			double num1 = numberStack.top();
			numberStack.pop();
			
			double result = 0;
			// * Proceeds to handle all overflow/underflow cases cuz why not*
			if (token == "+") {
				if (num1 > 0 && num2 > INT_MAX - num1) {
					throw std::invalid_argument("Error: overflow");
				}
				if (num1 < 0 && num2 < INT_MIN - num1) {
					throw std::invalid_argument("Error: underflow");
				}
				result = num1 + num2;
			}
			else if (token == "-") {
				if (num1 > 0 && num2 < num1 - INT_MAX) {
					throw std::invalid_argument("Error: overflow");
				}
				if (num1 < 0 && num2 > num1 - INT_MIN) {
					throw std::invalid_argument("Error: underflow");
				}
				result = num1 - num2;
			}
			else if (token == "*") {
				if (num1 != 0 && num2 != 0) {
					if (num1 > 0 && num2 > 0 && num1 > INT_MAX / num2) {
						throw std::invalid_argument("Error: overflow");
					}
					if (num1 < 0 && num2 < 0 && num1 < INT_MAX / num2) {
						throw std::invalid_argument("Error: overflow");
					}
					if (num1 > 0 && num2 < 0 && num2 < INT_MIN / num1) {
						throw std::invalid_argument("Error: underflow");
					}
					if (num1 < 0 && num2 > 0 && num1 < INT_MIN / num2) {
						throw std::invalid_argument("Error: underflow");
					}
				}
				result = num1 * num2;
			}
			else if (token == "/") {
				if (num2 == 0) {
					throw std::invalid_argument("Error: division by zero");
				}
				if (num1 == INT_MIN && num2 == -1) {
					throw std::invalid_argument("Error: overflow");
				}
				result = num1 / num2;
			}
			numberStack.push(result);
		}
		else {
			throw std::invalid_argument("Error: invalid token in evaluation");
		}
	}
	if (numberStack.size() != 1) {
		throw std::invalid_argument("Error: invalid RPN expression - incorrect number of operands");
	}
	return numberStack.top();
}