#ifndef FIXED_HPP
# define FIXED_HPP

#include <iostream>
#include <cmath>

class Fixed
{
	private:
		int _fixedPointValue;
		static const int _fractionalBits = 8;

	public:
		Fixed();
		Fixed(const int value);
		Fixed(const float value);
		Fixed(const Fixed &src);
		~Fixed();
		
		Fixed &operator=(const Fixed &that);
		
		bool operator>(const Fixed &that) const;
		bool operator<(const Fixed &that) const;
		bool operator>=(const Fixed &that) const;
		bool operator<=(const Fixed &that) const;
		bool operator==(const Fixed &that) const;
		bool operator!=(const Fixed &that) const;
		
		Fixed operator+(const Fixed &that) const;
		Fixed operator-(const Fixed &that) const;
		Fixed operator*(const Fixed &that) const;
		Fixed operator/(const Fixed &that) const;

		Fixed &operator++();
		Fixed operator++(int);
		Fixed &operator--();
		Fixed operator--(int);
		
		static Fixed &min(Fixed &a, Fixed &b);
		static const Fixed &min(const Fixed &a, const Fixed &b);
		static Fixed &max(Fixed &a, Fixed &b);
		static const Fixed &max(const Fixed &a, const Fixed &b);

		float toFloat(void) const;
		int toInt(void) const;
		int getRawBits(void) const;
		void setRawBits(int const raw);
};

std::ostream &operator<<(std::ostream &out, const Fixed &fixed);

#endif
