#ifndef TRANSLATOR_H
#define TRANSLATOR_H

#include <string>
#include <unordered_map>
#include <vector>

// Instance Singleton pour le chargeur de traduction
#define LOAD_TRANSLATIONS(filePath) Translator::instance().loadTranslations(filePath)

// Conversion de .ts vers .qm (ou format binaire de traduction)
#define CONVERT_TS_TO_QM(inputFile, outputFile) Translator::instance().convertTsToBin(inputFile, outputFile)

class Translator {
public:
    bool convertTsToBin(const std::string &inputFilePath, const std::string &outputFilePath);
    bool loadTranslations(std::string filePath);
    std::string getTranslation(const std::string &context, const std::string &source);

    static Translator& instance();  // Singleton pour accéder à l'instance globale

    // Fonction pour nettoyer le contenu de __PRETTY_FUNCTION__ ou __FUNCSIG__
    static std::string cleanFunctionSignature(const std::string& prettyFunction);

private:
    std::unordered_map<uint64_t, std::string> translations;

    uint64_t generateHash(const std::string &context, const std::string &source);

    std::vector<uint8_t> encryptData(const std::vector<uint8_t> &data, const std::string &key);
    std::vector<uint8_t> decryptData(const std::vector<uint8_t> &data, const std::string &key);

    const std::string encryptionKey = "Translator_Secret_key";  // Clé de chiffrement symétrique

    Translator() = default;  // Constructeur privé pour le singleton
};

// Macro `gtr` pour utiliser la traduction avec le nom de la classe nettoyé
#define gtr(text) Translator::instance().getTranslation(Translator::cleanFunctionSignature(__PRETTY_FUNCTION__), text)
#define gtrC(context, text) Translator::instance().getTranslation(context, text)

#endif // TRANSLATOR_H
