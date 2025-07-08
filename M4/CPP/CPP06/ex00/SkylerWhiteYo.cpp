#include "SkylerWhiteYo.hpp"

ScalarConverter::ScalarConverter(){}

ScalarConverter::~ScalarConverter() {}

ScalarConverter::ScalarConverter(const ScalarConverter &other) {
    (void)other;
}

ScalarConverter &ScalarConverter::operator=(const ScalarConverter &other) {
    (void)other;
    return *this;
}

bool ScalarConverter::isChar(const std::string &literal) {
    return literal.length() == 3 && literal[0] == '\'' && literal[2] == '\'';
}

bool ScalarConverter::isInt(const std::string &literal) {
    if (literal.empty())
        return false;
        
    size_t i = 0;
    if (literal[0] == '-' || literal[0] == '+')
        i++;
        
    if (i == literal.length())
        return false;
        
    for (; i < literal.length(); i++) {
        if (!isdigit(literal[i]))
            return false;
    }
    return true;
}

bool ScalarConverter::isFloat(const std::string &literal) {
    if (literal == "nanf" || literal == "+inff" || literal == "-inff")
        return true;
        
    if (literal.length() < 2 || literal[literal.length() - 1] != 'f')
        return false;
        
    std::string withoutF = literal.substr(0, literal.length() - 1);
    
    size_t i = 0;
    bool hasDecimal = false;
    
    if (withoutF[0] == '-' || withoutF[0] == '+')
        i++;
        
    for (; i < withoutF.length(); i++) {
        if (withoutF[i] == '.') {
            if (hasDecimal)
                return false;
            hasDecimal = true;
        } else if (!isdigit(withoutF[i])) {
            return false;
        }
    }
    
    return hasDecimal;
}


bool ScalarConverter::isDouble(const std::string &literal) {
    if (literal == "nan" || literal == "+inf" || literal == "-inf")
        return true;
        
    size_t i = 0;
    bool hasDecimal = false;
    
    if (literal[0] == '-' || literal[0] == '+')
        i++;
        
    for (; i < literal.length(); i++)
	{
        if (literal[i] == '.')
		{
            if (hasDecimal)
                return false;
            hasDecimal = true;
        }
		else if (!isdigit(literal[i]))
            return false;
    }
    
    return hasDecimal;
}

bool ScalarConverter::isPseudoLiteral(const std::string &literal) {
    return literal == "nan" || literal == "+inf" || literal == "-inf" || 
           literal == "nanf" || literal == "+inff" || literal == "-inff";
}

void ScalarConverter::convertFromChar(char c) {
    std::cout << "Char: ";
    if (isprint(c))
        std::cout << "'" << c << "'" << std::endl;
    else
        std::cout << "Non displayable" << std::endl;
        
    std::cout << "Int: " << static_cast<int>(c) << std::endl;
    std::cout << "Float: " << static_cast<float>(c) << ".0f" << std::endl;
    std::cout << "Double: " << static_cast<double>(c) << ".0" << std::endl;
}

void ScalarConverter::convertFromInt(int i) {
    std::cout << "Char: ";
    if (i >= 0 && i <= 127)
	{
        if (isprint(i))
            std::cout << "'" << static_cast<char>(i) << "'" << std::endl;
        else
            std::cout << "Non displayable" << std::endl;
    }
	else
        std::cout << "impossible" << std::endl;
    
    std::cout << "Int: " << i << std::endl;
    std::cout << "Float: " << static_cast<float>(i) << ".0f" << std::endl;
    std::cout << "Double: " << static_cast<double>(i) << ".0" << std::endl;
}

