/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vagarcia <vagarcia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/16 10:22:54 by vagarcia          #+#    #+#             */
/*   Updated: 2025/06/16 12:54:19 by vagarcia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include "Fixed.hpp"

int main(void) {
    Fixed a;
    Fixed const b(Fixed(512.05512f) * Fixed(2));
    
    std::cout << a << std::endl;
    std::cout << ++a << std::endl;
    std::cout << a << std::endl;
    std::cout << a++ << std::endl;
    std::cout << a << std::endl;
    std::cout << (a = a + Fixed(1.623611f)) << std::endl;
    
    std::cout << b << std::endl;
    
    std::cout << Fixed::max(a, b) << std::endl;
    
	// Extra tests to see its good
    Fixed c(420.206969f);
    Fixed d(69.80420f);
    
    std::cout << "\n--- Comparison operators ---" << std::endl;
    std::cout << "c = " << c << ", d = " << d << std::endl;
    std::cout << "c > d: " << (c > d) << std::endl;
    std::cout << "c < d: " << (c < d) << std::endl;
    std::cout << "c >= d: " << (c >= d) << std::endl;
    std::cout << "c <= d: " << (c <= d) << std::endl;
    std::cout << "c == d: " << (c == d) << std::endl;
    std::cout << "c != d: " << (c != d) << std::endl;
    
    std::cout << "\n--- Arithmetic operators ---" << std::endl;
    std::cout << "c + d = " << (c + d) << std::endl;
    std::cout << "c - d = " << (c - d) << std::endl;
    std::cout << "c * d = " << (c * d) << std::endl;
    std::cout << "c / b = " << (c / d) << std::endl;
    
    std::cout << "\n--- Decrement/Increment operators ---" << std::endl;
    std::cout << "c before: " << c << std::endl;
    std::cout << "--c: " << --c << std::endl;
    std::cout << "c after pre-decrement: " << c << std::endl;
    std::cout << "c--: " << c-- << std::endl;
    std::cout << "c after post-decrement: " << c << std::endl;
    std::cout << "a before: " << a << std::endl;
    std::cout << "++a: " << ++a << std::endl;
    std::cout << "a after pre-increment: " << a << std::endl;
    std::cout << "a--: " << a++ << std::endl;
    std::cout << "a after post-increment: " << a << std::endl;
    
    std::cout << "\n--- Min/Max functions ---" << std::endl;
    std::cout << "min(c, d): " << Fixed::min(c, d) << std::endl;
    std::cout << "max(c, d): " << Fixed::max(c, d) << std::endl;
    
    return 0;
}
