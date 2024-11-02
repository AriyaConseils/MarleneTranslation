#ifndef UIFILEPARSER_H
#define UIFILEPARSER_H

#include <string>
#include <vector>
#include <tinyxml2.h>

class UIFileParser {
public:
    struct UIFileData {
        std::string className;
        std::vector<std::string> strings;
    };

    explicit UIFileParser(const std::string& filePath);
    UIFileData parse() const;

private:
    std::string filePath;
    void parseElement(const tinyxml2::XMLElement* element, UIFileData& data) const ;

};

#endif // UIFILEPARSER_H
