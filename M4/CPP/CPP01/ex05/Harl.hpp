#ifndef HARL_H
# define HARL_H

# include <iostream>
# include <iomanip>

enum LogLevel {
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    INVALID
};

class Harl
{
	private :
		void debug( void );
		void info( void );
		void warning( void );
		void error( void );
	public :
		LogLevel getLogLevel(const std::string& level);
		void complain( std::string level );
		typedef void (Harl::*LogFunction)();
		void	showCase( void );
} ;

#endif