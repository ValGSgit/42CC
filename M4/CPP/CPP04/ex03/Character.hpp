#ifndef CHARACTER_HPP
#define CHARACTER_HPP

#include "ICharacter.hpp"
#include "AMateria.hpp"

class Character : public ICharacter {
    private:
        std::string name;
        AMateria* inventory[4];
        AMateria* floor[100]; // To store unequipped materias
        int floorCount;

    public:
        Character(std::string const & name);
        Character(const Character& src);
        Character& operator=(const Character& src);
        virtual ~Character();

        virtual std::string const & getName() const;
        virtual void equip(AMateria* m);
        virtual void unequip(int idx);
        virtual void use(int idx, ICharacter& target);
};

#endif
