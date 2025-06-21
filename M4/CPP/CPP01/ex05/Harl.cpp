#include "Harl.hpp"

void Harl::debug()
{
    std::cout << "[DEBUG] Debugging is like being a detective in a crime movie where you're also the killer!" << std::endl;
}

void Harl::info()
{
    std::cout << "[INFO] Did you know you can make nitroglycerin with Peanut oil?" << std::endl;
}

void Harl::warning()
{
    std::cout << "[WARNING] My rubber duck just gave me the silent treatment - debugging crisis imminent!" << std::endl;
}

void Harl::error()
{
    std::cout << "[ERROR] THE COFFEE MACHINE IS BROKEN! EVACUATE THE BUILDING! THIS IS NOT A DRILL!" << std::endl;
}

void	Harl::showCase( void )
{
	std::cout << "--- Harl's Possible Levels ---" << std::endl;
	debug();
	info();
	warning();
	error();
	std::cout << "--- END OF SHOWCASE ---" << std::endl;
}

LogLevel Harl::getLogLevel(const std::string& level)
{
    const std::string levels[4] = {"DEBUG", "INFO", "WARNING", "ERROR"};
    
    for (int i = 0; i < 4; i++)
	{
        if (levels[i] == level)
		{
            return static_cast<LogLevel>(i);
        }
    }
    return INVALID;
}

void Harl::complain(std::string level)
{
    if (level.empty())
    {
        std::cout << "[EMPTY] Empty input given, dont worry about it" << std::endl;
        return ;
    }
    LogLevel logLevel = getLogLevel(level);

    LogFunction functions[4] = {
        &Harl::debug,
        &Harl::info,
        &Harl::warning,
        &Harl::error
    };

    switch (logLevel)
    {
        case DEBUG:
            (this->*functions[DEBUG])();
            break;
        case INFO:
            (this->*functions[INFO])();
            break;
        case WARNING:
            (this->*functions[WARNING])();
            break;
        case ERROR:
            (this->*functions[ERROR])();
            break;
        case INVALID:
            std::cout << "[INVALID] Unknown log level '" << level << "' - did you make that up?" << std::endl;
            break;
    }
}
