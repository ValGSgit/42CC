#ifndef SKYLERWHITEYO_HPP
#define SKYLERWHITEYO_HPP

#include <iostream>
#include <string>
#include <cstdlib>
#include <limits>
#include <sstream> //For the test main

class ScalarConverter {
    private:
        ScalarConverter();
        ~ScalarConverter();
        ScalarConverter(const ScalarConverter &other);
        ScalarConverter &operator=(const ScalarConverter &other);

        static bool isChar(const std::string &literal);
        static bool isInt(const std::string &literal);
        static bool isFloat(const std::string &literal);
        static bool isDouble(const std::string &literal);
        static bool isPseudoLiteral(const std::string &literal);

        static void convertFromChar(char c);
        static void convertFromInt(int i);
        static void convertFromFloat(float f);
        static void convertFromDouble(double d);
        static void convertFromPseudoLiteral(const std::string &literal);

    public:
        static void convert(const std::string &literal);
};

#endif
