#include "UIFileParser.h"
#include <tinyxml2.h>
#include <iostream>

using namespace tinyxml2;

UIFileParser::UIFileParser(const std::string& filePath) : filePath(filePath) {}

UIFileParser::UIFileData UIFileParser::parse() const {
    UIFileData data;
    XMLDocument doc;

    // Chargement du fichier .ui
    std::cout << "Chargement du fichier .ui : " << filePath << std::endl;
    if (doc.LoadFile(filePath.c_str()) != XML_SUCCESS) {
        std::cerr << "Erreur: impossible de lire le fichier .ui : " << filePath << std::endl;
        return data;
    }

    // Récupération du nom de la classe
    XMLElement* classElement = doc.FirstChildElement("ui")->FirstChildElement("class");
    if (classElement) {
        data.className = classElement->GetText();
        std::cout << "Nom de la classe trouvé : " << data.className << std::endl;
    } else {
        std::cerr << "Erreur: élément <class> non trouvé dans le fichier .ui." << std::endl;
        return data;
    }

    // Démarrer le parcours depuis le premier élément <ui>
    XMLElement* rootElement = doc.FirstChildElement("ui");
    if (rootElement) {
        parseElement(rootElement, data); // Appel récursif pour tous les éléments pertinents
    }

    std::cout << "Analyse terminée pour le fichier : " << filePath << std::endl;
    return data;
}

// Fonction récursive pour parcourir tous les éléments potentiellement intéressants
void UIFileParser::parseElement(const XMLElement* element, UIFileData& data) const {
    std::string elementName = element->Name();
    const char* elementAttr = element->Attribute("name");

    // Affiche le type et le nom de chaque élément pour le suivi
    std::cout << "Traitement de l'élément : " << elementName
              << (elementAttr ? std::string(" (name=") + elementAttr + ")" : "")
              << std::endl;

    // Parcourir toutes les propriétés de l'élément pour les balises <string>
    for (const XMLElement* propElement = element->FirstChildElement("property");
         propElement;
         propElement = propElement->NextSiblingElement("property")) {

        const char* propName = propElement->Attribute("name");

        // Recherche d'un élément <string> dans la propriété
        const XMLElement* stringElement = propElement->FirstChildElement("string");
        if (stringElement) {
            const char* notrAttr = stringElement->Attribute("notr");
            const char* text = stringElement->GetText();

            // Ajouter la chaîne si "notr" n'est pas "true"
            if ((!notrAttr || std::string(notrAttr) != "true") && text) {
                std::cout << "  Propriété détectée : " << (propName ? propName : "(anonyme)") << std::endl;
                std::cout << "    Chaîne trouvée : \"" << text << "\"" << std::endl;
                data.strings.push_back(text);
            } else if (notrAttr && std::string(notrAttr) == "true") {
                std::cout << "    Chaîne ignorée (notr=\"true\")" << std::endl;
            }
        }
    }

    // Parcourir toutes les attributs de l'élément pour les balises <string>
    for (const XMLElement* attrElement = element->FirstChildElement("attribute");
         attrElement;
         attrElement = attrElement->NextSiblingElement("attribute")) {

        // Recherche d'un élément <string> dans l'attribut
        const XMLElement* stringElement = attrElement->FirstChildElement("string");
        if (stringElement) {
            const char* notrAttr = stringElement->Attribute("notr");
            const char* text = stringElement->GetText();

            // Ajouter la chaîne si "notr" n'est pas "true"
            if ((!notrAttr || std::string(notrAttr) != "true") && text) {
                data.strings.push_back(text);
            } else if (notrAttr && std::string(notrAttr) == "true") {

            }
        }
    }

    // Récursion pour tous les enfants de cet élément
    for (const XMLElement* childElement = element->FirstChildElement();
         childElement;
         childElement = childElement->NextSiblingElement()) {

        parseElement(childElement, data); // Appel récursif pour chaque enfant
    }
}
