/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vagarcia <vagarcia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/16 10:22:54 by vagarcia          #+#    #+#             */
/*   Updated: 2025/06/16 10:30:44 by vagarcia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Fixed.hpp"
// My First Class in Orthodox Canonical Form
// Turn-in directory : ex00/
// Files to turn in : Makefile, main.cpp, Fixed.{h, hpp}, Fixed.cpp
// Forbidden functions : None
// You think you know integers and floating-point numbers. How cute.
// Please read this 3 pages article (1, 2, 3) to discover that you don’t. Go on, read it.
// Until today, every number you used in your code was basically either an integer or a
// floating-point number, or any of their variants (short, char, long, double, and so forth).
// After reading the article above, it’s safe to assume that integers and floating-point num-
// bers have opposite characteristics.
// But today, things will change. You are going to discover a new and awesome number
// type: fixed-point numbers! Forever missing from the scalar types of most languages,
// fixed-point numbers offer a valuable balance between performance, accuracy, range and
// precision. That explains why fixed-point numbers are particularly applicable to computer
// graphics, sound processing or scientific programming, just to name a few.
// As C++ lacks fixed-point numbers, you’re going to add them. This article from
// Berkeley is a good start. If you have no idea what Berkeley University is, read this
// section of its Wikipedia page.
// 7C++ - Module 02
// Ad-hoc polymorphism, operator overloading
// and the Orthodox Canonical class form
// Create a class in Orthodox Canonical Form that represents a fixed-point number:
// • Private members:
// ◦ An integer to store the fixed-point number value.
// ◦ A static constant integer to store the number of fractional bits. Its value
// will always be the integer literal 8.
// • Public members:
// ◦ A default constructor that initializes the fixed-point number value to 0.
// ◦ A copy constructor.
// ◦ A copy assignment operator overload.
// ◦ A destructor.
// ◦ A member function int getRawBits( void ) const;
// that returns the raw value of the fixed-point value.
// ◦ A member function void setRawBits( int const raw );
// that sets the raw value of the fixed-point number.
#include <iostream>
int main( void ) {
	Fixed a;
	Fixed b( a );
	Fixed c;
	c = b;

	std::cout << a.getRawBits() << std::endl;
	std::cout << b.getRawBits() << std::endl;
	std::cout << c.getRawBits() << std::endl;
	return 0;
}