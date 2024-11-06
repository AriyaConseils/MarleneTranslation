#include "translator.h"
#include "tinyxml2.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <regex>

using namespace tinyxml2;


Translator& Translator::instance() {
    static Translator instance;
    return instance;
}

std::string Translator::cleanFunctionSignature(const std::string& prettyFunction) {
    // Regex pour extraire le nom de la classe avant "::"
    std::regex classRegex(R"((\w+)::)");
    std::smatch match;

    if (std::regex_search(prettyFunction, match, classRegex) && match.size() > 1) {
        return match.str(1);  // Retourne le nom de la classe
    }
    return "UnknownClass";  // Valeur par défaut si non trouvée
}

std::vector<uint8_t> Translator::encryptData(const std::vector<uint8_t> &data, const std::string &key) {
    std::vector<uint8_t> encryptedData = data;
    for (size_t i = 0; i < data.size(); ++i) {
        encryptedData[i] = data[i] ^ key[i % key.size()];
    }
    return encryptedData;
}

std::vector<uint8_t> Translator::decryptData(const std::vector<uint8_t> &data, const std::string &key) {
    return encryptData(data, key);  // XOR est symétrique pour chiffrement/déchiffrement
}

bool Translator::convertTsToBin(const std::string &inputFilePath, const std::string &outputFilePath) {
    XMLDocument doc;
    if (doc.LoadFile(inputFilePath.c_str()) != XML_SUCCESS) {
        std::cerr << "Erreur : Impossible de lire le fichier TS.\n";
        return false;
    }

    std::vector<uint8_t> binaryData;

    // Parcourir chaque contexte dans le fichier XML
    XMLElement* contextElement = doc.FirstChildElement("TS")->FirstChildElement("context");
    while (contextElement) {
        // Lire le nom du contexte
        XMLElement* nameElement = contextElement->FirstChildElement("name");
        std::string contextName = nameElement && nameElement->GetText() ? nameElement->GetText() : "";

        // Parcourir chaque message dans le contexte
        XMLElement* messageElement = contextElement->FirstChildElement("message");
        while (messageElement) {
            // Lire la source et la traduction
            XMLElement* sourceElement = messageElement->FirstChildElement("source");
            XMLElement* translationElement = messageElement->FirstChildElement("translation");

            std::string sourceText = sourceElement && sourceElement->GetText() ? sourceElement->GetText() : "";
            std::string translation = translationElement && translationElement->GetText() ? translationElement->GetText() : "";

            // Calculer le hash
            uint64_t hash = generateHash(contextName, sourceText);

            // Affichage de débogage
            std::cout << "Converting: context = " << contextName
                      << ", source = " << sourceText
                      << ", translation = " << translation
                      << ", hash = " << hash << std::endl;

            uint32_t translationSize = static_cast<uint32_t>(translation.size());
            binaryData.insert(binaryData.end(), reinterpret_cast<uint8_t*>(&hash), reinterpret_cast<uint8_t*>(&hash) + sizeof(hash));
            binaryData.insert(binaryData.end(), reinterpret_cast<uint8_t*>(&translationSize), reinterpret_cast<uint8_t*>(&translationSize) + sizeof(translationSize));
            binaryData.insert(binaryData.end(), translation.begin(), translation.end());

            messageElement = messageElement->NextSiblingElement("message");
        }

        contextElement = contextElement->NextSiblingElement("context");
    }

    // Chiffrement des données
    std::vector<uint8_t> encryptedData = encryptData(binaryData, encryptionKey);

    // Sauvegarder le fichier binaire chiffré
    std::ofstream outputFile(outputFilePath, std::ios::binary);
    if (!outputFile.is_open()) {
        std::cerr << "Impossible de créer le fichier binaire.\n";
        return false;
    }
    outputFile.write(reinterpret_cast<const char*>(encryptedData.data()), encryptedData.size());
    outputFile.close();

    return true;
}


// Fonction de chargement et déchiffrement du fichier binaire
bool Translator::loadTranslations(std::string filePath)
{
    std::replace(filePath.begin(), filePath.end(), '/', '\\');
    std::filesystem::path path(filePath);

     // Conversion en chemin absolu pour éviter les erreurs
     if (!path.is_absolute()) {
         path = std::filesystem::absolute(path);
     }

     // Vérification si le fichier existe
     if (!std::filesystem::exists(path)) {
         std::cerr << "Le chemin du fichier est incorrect ou inaccessible : " << path << std::endl;
         return false;
     }

     std::ifstream file(path, std::ios::binary);
     if (!file.is_open()) {
         std::cerr << "Impossible d'ouvrir le fichier binaire de traduction.\n";
         return false;
     }

    std::vector<uint8_t> encryptedData((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();

    // Déchiffrement des données
    std::vector<uint8_t> decryptedData = decryptData(encryptedData, encryptionKey);
    translations.clear();

    size_t offset = 0;
    while (offset < decryptedData.size()) {
        uint64_t hash = *reinterpret_cast<const uint64_t*>(decryptedData.data() + offset);
        offset += sizeof(uint64_t);

        uint32_t translationSize = *reinterpret_cast<const uint32_t*>(decryptedData.data() + offset);
        offset += sizeof(uint32_t);

        std::string translation(reinterpret_cast<const char*>(decryptedData.data() + offset), translationSize);
        offset += translationSize;

        translations[hash] = translation;

        // Affichage de débogage
        std::cout << "Loaded: hash = " << hash
                  << ", translation = " << translation << std::endl;
    }

    return true;
}

// Génération de hash avec FNV-1a
uint64_t Translator::generateHash(const std::string &context, const std::string &source) {
    const uint64_t FNV_prime = 1099511628211u;
    uint64_t hash = 14695981039346656037u;

    std::string uniqueKey = context + "::" + source;
    for (char c : uniqueKey) {
        hash ^= static_cast<uint64_t>(c);
        hash *= FNV_prime;
    }
    return hash;
}

std::string Translator::getTranslation(const std::string &context, const std::string &source) {
    uint64_t hash = generateHash(context, source);

    // Affichage de débogage
    std::cout << "Searching: context = " << context
              << ", source = " << source
              << ", hash = " << hash << std::endl;

    auto it = translations.find(hash);

    if (it != translations.end()) {
        return it->second;
    }
    return source;
}
