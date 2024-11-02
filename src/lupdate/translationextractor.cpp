#include "translationextractor.h"
#include <tinyxml2.h>
#include <filesystem>
#include <iostream>
#include <algorithm>

namespace fs = std::filesystem;
using namespace tinyxml2;

TranslationExtractor::TranslationExtractor(const std::string& rootPath) : rootPath(rootPath)
{
    m_languageRegionMap = {
        { "Afrikaans", "af_ZA" },       // South Africa
        { "Albanian", "sq_AL" },        // Albania
        { "Amharic", "am_ET" },         // Ethiopia
        { "Arabic", "ar_SA" },          // Saudi Arabia
        { "Armenian", "hy_AM" },        // Armenia
        { "Azerbaijani", "az_AZ" },     // Azerbaijan
        { "Basque", "eu_ES" },          // Spain
        { "Belarusian", "be_BY" },      // Belarus
        { "Bengali", "bn_BD" },         // Bangladesh
        { "Bosnian", "bs_BA" },         // Bosnia and Herzegovina
        { "Bulgarian", "bg_BG" },       // Bulgaria
        { "Catalan", "ca_ES" },         // Spain
        { "Cebuano", "ceb_PH" },        // Philippines
        { "Chinese", "zh_CN" },         // China
        { "Corsican", "co_FR" },        // France
        { "Croatian", "hr_HR" },        // Croatia
        { "Czech", "cs_CZ" },           // Czech Republic
        { "Danish", "da_DK" },          // Denmark
        { "Dutch", "nl_NL" },           // Netherlands
        { "English", "en_US" },         // United States
        { "Esperanto", "eo_EO" },       // Esperanto (International)
        { "Estonian", "et_EE" },        // Estonia
        { "Filipino", "fil_PH" },       // Philippines
        { "Finnish", "fi_FI" },         // Finland
        { "French", "fr_FR" },          // France
        { "Frisian", "fy_NL" },         // Netherlands
        { "Galician", "gl_ES" },        // Spain
        { "Georgian", "ka_GE" },        // Georgia
        { "German", "de_DE" },          // Germany
        { "Greek", "el_GR" },           // Greece
        { "Gujarati", "gu_IN" },        // India
        { "Haitian Creole", "ht_HT" },  // Haiti
        { "Hausa", "ha_NG" },           // Nigeria
        { "Hawaiian", "haw_US" },       // United States
        { "Hebrew", "he_IL" },          // Israel
        { "Hindi", "hi_IN" },           // India
        { "Hmong", "hmn_CN" },          // China
        { "Hungarian", "hu_HU" },       // Hungary
        { "Icelandic", "is_IS" },       // Iceland
        { "Igbo", "ig_NG" },            // Nigeria
        { "Indonesian", "id_ID" },      // Indonesia
        { "Irish", "ga_IE" },           // Ireland
        { "Italian", "it_IT" },         // Italy
        { "Japanese", "ja_JP" },        // Japan
        { "Javanese", "jv_ID" },        // Indonesia
        { "Kannada", "kn_IN" },         // India
        { "Kazakh", "kk_KZ" },          // Kazakhstan
        { "Khmer", "km_KH" },           // Cambodia
        { "Kinyarwanda", "rw_RW" },     // Rwanda
        { "Korean", "ko_KR" },          // South Korea
        { "Kurdish", "ku_TR" },         // Turkey
        { "Kyrgyz", "ky_KG" },          // Kyrgyzstan
        { "Lao", "lo_LA" },             // Laos
        { "Latin", "la_VA" },           // Vatican
        { "Latvian", "lv_LV" },         // Latvia
        { "Lithuanian", "lt_LT" },      // Lithuania
        { "Luxembourgish", "lb_LU" },   // Luxembourg
        { "Macedonian", "mk_MK" },      // North Macedonia
        { "Malagasy", "mg_MG" },        // Madagascar
        { "Malay", "ms_MY" },           // Malaysia
        { "Malayalam", "ml_IN" },       // India
        { "Maltese", "mt_MT" },         // Malta
        { "Maori", "mi_NZ" },           // New Zealand
        { "Marathi", "mr_IN" },         // India
        { "Mongolian", "mn_MN" },       // Mongolia
        { "Myanmar (Burmese)", "my_MM" }, // Myanmar
        { "Nepali", "ne_NP" },          // Nepal
        { "Norwegian", "no_NO" },       // Norway
        { "Nyanja (Chichewa)", "ny_MW" }, // Malawi
        { "Odia (Oriya)", "or_IN" },    // India
        { "Pashto", "ps_AF" },          // Afghanistan
        { "Persian", "fa_IR" },         // Iran
        { "Polish", "pl_PL" },          // Poland
        { "Portuguese", "pt_BR" },      // Brazil
        { "Punjabi", "pa_IN" },         // India
        { "Romanian", "ro_RO" },        // Romania
        { "Russian", "ru_RU" },         // Russia
        { "Samoan", "sm_WS" },          // Samoa
        { "Scots Gaelic", "gd_GB" },    // United Kingdom
        { "Serbian", "sr_RS" },         // Serbia
        { "Sesotho", "st_LS" },         // Lesotho
        { "Shona", "sn_ZW" },           // Zimbabwe
        { "Sindhi", "sd_PK" },          // Pakistan
        { "Sinhala", "si_LK" },         // Sri Lanka
        { "Slovak", "sk_SK" },          // Slovakia
        { "Slovenian", "sl_SI" },       // Slovenia
        { "Somali", "so_SO" },          // Somalia
        { "Spanish", "es_ES" },         // Spain
        { "Sundanese", "su_ID" },       // Indonesia
        { "Swahili", "sw_KE" },         // Kenya
        { "Swedish", "sv_SE" },         // Sweden
        { "Tajik", "tg_TJ" },           // Tajikistan
        { "Tamil", "ta_IN" },           // India
        { "Tatar", "tt_RU" },           // Russia
        { "Telugu", "te_IN" },          // India
        { "Thai", "th_TH" },            // Thailand
        { "Turkish", "tr_TR" },         // Turkey
        { "Turkmen", "tk_TM" },         // Turkmenistan
        { "Ukrainian", "uk_UA" },       // Ukraine
        { "Urdu", "ur_PK" },            // Pakistan
        { "Uyghur", "ug_CN" },          // China
        { "Uzbek", "uz_UZ" },           // Uzbekistan
        { "Vietnamese", "vi_VN" },      // Vietnam
        { "Welsh", "cy_GB" },           // United Kingdom
        { "Xhosa", "xh_ZA" },           // South Africa
        { "Yiddish", "yi_001" },        // World (Global)
        { "Yoruba", "yo_NG" },          // Nigeria
        { "Zulu", "zu_ZA" }             // South Africa
    };
}

