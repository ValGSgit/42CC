#include "Brain.hpp"

Brain::Brain() {
    std::cout << "Brain default constructor called" << std::endl;
    for (int i = 0; i < 100; i++) {
        std::stringstream ss;
        ss << "Default idea " << i;
        this->ideas[i] = ss.str();
    }
}

Brain::Brain(const Brain& src) {
    std::cout << "Brain copy constructor called" << std::endl;
    for (int i = 0; i < 100; i++) {
        this->ideas[i] = src.ideas[i];
    }
}

Brain& Brain::operator=(const Brain& src) {
    std::cout << "Brain copy assignment operator called" << std::endl;
    if (this != &src) {
        for (int i = 0; i < 100; i++) {
            this->ideas[i] = src.ideas[i];
        }
    }
    return *this;
}

Brain::~Brain() {
    std::cout << "Brain destructor called" << std::endl;
}

const std::string& Brain::getIdea(int index) const {
    if (index >= 0 && index < 100)
        return this->ideas[index];
    static std::string empty;
    return empty;
}

void Brain::setIdea(int index, const std::string& idea) {
    if (index >= 0 && index < 100)
        this->ideas[index] = idea;
}
