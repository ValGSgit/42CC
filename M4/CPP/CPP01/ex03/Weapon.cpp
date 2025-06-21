#include "Weapon.hpp"

Weapon::Weapon() : type("default weapon")
{
}

Weapon::Weapon(const std::string& type) : type(type)
{
}

Weapon::~Weapon()
{
}

const std::string& Weapon::getType() const
{
	return this->type;
}

void Weapon::setType(const std::string& newType)
{
	this->type = newType;
}