#include "Harl.hpp"

void Harl::debug()
{
    std::cout << "[ DEBUG ]" << std::endl;
    std::cout << "I tried debugging my code, but it was like negotiating with a ghost..." << std::endl;
    std::cout << "every fix just revealed a new haunting error." << std::endl;
    std::cout << std::endl;
}

void Harl::info()
{
    std::cout << "[ INFO ]" << std::endl;
    std::cout << "I asked the INFO log for a quick summary, and it gave me a novel about every variables existential crisis." << std::endl;
    std::cout << "INFO messages are like nosy neighbors: they know everything but wont stop the ERROR from burning the house down." << std::endl;
    std::cout << std::endl;
}

void Harl::warning()
{
    std::cout << "[ WARNING ]" << std::endl;
    std::cout << "I got a WARNING at midnight, like my code was whispering, “Fix me, or Ill haunt your production server forever.”" << std::endl;
    std::cout << std::endl;
}

void Harl::error()
{
    std::cout << "[ ERROR ]" << std::endl;
    std::cout << "The ERROR said, “Im not a bug, Im a feature!”—yeah, a feature that sends my app to the graveyard." << std::endl;
    std::cout << std::endl;
}

void Harl::complain(std::string level)
{
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
        {
            std::cout << "[ Probably tried something invalid... tsk tsk tsk ]" << std::endl;
            break;
        }
    }
}


void Harl::complainFilter(std::string level)
{
    LogLevel logLevel = getLogLevel(level);

    switch (logLevel)
    {
        case DEBUG:
            debug();
        case INFO:
            info();
        case WARNING:
            warning();
        case ERROR:
            error();
            break;
        case INVALID:
            std::cout << "[ Probably complaining about insignificant problems ]" << std::endl;
             std::cout << "Also fun fact: did you know you can make nitroglycerin with Peanut oil?" << std::endl;
            break;
    }
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
