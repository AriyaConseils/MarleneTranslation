#include "TSTranslationReader.h"
#include <QFile>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QDebug>
#include "chatgptclient.h"
#include "FileAggregator.h"
#include <QEventLoop>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>




TSTranslationReader::TSTranslationReader(QObject *parent)
    : QObject(parent)
{
    m_languageMap = {
        { "Afrikaans", "af" },
        { "Albanian", "sq" },
        { "Amharic", "am" },
        { "Arabic", "ar" },
        { "Armenian", "hy" },
        { "Azerbaijani", "az" },
        { "Basque", "eu" },
        { "Belarusian", "be" },
        { "Bengali", "bn" },
        { "Bosnian", "bs" },
        { "Bulgarian", "bg" },
        { "Catalan", "ca" },
        { "Cebuano", "ceb" },
        { "Chinese", "zh" },
        { "Corsican", "co" },
        { "Croatian", "hr" },
        { "Czech", "cs" },
        { "Danish", "da" },
        { "Dutch", "nl" },
        { "English", "en" },
        { "Esperanto", "eo" },
        { "Estonian", "et" },
        { "Filipino", "fil" },
        { "Finnish", "fi" },
        { "French", "fr" },
        { "Frisian", "fy" },
        { "Galician", "gl" },
        { "Georgian", "ka" },
        { "German", "de" },
        { "Greek", "el" },
        { "Gujarati", "gu" },
        { "Haitian Creole", "ht" },
        { "Hausa", "ha" },
        { "Hawaiian", "haw" },
        { "Hebrew", "he" },
        { "Hindi", "hi" },
        { "Hmong", "hmn" },
        { "Hungarian", "hu" },
        { "Icelandic", "is" },
        { "Igbo", "ig" },
        { "Indonesian", "id" },
        { "Irish", "ga" },
        { "Italian", "it" },
        { "Japanese", "ja" },
        { "Javanese", "jv" },
        { "Kannada", "kn" },
        { "Kazakh", "kk" },
        { "Khmer", "km" },
        { "Kinyarwanda", "rw" },
        { "Korean", "ko" },
        { "Kurdish", "ku" },
        { "Kyrgyz", "ky" },
        { "Lao", "lo" },
        { "Latin", "la" },
        { "Latvian", "lv" },
        { "Lithuanian", "lt" },
        { "Luxembourgish", "lb" },
        { "Macedonian", "mk" },
        { "Malagasy", "mg" },
        { "Malay", "ms" },
        { "Malayalam", "ml" },
        { "Maltese", "mt" },
        { "Maori", "mi" },
        { "Marathi", "mr" },
        { "Mongolian", "mn" },
        { "Myanmar (Burmese)", "my" },
        { "Nepali", "ne" },
        { "Norwegian", "no" },
        { "Nyanja (Chichewa)", "ny" },
        { "Odia (Oriya)", "or" },
        { "Pashto", "ps" },
        { "Persian", "fa" },
        { "Polish", "pl" },
        { "Portuguese", "pt" },
        { "Punjabi", "pa" },
        { "Romanian", "ro" },
        { "Russian", "ru" },
        { "Samoan", "sm" },
        { "Scots Gaelic", "gd" },
        { "Serbian", "sr" },
        { "Sesotho", "st" },
        { "Shona", "sn" },
        { "Sindhi", "sd" },
        { "Sinhala", "si" },
        { "Slovak", "sk" },
        { "Slovenian", "sl" },
        { "Somali", "so" },
        { "Spanish", "es" },
        { "Sundanese", "su" },
        { "Swahili", "sw" },
        { "Swedish", "sv" },
        { "Tajik", "tg" },
        { "Tamil", "ta" },
        { "Tatar", "tt" },
        { "Telugu", "te" },
        { "Thai", "th" },
        { "Turkish", "tr" },
        { "Turkmen", "tk" },
        { "Ukrainian", "uk" },
        { "Urdu", "ur" },
        { "Uyghur", "ug" },
        { "Uzbek", "uz" },
        { "Vietnamese", "vi" },
        { "Welsh", "cy" },
        { "Xhosa", "xh" },
        { "Yiddish", "yi" },
        { "Yoruba", "yo" },
        { "Zulu", "zu" }
    };

}