void TranslationExtractor::load() {
    for (const auto& entry : fs::recursive_directory_iterator(rootPath)) {
        if (entry.is_regular_file()) {
            std::string extension = entry.path().extension().string();
            std::string fileName = entry.path().filename().string();

            // Vérifie que le fichier est un .cpp ou .ui et ne commence pas par "moc_" ou "qrc_"
            if (fileName.rfind("moc_", 0) == 0 || fileName.rfind("qrc_", 0) == 0) {
                std::cout << "Fichier ignoré : " << fileName << std::endl;
                continue;
            }

            if (extension == ".cpp") {
                CPPFileParser cppParser(entry.path().string());
                std::vector<CPPFileParser::CPPFileData> cppData = cppParser.parse();
                mergePatterns(cppData);
            } else if (extension == ".ui") {
                UIFileParser uiParser(entry.path().string());
                UIFileParser::UIFileData uiData = uiParser.parse();
                mergePatterns(uiData);
            }

        }
    }
}

void TranslationExtractor::mergePatterns(const std::vector<CPPFileParser::CPPFileData>& cppData) {
    for (const auto& data : cppData) {
        for (const auto& str : data.strings) {
            patterns[data.className].insert(str);
        }
    }
}

void TranslationExtractor::mergePatterns(const UIFileParser::UIFileData& uiData) {
    for (const auto& str : uiData.strings) {
        patterns[uiData.className].insert(str);
    }
}

void TranslationExtractor::setLanguage(const std::string &newLanguage)
{
    m_language = newLanguage;
}

std::string TranslationExtractor::getAssociatedCode(const std::string &language)
{
    return m_languageRegionMap.at(language);
}

