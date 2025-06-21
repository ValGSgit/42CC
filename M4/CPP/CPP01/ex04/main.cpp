// Turn-in directory : ex04/
// Files to turn in : Makefile, main.cpp, *.cpp, *.{h, hpp}
// Forbidden functions : std::string::replace
// Create a program that takes three parameters in the following order: a filename and
// two strings, s1 and s2.
// It must open the file <filename> and copy its content into a new file
// <filename>.replace, replacing every occurrence of s1 with s2.
// Using C file manipulation functions is forbidden and will be considered cheating. All
// the member functions of the class std::string are allowed, except replace. Use them
// wisely!
// Of course, handle unexpected inputs and errors. You must create and turn in your
// own tests to ensure that your program works as expected.

#include <iostream>
#include <fstream>
#include <string>

std::string customReplace(const std::string& content, const std::string& s1, const std::string& s2)
{
    std::string result = content;

    if (s1.empty()) {
        return result;
    }

    size_t pos = 0;
	pos = result.find(s1, pos);
	//Note: std::string::npos is special constant that means 'no position' or 'not found'
    while (pos != std::string::npos)
	{
        result.erase(pos, s1.length());
        result.insert(pos, s2);
        pos += s2.length();
		pos = result.find(s1, pos);
    }
    return result;
}

int validateInput(int ac, char **av, std::string &filename, std::string &s1, std::string &s2)
{
    if (ac != 4) {
        std::cerr << "Error: Usage: " << av[0] << " <filename> <s1> <s2>" << std::endl;
        return 1;
    }
    
    filename = std::string(av[1]);
    s1 = std::string(av[2]);
    s2 = std::string(av[3]);
    
    if (filename.empty()) {
        std::cerr << "Error: Filename cannot be empty." << std::endl;
        return 1;
    }
    if (s1.empty()) {
        std::cerr << "Error: s1 cannot be empty." << std::endl;
        return 1;
    }
	if (s2.empty()){
		std::cerr << "Error: Nothing to replace... no need to execute :)" << std::endl;
	}
    return 0;
}

int main(int ac, char* av[])
{
	std::string filename, s1, s2;

	if (validateInput(ac, av, filename, s1, s2) != 0)
		return (1);

    std::ifstream inFile(filename.c_str());
    if (!inFile.is_open()) {
        std::cerr << "Error: Unable to open input file '" << filename << "'." << std::endl;
        return 1;
    }
    std::string content;
    std::string line;
    while (std::getline(inFile, line))
	{
        content += line;
        if (!inFile.eof())
		{
            content += "\n"; //Add newline while not eof
        }
    }
    inFile.close();
    std::string outFilename = filename + ".replace";
    std::ofstream outFile(outFilename.c_str());
    if (!outFile.is_open()) {
        std::cerr << "Error: Open failed to create the file: '" << outFilename << "'." << std::endl;
        return 1;
    }
    std::string modifiedContent = customReplace(content, s1, s2);
    outFile << modifiedContent;
    outFile.close();
	std::cout << "Successfully replaced all occurances of '" << s1 << "' with '" << s2 << "'" << std::endl ;
    std::cout << "Output written to '" << outFilename << "'." << std::endl;
    return 0;
}