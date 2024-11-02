#include "CPPFileParser.h"
#include <fstream>
#include <iostream>
#include <regex>

CPPFileParser::CPPFileParser(const std::string& filePath) : filePath(filePath) {}

std::vector<CPPFileParser::CPPFileData> CPPFileParser::parse() {
    std::vector<CPPFileData> dataList;

    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Erreur: impossible d'ouvrir le fichier : " << filePath << std::endl;
        return dataList;
    }

    std::string fileContent((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();

    std::regex functionRegex("(\\w+::)?(\\w+)\\s*\\([^)]*\\)\\s*(const)?\\s*(:\\s*[^\\{]*)?\\{");
    std::regex gtrOrTrRegex("tr\\s*\\(\\s*\"([^\"]*)\"\\s*\\)|gtr\\s*\\(\\s*\"([^\"]*)\"\\s*\\)");

    std::smatch match;
    std::string currentClass;
    auto searchStart = fileContent.cbegin();

    while (searchStart != fileContent.cend()) {
        if (std::regex_search(searchStart, fileContent.cend(), match, functionRegex)) {
            // Extraire le nom de la fonction et définir le contexte
            std::string functionName = match[2].str();
            currentClass = match[1].str();
            currentClass = std::regex_replace(currentClass, std::regex("::"), ""); // Nettoyer le nom de la classe

            CPPFileData data;
            data.className = currentClass;
            std::string fullContext = currentClass + "::" + functionName;
            std::cout << "  Fonction détectée : " << fullContext << std::endl;

            // Début du bloc de la fonction
            size_t funcStart = std::distance(fileContent.cbegin(), match.suffix().first);
            size_t pos = funcStart;
            int braceCount = 1;

            // Suivi des accolades pour délimiter la fonction
            while (braceCount > 0 && pos < fileContent.size()) {
                if (fileContent[pos] == '{') braceCount++;
                else if (fileContent[pos] == '}') braceCount--;
                pos++;
            }

            // Contenu de la fonction
            std::string functionContent = fileContent.substr(funcStart, pos - funcStart);
            auto funcSearchStart = functionContent.cbegin();

            // Recherche des balises tr et gtr dans le contenu de la fonction
            while (std::regex_search(funcSearchStart, functionContent.cend(), match, gtrOrTrRegex)) {
                std::string sourceText = match[1].matched ? match[1].str() : match[2].str();
                data.strings.push_back(sourceText);
                std::cout << "    Balise trouvée : \"" << sourceText << "\" dans le contexte " << fullContext << std::endl;
                funcSearchStart = match.suffix().first;
            }

            dataList.push_back(data); // Ajoute les données pour cette fonction
            searchStart = fileContent.cbegin() + pos; // Avancer au-delà de la fonction
        } else {
            break;
        }
    }

    return dataList;
}



