#ifndef ARRAY_HPP
#define ARRAY_HPP

#include <exception>
#include <cstddef>

template <typename T>
class Array {
	private:
		T* _elements;
		unsigned int _size;

	public:
		Array() : _elements(NULL), _size(0) {}
		Array(unsigned int n) : _size(n) {
			if (n > 0) {
				_elements = new T[n]();
			} else {
				_elements = NULL;
			}
		}
		
		Array(Array const &src) : _elements(NULL), _size(0) {
			*this = src;
		}
		
		~Array() {
			if (_elements) {
				delete[] _elements;
			}
		}
		
		Array &operator=(Array const &rhs) {
			if (this != &rhs) {
				if (_elements) {
					delete[] _elements;
				}
				
				_size = rhs._size;
				
				if (_size > 0) {
					_elements = new T[_size]();
					for (unsigned int i = 0; i < _size; i++) {
						_elements[i] = rhs._elements[i];
					}
				} else {
					_elements = NULL;
				}
			}
			return *this;
		}
		
		// Subscript operator
		T &operator[](unsigned int index) {
			if (index >= _size) {
				throw std::exception();
			}
			return _elements[index];
		}
		
		// Const subscript operator
		const T &operator[](unsigned int index) const {
			if (index >= _size) {
				throw std::exception();
			}
			return _elements[index];
		}
		
		unsigned int size() const {
			return _size;
		}
};

#endif
