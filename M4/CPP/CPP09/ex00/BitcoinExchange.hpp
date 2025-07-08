#ifndef BITCOINEXCHANGE_HPP
# define BITCOINEXCHANGE_HPP

# include <iostream>
# include <map>
# include <string>
# include <fstream>
# include <algorithm>
# include <ctime>
# include <cstring>
# include <iomanip>

class BitcoinExchange {
	private:
		std::map<std::string, double> _dataBase;
		bool _dataLoaded;

		bool isValidDate(const std::string &date) const;
		bool isValidValue(const std::string &valStr, double &value) const;
		bool loadDatabase();
		std::string findClosestDate(const std::string &date) const;
		std::string ft_trim(const std::string &str) const;
		bool parseDate(const std::string &dateStr, struct tm &dateStruct) const;
		bool validateDateStructure(const struct tm &dateStruct) const;
		// std::string formatDate(const struct tm &dateStruct) const;
	public:
		BitcoinExchange();
		~BitcoinExchange();
		BitcoinExchange(const BitcoinExchange &other);
		BitcoinExchange &operator=(const BitcoinExchange &other);
		void processInputFile(const std::string& filename);
		void processLine(const std::string& line);
} ;

#endif