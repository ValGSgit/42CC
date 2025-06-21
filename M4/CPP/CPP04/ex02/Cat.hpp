#ifndef CAT_HPP
#define CAT_HPP

#include "AAnimal.hpp"
#include "Brain.hpp"

class Cat : public AAnimal {
    private:
        Brain* brain;

    public:
        Cat();
        Cat(const Cat& src);
        Cat& operator=(const Cat& src);
        virtual ~Cat();

        virtual void makeSound() const;
};

#endif
