#include "SkylerWhiteYo.hpp"

int main(int argc, char **argv) {
    if (argc != 2)
    {
        std::cout << "Usage: " << argv[0] << " <literal>" << std::endl;
        return 1;
    }

	ScalarConverter::convert(argv[1]);

	// (void)argc;
	// (void)argv;
	// for (int i = 0; i < 128; i++)
	// {
	// 	std::stringstream ss;
	// 	ss << i;
	// 	std::string value = ss.str();
	// 	std::cout << "\nTesting value " << i << ":" << std::endl;
	// 	ScalarConverter::convert(value.c_str());
	// }
    return 0;
}