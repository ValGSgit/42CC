/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Fixed.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vagarcia <vagarcia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/16 10:22:51 by vagarcia          #+#    #+#             */
/*   Updated: 2025/06/16 13:01:35 by vagarcia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Fixed.hpp"
#include <iostream>
#include <cmath>

// =============================================================================
// Constructors and Destructor
// =============================================================================

// Default constructor
Fixed::Fixed() : _fixedPointValue(0) {
}

// Int constructor
Fixed::Fixed(const int value) {
    _fixedPointValue = value << _fractionalBits;
}

// Float constructor
Fixed::Fixed(const float value) {
    _fixedPointValue = roundf(value * (1 << _fractionalBits));
}

// Copy constructor
Fixed::Fixed(const Fixed &src) {
    *this = src;
}

// Destructor
Fixed::~Fixed() {
}

// =============================================================================
// Assignment operator
// =============================================================================

Fixed &Fixed::operator=(const Fixed &that) {
    if (this != &that)
        this->_fixedPointValue = that.getRawBits();
    return *this;
}

// =============================================================================
// Comparison operators
// =============================================================================

bool Fixed::operator>(const Fixed &that) const {
    return this->_fixedPointValue > that._fixedPointValue;
}

bool Fixed::operator<(const Fixed &that) const {
    return this->_fixedPointValue < that._fixedPointValue;
}

bool Fixed::operator>=(const Fixed &that) const {
    return this->_fixedPointValue >= that._fixedPointValue;
}

bool Fixed::operator<=(const Fixed &that) const {
    return this->_fixedPointValue <= that._fixedPointValue;
}

bool Fixed::operator==(const Fixed &that) const {
    return this->_fixedPointValue == that._fixedPointValue;
}

bool Fixed::operator!=(const Fixed &that) const {
    return this->_fixedPointValue != that._fixedPointValue;
}

// =============================================================================
// Arithmetic operators
// =============================================================================

Fixed Fixed::operator+(const Fixed &that) const {
    Fixed result;
    result.setRawBits(this->_fixedPointValue + that._fixedPointValue);
    return result;
}

Fixed Fixed::operator-(const Fixed &that) const {
    Fixed result;
    result.setRawBits(this->_fixedPointValue - that._fixedPointValue);
    return result;
}

Fixed Fixed::operator*(const Fixed &that) const {
    // Multiplication needs to account for the fractional bits
    // x * y = (x * y) / (2^fractionalBits)
    Fixed result;

    long long temp = (long long)this->_fixedPointValue * (long long)that._fixedPointValue;
    result.setRawBits((int)(temp >> _fractionalBits));
    return result;
}

Fixed Fixed::operator/(const Fixed &that) const {
    // Division needs to account for the fractional bits
    // x / y = (x * 2^fractionalBits) / y
    Fixed result;

    if (that._fixedPointValue == 0) {
        std::cerr << "Error: Division by zero" << std::endl;
        return *this;
    }

    long long temp = ((long long)this->_fixedPointValue << _fractionalBits) / that._fixedPointValue;
	result.setRawBits((int)temp);
    return result;
}

// =============================================================================
// Increment/decrement operators
// =============================================================================

Fixed &Fixed::operator++() {
    ++this->_fixedPointValue;
    return *this;
}

Fixed Fixed::operator++(int) {
    Fixed temp = *this;
    ++(*this);
    return temp;
}

Fixed &Fixed::operator--() {
    --this->_fixedPointValue;
    return *this;
}

Fixed Fixed::operator--(int) {
    Fixed temp = *this;
    --(*this);
    return temp;
}

// =============================================================================
// Static min/max methods
// =============================================================================

Fixed &Fixed::min(Fixed &a, Fixed &b) {
	if (a < b)
		return a;
	return b;
}

const Fixed &Fixed::min(const Fixed &a, const Fixed &b) {
	if (a < b)
		return a;
	return b;
}

Fixed &Fixed::max(Fixed &a, Fixed &b) {
	if (a > b)
		return a;
	return b;
}

const Fixed &Fixed::max(const Fixed &a, const Fixed &b) {
	if (a > b)
		return a;
	return b;
}

// =============================================================================
// Member functions
// =============================================================================

float Fixed::toFloat(void) const {
    return static_cast<float>(_fixedPointValue) / (1 << _fractionalBits);
}

int Fixed::toInt(void) const {
    return _fixedPointValue >> _fractionalBits;
}

int Fixed::getRawBits(void) const {
    return this->_fixedPointValue;
}

void Fixed::setRawBits(int const raw) {
    this->_fixedPointValue = raw;
}

// =============================================================================
// Stream insertion operator
// =============================================================================

std::ostream &operator<<(std::ostream &out, const Fixed &fixed) {
    out << fixed.toFloat();
    return out;
}
