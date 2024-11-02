#ifndef TRANSLATIONEXTRACTOR_H
#define TRANSLATIONEXTRACTOR_H

#include "CPPFileParser.h"
#include "UIFileParser.h"
#include <string>
#include <unordered_map>
#include <unordered_set>

class TranslationExtractor {
public:
    explicit TranslationExtractor(const std::string& rootPath);
    void load();
    void saveToTSFile(const std::string& outputPath);

    void setLanguage(const std::string &newLanguage);
    std::string getAssociatedCode(const std::string &language);

private:
    std::string rootPath;
    std::unordered_map<std::string, std::unordered_set<std::string>> patterns;

    void mergePatterns(const std::vector<CPPFileParser::CPPFileData>& cppData);
    void mergePatterns(const UIFileParser::UIFileData& uiData);
    std::string m_language;
    std::unordered_map<std::string, std::string> m_languageRegionMap;
};

#endif // TRANSLATIONEXTRACTOR_H
