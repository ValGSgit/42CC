#include <iostream>
#include <vector>
#include <string>
#include <typeinfo>

// 1. Class Template with Template Operator Overloads
template <typename T>
class Container {
private:
    T* data;
    size_t capacity;
    size_t size;

public:
    // Constructor
    explicit Container(size_t cap = 10) : capacity(cap), size(0) {
        data = new T[capacity];
    }

    // Copy Constructor
    Container(const Container& other) : capacity(other.capacity), size(other.size) {
        data = new T[capacity];
        for (size_t i = 0; i < size; ++i) {
            data[i] = other.data[i];
        }
    }

    // Destructor
    ~Container() {
        delete[] data;
    }

    // Template Assignment Operator
    Container& operator=(const Container& other) {
        if (this != &other) {
            delete[] data;
            capacity = other.capacity;
            size = other.size;
            data = new T[capacity];
            for (size_t i = 0; i < size; ++i) {
                data[i] = other.data[i];
            }
        }
        return *this;
    }

    // Template Subscript Operators
    T& operator[](size_t index) {
        if (index >= size) {
            throw std::out_of_range("Index out of range");
        }
        return data[index];
    }

    const T& operator[](size_t index) const {
        if (index >= size) {
            throw std::out_of_range("Index out of range");
        }
        return data[index];
    }

    // Template Addition Operator (adds element)
    Container& operator+=(const T& element) {
        if (size >= capacity) {
            // Simple resize (double capacity)
            T* new_data = new T[capacity * 2];
            for (size_t i = 0; i < size; ++i) {
                new_data[i] = data[i];
            }
            delete[] data;
            data = new_data;
            capacity *= 2;
        }
        data[size++] = element;
        return *this;
    }

    // Template Equality Operator
    bool operator==(const Container& other) const {
        if (size != other.size) return false;
        for (size_t i = 0; i < size; ++i) {
            if (data[i] != other.data[i]) return false;
        }
        return true;
    }

    // Template Less Than Operator
    bool operator<(const Container& other) const {
        size_t min_size = (size < other.size) ? size : other.size;
        for (size_t i = 0; i < min_size; ++i) {
            if (data[i] < other.data[i]) return true;
            if (data[i] > other.data[i]) return false;
        }
        return size < other.size;
    }

    // Template Function Call Operator (functor)
    T& operator()(size_t index) {
        return (*this)[index];
    }

    // Getters
    size_t getSize() const { return size; }
    size_t getCapacity() const { return capacity; }

    // Template friend function for output
    template <typename U>
    friend std::ostream& operator<<(std::ostream& os, const Container<U>& container);
};

// 2. Non-member Template Operator Overload
template <typename T>
std::ostream& operator<<(std::ostream& os, const Container<T>& container) {
    os << "Container[";
    for (size_t i = 0; i < container.size; ++i) {
        os << container.data[i];
        if (i < container.size - 1) os << ", ";
    }
    os << "] (size: " << container.size << ", capacity: " << container.capacity << ")";
    return os;
}

// 3. Function Templates
template <typename T>
T maximum(const T& a, const T& b) {
    return (a > b) ? a : b;
}

// Function template with multiple template parameters
template <typename T, typename U>
auto add(const T& a, const U& b) -> decltype(a + b) {
    return a + b;
}

// 4. Template Specialization
template <>
class Container<bool> {
private:
    std::vector<bool> data;

public:
    explicit Container(size_t cap = 10) {
        data.reserve(cap);
    }

    Container& operator+=(bool element) {
        data.push_back(element);
        return *this;
    }

    bool operator[](size_t index) const {
        if (index >= data.size()) {
            throw std::out_of_range("Index out of range");
        }
        return data[index];
    }

    size_t getSize() const { return data.size(); }

    friend std::ostream& operator<<(std::ostream& os, const Container<bool>& container) {
        os << "BoolContainer[";
        for (size_t i = 0; i < container.data.size(); ++i) {
            os << (container.data[i] ? "true" : "false");
            if (i < container.data.size() - 1) os << ", ";
        }
        os << "]";
        return os;
    }
};

// 5. Template with Non-Type Parameters
template <typename T, size_t N>
class FixedArray {
private:
    T data[N];

public:
    // Template operator overloads
    T& operator[](size_t index) {
        if (index >= N) {
            throw std::out_of_range("Index out of range");
        }
        return data[index];
    }