void TranslationExtractor::saveToTSFile(const std::string& outputPath) {
    XMLDocument doc;
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> mergedTranslations;

    int newSourceCount = 0, existingSourceCount = 0, existingTranslationCount = 0;
    std::string language;  // Variable pour stocker la langue si elle existe

    // Charger le fichier TS existant
    if (doc.LoadFile(outputPath.c_str()) == XML_SUCCESS) {
        XMLElement* tsElement = doc.FirstChildElement("TS");
        if (tsElement) {
            // Vérifie et sauvegarde l'attribut de langue si présent
            const char* langAttr = tsElement->Attribute("language");
            if (langAttr) {
                language = langAttr;
            }

            for (XMLElement* contextElement = tsElement->FirstChildElement("context"); contextElement; contextElement = contextElement->NextSiblingElement("context")) {
                std::string contextName = contextElement->FirstChildElement("name")->GetText();

                for (XMLElement* messageElement = contextElement->FirstChildElement("message"); messageElement; messageElement = messageElement->NextSiblingElement("message")) {
                    const char* sourceText = messageElement->FirstChildElement("source")->GetText();
                    const char* translationText = messageElement->FirstChildElement("translation")->GetText();
                    if (sourceText) {
                        std::string sourceStr = sourceText;
                        std::string translationStr = translationText ? translationText : "";
                        mergedTranslations[contextName][sourceStr] = translationStr;

                        // Compte les traductions non vides uniquement pour les sources existantes
                        if (!translationStr.empty()) {
                            existingTranslationCount++;
                        }
                    }
                }
            }
        }
    }

    // Ajout des nouvelles `source` aux données fusionnées sans écraser les existantes
    for (const auto& [context, sources] : patterns) {
        for (const auto& source : sources) {
            if (mergedTranslations[context].find(source) == mergedTranslations[context].end()) {
                // Nouvelle source détectée
                mergedTranslations[context][source] = "";  // Nouvelle entrée avec traduction vide
                newSourceCount++;
            } else {
                // Source existante détectée
                existingSourceCount++;
            }
        }
    }

    // Créer un nouveau document XML pour enregistrer les données triées
    doc.Clear();
    doc.InsertEndChild(doc.NewDeclaration("xml version=\"1.0\" encoding=\"UTF-8\""));
    XMLUnknown* doctype = doc.NewUnknown("DOCTYPE TS");
    doc.InsertEndChild(doctype);
    XMLElement* tsElement = doc.NewElement("TS");
    tsElement->SetAttribute("version", "2.1");

    // Appliquer l'attribut de langue si existant
    if (!language.empty() || !m_language.empty()) {
        tsElement->SetAttribute("language", (!m_language.empty())?getAssociatedCode(m_language).c_str():language.c_str());
    }

    doc.InsertEndChild(tsElement);

    // Trie les contextes et sources par ordre alphabétique
    std::vector<std::string> sortedContexts;
    for (const auto& [context, _] : mergedTranslations) {
        sortedContexts.push_back(context);
    }
    std::sort(sortedContexts.begin(), sortedContexts.end());

    for (const auto& context : sortedContexts) {
        XMLElement* contextElement = doc.NewElement("context");
        XMLElement* nameElement = doc.NewElement("name");
        nameElement->SetText(context.c_str());
        contextElement->InsertEndChild(nameElement);

        std::vector<std::string> sortedSources;
        for (const auto& [source, _] : mergedTranslations[context]) {
            sortedSources.push_back(source);
        }
        std::sort(sortedSources.begin(), sortedSources.end());

        for (const auto& source : sortedSources) {
            XMLElement* messageElement = doc.NewElement("message");
            XMLElement* sourceElement = doc.NewElement("source");
            sourceElement->SetText(source.c_str());
            messageElement->InsertEndChild(sourceElement);

            XMLElement* translationElement = doc.NewElement("translation");
            translationElement->SetText(mergedTranslations[context][source].c_str());
            messageElement->InsertEndChild(translationElement);

            contextElement->InsertEndChild(messageElement);
        }
        tsElement->InsertEndChild(contextElement);
    }

    // Sauvegarder le fichier fusionné et trié
    if (doc.SaveFile(outputPath.c_str()) != XML_SUCCESS) {
        std::cerr << "Erreur: impossible de sauvegarder le fichier TS : " << outputPath << std::endl;
    } else {
        std::cout << "Extraction terminée.\n"
                  << "Nouvelles sources ajoutées : " << newSourceCount << "\n"
                  << "Sources déjà existantes : " << existingSourceCount << "\n"
                  << "Traductions déjà existantes : " << existingTranslationCount << std::endl;
    }
}