bool TSTranslationReader::load(const QString &filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }
    translations.clear();

    QXmlStreamReader xml(&file);
    QString currentContext;

    // Lecture de l'attribut `language` dans l'élément racine <TS>
    if (xml.readNextStartElement()) {
        if (xml.name() == QStringLiteral("TS")) {
            if (xml.attributes().hasAttribute("language")) {
                setLanguageCode(xml.attributes().value("language").toString());
                qDebug() << "Language code found:" << m_languageCode;
            } else {
                setLanguageCode("");
            }
        }
    }

    while (!xml.atEnd() && !xml.hasError()) {
        xml.readNext();

        if (xml.isStartElement()) {
            if (xml.name().compare("context", Qt::CaseSensitive) == 0) {
                currentContext.clear();
            } else if (xml.name().compare("name", Qt::CaseSensitive) == 0 && currentContext.isEmpty()) {
                currentContext = xml.readElementText();
            } else if (xml.name().compare("message", Qt::CaseSensitive) == 0) {
                TranslationMessage message;
                while (!(xml.isEndElement() && xml.name().compare("message", Qt::CaseSensitive) == 0)) {
                    xml.readNext();

                    if (xml.isStartElement()) {
                        if (xml.name().compare("source", Qt::CaseSensitive) == 0) {
                            message.source = xml.readElementText();
                        } else if (xml.name().compare("translation", Qt::CaseSensitive) == 0) {
                            message.translation = xml.readElementText();
                        }
                    }
                }
                translations[currentContext].append(message);
            }
        }
    }

    file.close();
    return !xml.hasError();
}

QMap<QString, QVector<TranslationMessage>> TSTranslationReader::getTranslations() const {
    return translations;
}


bool TSTranslationReader::updateTranslation(const QString &context, const QString &source, const QString &newTranslation) {
    if (translations.contains(context)) {
        auto &messages = translations[context];

        for (auto &message : messages) {
            qDebug() << message.source;
            if (message.source == source) {
                message.translation = newTranslation;
                return true; // Mise à jour réussie
            }
        }
    }
    return false; // Message non trouvé
}


bool TSTranslationReader::merge(const QString &context, const QString &source) {
    if (translations.contains(context)) {
        auto &messages = translations[context];

        for (auto &message : messages) {
            // Vérifie si le message source correspond
            if (message.source == source) {
                // Met à jour translation avec la valeur de generatedTranslation
                message.translation = message.generatedTranslation;
                return true;
            }
        }
    }
    return false; // Message non trouvé
}

void TSTranslationReader::mergeAll() {
    // Parcourt tous les contextes dans translations
    for (auto &messages : translations) {
        // Parcourt chaque message dans le contexte
        for (auto &message : messages) {
            // Met à jour translation avec la valeur de generatedTranslation
            if(message.generatedTranslation != "") {
                message.translation = message.generatedTranslation;
            }
        }
    }
}


bool TSTranslationReader::updateGeneratedTranslation(const QString &context, const QString &source, const QString &newTranslation) {
    if (translations.contains(context)) {
        auto &messages = translations[context];

        for (auto &message : messages) {
            qDebug() << message.source;
            if (message.source == source) {
                message.generatedTranslation = newTranslation;
                emit generatedTranslationChanged(context, source, newTranslation);
                return true; // Mise à jour réussie
            }
        }
    }
    return false; // Message non trouvé
}


void TSTranslationReader::waitForFinish() const
{
    if (activeClientsCount > 0) {
        QEventLoop loop;

        // Connecte le signal qui indique que toutes les traductions sont terminées pour quitter la boucle
        connect(this, &TSTranslationReader::allTranslationsCompleted, &loop, &QEventLoop::quit);

        loop.exec(); // Lance la boucle d'attente
    }
}

