#ifndef BRAIN_HPP
#define BRAIN_HPP

#include <string>
#include <iostream>
#include <sstream>

class Brain {
private:
    std::string ideas[100];

public:
    Brain();
    Brain(const Brain& src);
    Brain& operator=(const Brain& src);
    ~Brain();
    
    const std::string& getIdea(int index) const;
    void setIdea(int index, const std::string& idea);
};

#endif