void ScalarConverter::convertFromFloat(float f) {
    std::cout << "Char: ";
    if ((f != f) || (f && f == 0.5f * f) || f < 0 || f > 127)
        std::cout << "impossible" << std::endl;
    else if (!isprint(static_cast<int>(f)))
        std::cout << "Non displayable" << std::endl;
    else
        std::cout << "'" << static_cast<char>(f) << "'" << std::endl;
    
    std::cout << "Int: ";
    if ((f != f) || (f && f == 0.5f * f) || f > std::numeric_limits<int>::max() || f < std::numeric_limits<int>::min())
        std::cout << "impossible" << std::endl;
    else
        std::cout << static_cast<int>(f) << std::endl;
    
    if (f == static_cast<int>(f))
        std::cout << "Float: " << f << ".0f" << std::endl;
    else
        std::cout << "Float: " << f << "f" << std::endl;
    
    if (f == static_cast<int>(f))
        std::cout << "Double: " << static_cast<double>(f) << ".0" << std::endl;
    else
        std::cout << "Double: " << static_cast<double>(f) << std::endl;
}

void ScalarConverter::convertFromDouble(double d) {
    std::cout << "Char: ";
    if ((d != d) || (d && d == 0.5 * d) || d < 0 || d > 127)
        std::cout << "impossible" << std::endl;
    else if (!isprint(static_cast<int>(d)))
        std::cout << "Non displayable" << std::endl;
    else
        std::cout << "'" << static_cast<char>(d) << "'" << std::endl;
    
    std::cout << "Int: ";
    if ((d != d) || (d && d == 0.5 * d) || d > std::numeric_limits<int>::max() || d < std::numeric_limits<int>::min())
        std::cout << "impossible" << std::endl;
    else
        std::cout << static_cast<int>(d) << std::endl;
    
    std::cout << "Float: ";
    if (d > std::numeric_limits<float>::max() || d < -std::numeric_limits<float>::max())
        std::cout << "impossible" << std::endl;
    else {
        if (d == static_cast<int>(d))
            std::cout << static_cast<float>(d) << ".0f" << std::endl;
        else
            std::cout << static_cast<float>(d) << "f" << std::endl;
    }
    
    if (d == static_cast<int>(d))
        std::cout << "Double: " << d << ".0" << std::endl;
    else
        std::cout << "Double: " << d << std::endl;
}

void ScalarConverter::convertFromPseudoLiteral(const std::string &literal) {
    std::cout << "Char: impossible" << std::endl;
    std::cout << "Int: impossible" << std::endl;
    
    if (literal == "nanf" || literal == "+inff" || literal == "-inff") {
        std::cout << "Float: " << literal << std::endl;
        std::cout << "Double: " << literal.substr(0, literal.length() - 1) << std::endl;
    } else {
        std::cout << "Float: " << literal << "f" << std::endl;
        std::cout << "Double: " << literal << std::endl;
    }
}

void ScalarConverter::convert(const std::string &literal) {
    if (literal.empty()) {
        std::cout << "Error: Empty input" << std::endl;
        return;
    }
    
    if (isPseudoLiteral(literal)) {
        convertFromPseudoLiteral(literal);
        return;
    }
    if (isChar(literal)) {
        convertFromChar(literal[1]);
        return;
    }
    if (isInt(literal)) {
        try {
            char *endptr;
            long value = std::strtol(literal.c_str(), &endptr, 10);
            if (value > std::numeric_limits<int>::max() || value < std::numeric_limits<int>::min()) {
                std::cout << "Error: Integer overflow" << std::endl;
                return;
            }
            convertFromInt(static_cast<int>(value));
        } catch (const std::exception &e) {
            std::cout << "Error: Invalid integer conversion" << std::endl;
        }
        return;
    }
    if (isFloat(literal)) {
        try {
            char *endptr;
            float value = static_cast<float>(std::strtod(literal.c_str(), &endptr));
            convertFromFloat(value);
        } catch (const std::exception &e) {
            std::cout << "Error: Invalid float conversion" << std::endl;
        }
        return;
    }
    if (isDouble(literal)) {
        try {
            char *endptr;
            double value = std::strtod(literal.c_str(), &endptr);
            convertFromDouble(value);
        } catch (const std::exception &e) {
            std::cout << "Error: Invalid double conversion" << std::endl;
        }
        return;
    }
    if (literal.length() == 1) {
        convertFromChar(literal[0]);
        return;
    }
    std::cout << "Error: Unrecognized input format" << std::endl;
}