void TSTranslationReader::generateTranslation(const QString &interface, bool generateAll)
{
    foreach(QString context, translations.keys()){
        auto &messages = translations[context];

        for (auto &message : messages) {
            qDebug() << message.source;
            QString source = message.source;
            if(!generateAll && message.translation != ""){
                continue;
            }
            ChatGptClient *client = new ChatGptClient(interface);
            client->setLanguage(m_language);

            // Envoyer le texte pour traduction
            client->sendMessage(source);
            activeClientsCount++;
            // Utiliser un lambda pour gérer la réponse et mettre à jour la colonne 2
            connect(client, &ChatGptClient::messageReceived, this, [this, client, context, source](const QString &response) {
                updateGeneratedTranslation(context, source, response);
                qDebug() << "GPT response: " << response;
                client->deleteLater();
                activeClientsCount--; // Décrémenter le compteur en cas d'erreur
                if (activeClientsCount == 0) emit allTranslationsCompleted();
            });

            // Gérer les erreurs, si nécessaire
            connect(client, &ChatGptClient::errorOccurred, this, [this, client, context, source](const QString &error) {
                qWarning() << "Erreur de traduction pour la ligne" << context << ":" << source << ":" << error;
                client->deleteLater();
                activeClientsCount--;
                if (activeClientsCount == 0) emit allTranslationsCompleted();
            });
        }
    }
}



void TSTranslationReader::generateTranslation(const QString &interface, const QString & context, const QString &source)
{
        auto &messages = translations[context];

        for (auto &message : messages) {
            qDebug() << message.source;
            if(message.source != source){
                continue;
            }
            ChatGptClient *client = new ChatGptClient(interface);
            client->setLanguage(m_language);

            // Envoyer le texte pour traduction
            client->sendMessage(source);
            activeClientsCount++;
            // Utiliser un lambda pour gérer la réponse et mettre à jour la colonne 2
            connect(client, &ChatGptClient::messageReceived, this, [this, client, context, source](const QString &response) {
                updateGeneratedTranslation(context, source, response);
                client->deleteLater();
                activeClientsCount--;
                if (activeClientsCount == 0) emit allTranslationsCompleted();
            });

            // Gérer les erreurs, si nécessaire
            connect(client, &ChatGptClient::errorOccurred, this, [this, client, context, source](const QString &error) {
                qWarning() << "Erreur de traduction pour la ligne" << context << ":" << source << ":" << error;
                client->deleteLater();
                activeClientsCount--;
                if (activeClientsCount == 0) emit allTranslationsCompleted();
            });
            break;
        }
}

bool TSTranslationReader::save(const QString &filePath) const {
    this->waitForFinish();
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }

    QXmlStreamWriter xml(&file);
    xml.setAutoFormatting(true);
    xml.writeStartDocument();
    xml.writeDTD("<!DOCTYPE TS>");
    xml.writeStartElement("TS");
    xml.writeAttribute("version", "2.1");
    xml.writeAttribute("language", m_languageCode);

    // Écrire chaque contexte et ses messages
    for (const auto &context : translations.keys()) {
        xml.writeStartElement("context");
        xml.writeTextElement("name", context);

        const auto &messages = translations[context];
        for (const auto &message : messages) {
            xml.writeStartElement("message");
            xml.writeTextElement("source", message.source);
            xml.writeTextElement("translation", message.translation);
            xml.writeEndElement();
        }

        xml.writeEndElement(); // Fin de <context>
    }

    xml.writeEndElement(); // Fin de <TS>
    xml.writeEndDocument();

    file.close();
    return true;
}


