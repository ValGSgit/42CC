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
		
		if (i >= trimmed.length()) break;
		
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

/**
 * Takes a stack of string tokens and processes them according to RPN rules.
 * Numbers are pushed onto an evaluation stack, operators pop two numbers,
 * perform the operation, and push the result back.
 * Uses only std::stack as required by the exercise.
 * @param tokens Stack of string tokens (numbers and operators) to evaluate
 * @return The final result, might have to turn it into a long (?)
 * @throws std::invalid_argument if division by zero or invalid RPN structure
 */
int evaluateRPN(std::stack<std::string> tokens) {
	std::stack<int> evaluationStack;
	// Since stack is LIFO, we need to process in reverse order
	std::stack<std::string> tempStack;
	while (!tokens.empty()) {
		tempStack.push(tokens.top()); // Flip. That. Thiiiiing
		tokens.pop();
	}

	while (!tempStack.empty()) {
		std::string token = tempStack.top();
		tempStack.pop();
		if (token.length() == 1 && std::isdigit(token[0])) {
			int number = token[0] - '0';
			evaluationStack.push(number);
		}
		else if (token == "+" || token == "-" || token == "*" || token == "/") {
			if (evaluationStack.size() < 2) {
				throw std::invalid_argument("Error: insufficient operands for operator");
			}
			// Pop two operands (note: second operand is popped first)
			int operand2 = evaluationStack.top();
			evaluationStack.pop();
			int operand1 = evaluationStack.top();
			evaluationStack.pop();
			
			int result = 0;
			if (token == "+") {
				if (operand1 > 0 && operand2 > INT_MAX - operand1) {
					throw std::invalid_argument("Error: overflow");
				}
				if (operand1 < 0 && operand2 < INT_MIN - operand1) {
					throw std::invalid_argument("Error: underflow");
				}
				result = operand1 + operand2;
			}
			else if (token == "-") {
				if (operand1 > 0 && operand2 < operand1 - INT_MAX) {
					throw std::invalid_argument("Error: overflow");
				}
				if (operand1 < 0 && operand2 > operand1 - INT_MIN) {
					throw std::invalid_argument("Error: underflow");
				}
				result = operand1 - operand2;
			}
			else if (token == "*") {
				if (operand1 != 0 && operand2 != 0) {
					if (operand1 > 0 && operand2 > 0 && operand1 > INT_MAX / operand2) {
						throw std::invalid_argument("Error: overflow");
					}
					if (operand1 < 0 && operand2 < 0 && operand1 < INT_MAX / operand2) {
						throw std::invalid_argument("Error: overflow");
					}
					if (operand1 > 0 && operand2 < 0 && operand2 < INT_MIN / operand1) {
						throw std::invalid_argument("Error: underflow");
					}
					if (operand1 < 0 && operand2 > 0 && operand1 < INT_MIN / operand2) {
						throw std::invalid_argument("Error: underflow");
					}
				}
				result = operand1 * operand2;
			}
			else if (token == "/") {
				if (operand2 == 0) {
					throw std::invalid_argument("Error: division by zero");
				}
				// Check for overflow in division (only case: INT_MIN / -1)
				if (operand1 == INT_MIN && operand2 == -1) {
					throw std::invalid_argument("Error: overflow");
				}
				result = operand1 / operand2;
			}
			evaluationStack.push(result);
		}
		else {
			throw std::invalid_argument("Error: invalid token in evaluation");
		}
	}
	if (evaluationStack.size() != 1) {
		throw std::invalid_argument("Error: invalid RPN expression - incorrect number of operands");
	}
	return evaluationStack.top();
}