#include "BitcoinExchange.hpp"
# include <iostream>
# include <map>
# include <string>
# include <fstream>
# include <algorithm>
# include <ctime>
# include <cstring>
# include <iomanip>

BitcoinExchange::BitcoinExchange() : _dataLoaded(false) {
	_dataLoaded = loadDatabase();
}

BitcoinExchange::BitcoinExchange(const BitcoinExchange& other) : _dataBase(other._dataBase), _dataLoaded(other._dataLoaded) {
}

BitcoinExchange& BitcoinExchange::operator=(const BitcoinExchange& other) {
	if (this != &other) {
		_dataBase = other._dataBase;
		_dataLoaded = other._dataLoaded;
	}
	return *this;
}

BitcoinExchange::~BitcoinExchange() {
}

bool BitcoinExchange::loadDatabase() {
	std::ifstream file("data.csv");
	if (!file.is_open()) {
		std::cerr << "Error: could not open database file." << std::endl;
		return false;
	}
	
	std::string line;
	std::getline(file, line);
	int lineNumber = 1;
	while (std::getline(file, line)) {
		lineNumber++;
		size_t commaPos = line.find(',');
		if (commaPos != std::string::npos) {
			std::string date = ft_trim(line.substr(0, commaPos));
			std::string rateStr = ft_trim(line.substr(commaPos + 1));
			
			if (!isValidDate(date)) {
				std::cerr << "Error: invalid date in database at line " << lineNumber << ": " << date << std::endl;
				file.close();
				return false;
			}

			char* endptr;
			double rate = std::strtod(rateStr.c_str(), &endptr);
			if (*endptr != '\0' || rate < 0) {
				std::cerr << "Error: invalid rate in database at line " << lineNumber << ": " << rateStr << std::endl;
				file.close();
				return false;
			}
			
			_dataBase[date] = static_cast<double>(rate);
		} else {
			std::cerr << "Error: invalid format in database at line " << lineNumber << ": " << line << std::endl;
			file.close();
			return false;
		}
	}
	file.close();
	
	if (_dataBase.empty()) {
		std::cerr << "Error: database file is empty or invalid." << std::endl;
		return false;
	}
	
	return true;
}

std::string BitcoinExchange::ft_trim(const std::string& str) const {
	size_t start = str.find_first_not_of(" \t\r\n");
	if (start == std::string::npos)
		return "";
	size_t end = str.find_last_not_of(" \t\r\n");
	return str.substr(start, end - start + 1);
}

bool BitcoinExchange::parseDate(const std::string &dateStr, struct tm &dateStruct) const {
	if (dateStr.empty() || dateStr.length() != 10)
		return false;
	if (dateStr[4] != '-' || dateStr[7] != '-')
		return false;
	
	std::string yearStr = dateStr.substr(0, 4);
	std::string monthStr = dateStr.substr(5, 2);
	std::string dayStr = dateStr.substr(8, 2);
	
	for (size_t i = 0; i < yearStr.length(); ++i) {
		if (!std::isdigit(yearStr[i]))
			return false;
	}
	for (size_t i = 0; i < monthStr.length(); ++i) {
		if (!std::isdigit(monthStr[i]))
			return false;
	}
	for (size_t i = 0; i < dayStr.length(); ++i) {
		if (!std::isdigit(dayStr[i]))
			return false;
	}

	int year = std::atoi(yearStr.c_str());
	int month = std::atoi(monthStr.c_str());
	int day = std::atoi(dayStr.c_str());
	
	if (year < 1900 || year > 3000)
		return false;
	if (month < 1 || month > 12)
		return false;
	if (day < 1 || day > 31)
		return false;

	std::memset(&dateStruct, 0, sizeof(dateStruct));
	dateStruct.tm_year = year - 1900;  // tm_year is years since 1900
	dateStruct.tm_mon = month - 1;     // tm_mon is 0-11
	dateStruct.tm_mday = day;          // tm_mday is 1-31
	dateStruct.tm_hour = 12;           // Set to noon to avoid DST issues
	dateStruct.tm_min = 0;
	dateStruct.tm_sec = 0;
	dateStruct.tm_isdst = -1;          // Let system determine DaylightSavingTime
	
	return true;
}

