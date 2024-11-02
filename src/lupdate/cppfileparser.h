#ifndef CPPFILEPARSER_H
#define CPPFILEPARSER_H

#include <string>
#include <vector>

class CPPFileParser {
public:
    struct CPPFileData {
        std::string className;
        std::vector<std::string> strings;
    };

    explicit CPPFileParser(const std::string& filePath);
    std::vector<CPPFileData> parse();  // Change to return a vector of CPPFileData

private:
    std::string filePath;

};

#endif // CPPFILEPARSER_H
