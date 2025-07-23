#ifndef RPN_HPP
# define RPN_HPP

# include <iostream>
# include <iomanip>
# include <stack>
# include <algorithm>
# include <sstream>
# include <cstdio>   // For EOF
# include <string>
# include <stdexcept>
# include <cctype>
# include <climits>  // For INT_MAX and INT_MIN

std::string ft_trim(const std::string& str);
std::stack<std::string> parseRPNTokens(const std::string& input);
double evaluateRPN(std::stack<std::string> tokens);

#endif