//I regret using this approach tbh
bool BitcoinExchange::validateDateStructure(const struct tm &dateStruct) const {
	// Create a copy for mktime (mktime modifies the struct)
	struct tm tempStruct = dateStruct;
	int originalYear = tempStruct.tm_year;
	int originalMonth = tempStruct.tm_mon;
	int originalDay = tempStruct.tm_mday;
	
	// mktime validates and normalizes the date
	time_t timeValue = std::mktime(&tempStruct);
	if (timeValue == -1)
		return false;
	
	// if mktime changed any values should be invalid date
	if (tempStruct.tm_year != originalYear ||
		tempStruct.tm_mon != originalMonth ||
		tempStruct.tm_mday != originalDay)
		return false;

	int actualYear = originalYear + 1900;
	// Bitcoin's genesis block was created on January 2, 2009
	if (actualYear == 2009 && originalMonth == 0 && originalDay < 2)
		return false;
	if (actualYear < 2009 || actualYear > 2030)
		return false;
	
	return true;
}

bool BitcoinExchange::isValidDate(const std::string& date) const {
	struct tm dateStruct;
	
	if (!parseDate(date, dateStruct))
		return false;
	if (!validateDateStructure(dateStruct))
		return false;
	
	return true;
}

bool BitcoinExchange::isValidValue(const std::string& valueStr, double& value) const {
	char* endptr;
	double tempValue = std::strtod(valueStr.c_str(), &endptr);
	
	if (*endptr != '\0')
		return false;
	if (tempValue < 0)
		return false;
	if (tempValue > 1000)
		return false;
	
	value = static_cast<double>(tempValue);
	return true;
}

std::string BitcoinExchange::findClosestDate(const std::string& date) const {
	if (_dataBase.empty()) {
		return "";
	}
	std::map<std::string, double>::const_iterator it = _dataBase.lower_bound(date);
	// lower_bound returns an iterator pointing to the first element that is not less than the search value
	if (it == _dataBase.begin()) {
		return it->first;
	}
	// also lower_bound might return an iterator to end() if the search date is greater than all dates in database
	if (it == _dataBase.end() || it->first != date) {
		--it;
	}
	return it->first; //First is special map iterator for key (first = key, second = value)
}	

void BitcoinExchange::processLine(const std::string& line) {
	if (!_dataLoaded) {
		std::cout << "Error: database not available." << std::endl;
		return;
	}
	
	size_t pipePos = line.find(" | ");
	if (pipePos == std::string::npos) {
		std::cout << "Error: bad input => " << line << std::endl;
		return;
	}
	
	std::string dateStr = ft_trim(line.substr(0, pipePos));
	std::string valueStr = ft_trim(line.substr(pipePos + 3));
	
	if (!isValidDate(dateStr)) {
		std::cout << "Error: bad input => " << dateStr << std::endl;
		return;
	}
	
	double value;
	if (!isValidValue(valueStr, value)) {
		if (std::atof(valueStr.c_str()) < 0) {
			std::cout << "Error: not a positive number." << std::endl;
		} else {
			std::cout << "Error: too large number." << std::endl;
		}
		return;
	}
	
	std::string closestDate = findClosestDate(dateStr);
	if (closestDate.empty()) {
		std::cout << "Error: no data available for date." << std::endl;
		return;
	}
	
	double exchangeRate = _dataBase.find(closestDate)->second;
	
	double result = value * exchangeRate;

	std::cout << dateStr << " => " << value << " = " << result << std::endl;
}

void BitcoinExchange::processInputFile(const std::string& filename) {
	if (!_dataLoaded) {
		std::cout << "Error: database could not be loaded." << std::endl;
		return;
	}
	
	std::ifstream file(filename.c_str());
	if (!file.is_open()) {
		std::cout << "Error: could not open file." << std::endl;
		return;
	}
	
	std::string line;
	std::getline(file, line);
	
	size_t openFormat = line.find("date | value");
	if (openFormat == std::string::npos) {
		std::cout << "Unexpected format line =>" << line << std::endl;
		return;
	}

	while (std::getline(file, line)) {
		if (!line.empty()) {
			processLine(line);
		}
	}
	
	file.close();
}