void TSTranslationReader::setLanguage(const QString &language)
{
    m_language = language;

    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    FileAggregator customInterface;
    QStringList requestInformation = customInterface.loadAggregateFile(":/configurations/ressources/CodePaysInterface.awt");

    if (requestInformation.size() < 3) {
        qDebug() << "Error: Custom interface not valid!";
        return;
    }

    QString headerStr = requestInformation.at(1);
    QString bodyStr = requestInformation.last();
    bodyStr.replace("<m_language>", m_language);

    QUrl url(requestInformation.first());
    QNetworkRequest request(url);

    // Parsing the header as a JSON object
    QJsonDocument headerDoc = QJsonDocument::fromJson(headerStr.toUtf8());
    if (!headerDoc.isObject()) {
        qDebug() << "Error: Header is not a valid JSON object!";
        return;
    }
    QJsonObject headerObj = headerDoc.object();

    // Setting the headers from JSON object
    for (const QString &key : headerObj.keys()) {
        request.setRawHeader(key.toUtf8(), headerObj.value(key).toString().toUtf8());
    }

    // Parsing and formatting the body as a JSON object
    QJsonDocument bodyDoc = QJsonDocument::fromJson(bodyStr.toUtf8());
    if (!bodyDoc.isObject()) {
        qDebug() << "Error: Body is not a valid JSON object!";
        return;
    }

    QByteArray requestData = bodyDoc.toJson();


    // Envoyer la requête
    QNetworkReply *reply = manager->post(request, requestData);
    activeClientsCount++;
    // Utiliser une lambda pour gérer la réponse de manière asynchrone
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        if (reply->error() == QNetworkReply::NoError) {
            // Lire la réponse JSON
            QJsonDocument responseDoc = QJsonDocument::fromJson(reply->readAll());
            QJsonObject responseObject = responseDoc.object();

            // Extraire le contenu de la réponse
            QString isoCode = responseObject["choices"].toArray().first().toObject()
                                  .value("message").toObject()
                                  .value("content").toString().trimmed();

            // Mettre à jour m_languageCode avec le code ISO reçu
            m_languageCode = isoCode.remove("`").remove("\"").remove("\n");
            qDebug() << "Language code set to:" << m_languageCode;
        } else {
            // Gérer les erreurs de requête
            qWarning() << "Failed to retrieve language code:" << reply->errorString();
        }
        reply->deleteLater();
        activeClientsCount--;
        if (activeClientsCount == 0) emit allTranslationsCompleted();
    });
}

QString TSTranslationReader::language() const
{
    return m_language;
}

QString TSTranslationReader::languageCode() const
{
    return m_languageCode;
}

QStringList TSTranslationReader::languageNames() const
{
    return m_languageMap.keys();
}

QStringList TSTranslationReader::availableLanguages()
{
    return TSTranslationReader().languageNames();
}


void TSTranslationReader::setLanguageCode(const QString &newLanguageCode) {
    // Extraction du code de base de la langue (par exemple "en" de "en_US")
    QString basicLanguageCode = newLanguageCode.split("_").first();

    // Recherche du nom de la langue dans la map en utilisant le code de base
    m_language = "";  // Par défaut, vide si non trouvé
    for (auto it = m_languageMap.cbegin(); it != m_languageMap.cend(); ++it) {
        if (it.value() == basicLanguageCode) {
            m_language = it.key();  // Nom de la langue trouvée
            break;
        }
    }

    // Stockage du code complet
    m_languageCode = newLanguageCode;
}

bool TSTranslationReader::isEmpty() const {
    // Parcourir chaque contexte et chaque message
    for (const auto &messages : translations) {
        for (const auto &message : messages) {
            // Si un message a une traduction, alors ce n'est pas vide
            if (!message.translation.isEmpty()) {
                return false;
            }
        }
    }
    // Si toutes les traductions sont vides, retourner vrai
    return true;
}

bool TSTranslationReader::isFullyTranslated() const {
    // Parcourir chaque contexte et chaque message
    for (const auto &messages : translations) {
        for (const auto &message : messages) {
            // Si un message n'a pas de traduction, alors ce n'est pas complètement traduit
            if (message.translation.isEmpty()) {
                return false;
            }
        }
    }
    // Si toutes les traductions sont remplies, retourner vrai
    return true;
}