    const T& operator[](size_t index) const {
        if (index >= N) {
            throw std::out_of_range("Index out of range");
        }
        return data[index];
    }

    constexpr size_t size() const { return N; }

    // Fill operator
    FixedArray& operator=(const T& value) {
        for (size_t i = 0; i < N; ++i) {
            data[i] = value;
        }
        return *this;
    }
};

// 6. Variadic Template
template <typename... Args>
void print(Args... args) {
    ((std::cout << args << " "), ...);  // C++17 fold expression
    std::cout << std::endl;
}

// 7. Template Metaprogramming Example
template <int N>
struct Factorial {
    static const int value = N * Factorial<N - 1>::value;
};

template <>
struct Factorial<0> {
    static const int value = 1;
};

// 8. SFINAE Example (Substitution Failure Is Not An Error)
template <typename T>
typename std::enable_if<std::is_arithmetic<T>::value, T>::type
safe_divide(const T& a, const T& b) {
    if (b == 0) {
        throw std::runtime_error("Division by zero");
    }
    return a / b;
}

int main() {
    std::cout << "=== Template Operator Overloads Demo ===" << std::endl;

    // 1. Testing Class Template with Operator Overloads
    std::cout << "\n1. Class Template with Operator Overloads:" << std::endl;
    Container<int> intContainer(5);
    intContainer += 10;
    intContainer += 20;
    intContainer += 30;
    
    std::cout << "Integer container: " << intContainer << std::endl;
    std::cout << "Element at index 1: " << intContainer[1] << std::endl;
    std::cout << "Using functor operator: " << intContainer(0) << std::endl;

    Container<std::string> stringContainer(3);
    stringContainer += "Hello";
    stringContainer += "World";
    stringContainer += "C++";
    
    std::cout << "String container: " << stringContainer << std::endl;

    // Testing equality operator
    Container<int> anotherIntContainer(5);
    anotherIntContainer += 10;
    anotherIntContainer += 20;
    anotherIntContainer += 30;
    
    std::cout << "Containers equal? " << (intContainer == anotherIntContainer) << std::endl;

    // 2. Function Templates
    std::cout << "\n2. Function Templates:" << std::endl;
    std::cout << "Maximum of 5 and 10: " << maximum(5, 10) << std::endl;
    std::cout << "Maximum of 3.14 and 2.71: " << maximum(3.14, 2.71) << std::endl;
    std::cout << "Adding int and double: " << add(5, 3.14) << std::endl;

    // 3. Template Specialization
    std::cout << "\n3. Template Specialization:" << std::endl;
    Container<bool> boolContainer;
    boolContainer += true;
    boolContainer += false;
    boolContainer += true;
    std::cout << "Bool container: " << boolContainer << std::endl;

    // 4. Template with Non-Type Parameters
    std::cout << "\n4. Template with Non-Type Parameters:" << std::endl;
    FixedArray<int, 5> fixedArr;
    fixedArr = 42;  // Fill with 42
    fixedArr[0] = 100;
    std::cout << "Fixed array size: " << fixedArr.size() << std::endl;
    std::cout << "Fixed array elements: ";
    for (size_t i = 0; i < fixedArr.size(); ++i) {
        std::cout << fixedArr[i] << " ";
    }
    std::cout << std::endl;

    // 5. Variadic Templates
    std::cout << "\n5. Variadic Templates:" << std::endl;
    print("Hello", 42, 3.14, "World");

    // 6. Template Metaprogramming
    std::cout << "\n6. Template Metaprogramming:" << std::endl;
    std::cout << "Factorial of 5: " << Factorial<5>::value << std::endl;

    // 7. SFINAE
    std::cout << "\n7. SFINAE (only works with arithmetic types):" << std::endl;
    std::cout << "10 / 3 = " << safe_divide(10, 3) << std::endl;
    std::cout << "10.5 / 2.5 = " << safe_divide(10.5, 2.5) << std::endl;

    // 8. Template Type Deduction
    std::cout << "\n8. Template Type Deduction:" << std::endl;
    auto result1 = maximum(5, 10);          // T deduced as int
    auto result2 = maximum(3.14, 2.71);     // T deduced as double
    std::cout << "Deduced int result: " << result1 << std::endl;
    std::cout << "Deduced double result: " << result2 << std::endl;

    return 0;
}
