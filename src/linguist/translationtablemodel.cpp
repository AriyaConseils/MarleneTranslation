#include "translationtablemodel.h"
#include <QFile>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QDebug>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QEventLoop>
#include "chatgptclient.h"

TSTranslationModel::TSTranslationModel(QObject *parent)
    : QAbstractTableModel(parent)
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
        { "Chinese", "zh" },
        { "Croatian", "hr" },
        { "Czech", "cs" },
        { "Danish", "da" },
        { "Dutch", "nl" },
        { "English", "en" },
        { "French", "fr" },
        { "German", "de" },
        { "Greek", "el" },
        { "Hindi", "hi" },
        { "Hungarian", "hu" },
        { "Italian", "it" },
        { "Japanese", "ja" },
        { "Korean", "ko" },
        { "Polish", "pl" },
        { "Portuguese", "pt" },
        { "Romanian", "ro" },
        { "Russian", "ru" },
        { "Spanish", "es" },
        { "Swedish", "sv" },
        { "Turkish", "tr" },
        { "Ukrainian", "uk" }
    };

    connect(this, &TSTranslationModel::durtyChanged, this, [this](bool durty){
        m_isDurty = durty;
    });
}


bool TSTranslationModel::load(const QString &filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }

    beginResetModel();

    translations.clear();
    contexts.clear();

    QXmlStreamReader xml(&file);
    QString currentContext;

    if (xml.readNextStartElement()) {
        if (xml.name() == QStringLiteral("TS")) {
            if (xml.attributes().hasAttribute("language")) {
                setLanguageCode(xml.attributes().value("language").toString());
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
                contexts.append(currentContext);
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
    endResetModel();

    return !xml.hasError();
}




bool TSTranslationModel::save(const QString &filePath)  {
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }
    m_isDurty = false;
    QXmlStreamWriter xml(&file);
    xml.setAutoFormatting(true);
    xml.writeStartDocument();
    xml.writeDTD("<!DOCTYPE TS>");
    xml.writeStartElement("TS");
    xml.writeAttribute("version", "2.1");
    xml.writeAttribute("language", m_languageCode);

    for (const auto &context : contexts) {
        xml.writeStartElement("context");
        xml.writeTextElement("name", context);

        for (auto &message : translations[context]) {
            xml.writeStartElement("message");
            xml.writeTextElement("source", message.source);
            xml.writeTextElement("translation", message.translation);
            if(message.generatedTranslation != "" && message.translation != message.generatedTranslation){
                if(!m_isDurty) emit durtyChanged(true);
            } else {
                message.generatedTranslation = "";
            }
            xml.writeEndElement();
        }

        xml.writeEndElement(); // Fin de <context>
    }

    xml.writeEndElement(); // Fin de <TS>
    xml.writeEndDocument();

    file.close();
    emit dataChanged(index(0, 3), index(rowCount() - 1, 3));

    if(!m_isDurty){
        emit durtyChanged(m_isDurty);
    }
    return true;
}



int TSTranslationModel::rowCount(const QModelIndex &parent) const {
    if (parent.isValid()) {
        return 0;
    }
    int totalRows = 0;
    for (const auto &context : contexts) {
        totalRows += translations[context].size();
    }
    return totalRows;
}

int TSTranslationModel::columnCount(const QModelIndex &parent) const {
    if (parent.isValid()) {
        return 0;
    }
    return 4; // Context, Source, Translation, generatedTranslation
}

QVariant TSTranslationModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || role != Qt::DisplayRole) {
        return QVariant();
    }

    int row = index.row();
    int col = index.column();

    int currentRow = 0;
    for (const auto &context : contexts) {
        const auto &messages = translations[context];
        if (row < currentRow + messages.size()) {
            const auto &message = messages[row - currentRow];
            switch (col) {
            case 0: return context;
            case 1: return message.source;
            case 2: return message.translation;
            case 3: return message.generatedTranslation;
            default: return QVariant();
            }
        }
        currentRow += messages.size();
    }

    return QVariant();
}

QVariant TSTranslationModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role != Qt::DisplayRole) {
        return QVariant();
    }

    if (orientation == Qt::Horizontal) {
        switch (section) {
        case 0: return QStringLiteral("Context");
        case 1: return QStringLiteral("Source");
        case 2: return QStringLiteral("Translation");
        case 3: return QStringLiteral("New translation");
        default: return QVariant();
        }
    }
    return QVariant();
}


bool TSTranslationModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    if (!index.isValid() || role != Qt::EditRole) {
        return false;
    }

    int row = index.row();
    int col = index.column();

    int currentRow = 0;
    for (const auto &context : contexts) {
        auto &messages = translations[context];
        if (row < currentRow + messages.size()) {
            auto &message = messages[row - currentRow];
            if (col == 2) {
                message.translation = value.toString();
                emit dataChanged(index, index, {role});
                emit durtyChanged(true);
                return true;
            }
        }
        currentRow += messages.size();
    }

    return false;
}

Qt::ItemFlags TSTranslationModel::flags(const QModelIndex &index) const {
    if (!index.isValid()) {
        return Qt::NoItemFlags;
    }
    Qt::ItemFlags flags = QAbstractTableModel::flags(index);
    if (index.column() == 2) {
        flags |= Qt::ItemIsEditable;
    }
    return flags;
}


void TSTranslationModel::setLanguageCode(const QString &newLanguageCode) {
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


void TSTranslationModel::setLanguage(const QString &language) {
    m_language = language;

    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    QUrl url("https://api.example.com/setLanguage");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject json;
    json["language"] = m_language;
    QJsonDocument doc(json);
    QByteArray data = doc.toJson();

    QNetworkReply *reply = manager->post(request, data);
    activeClientsCount++;

    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        if (reply->error() == QNetworkReply::NoError) {
            QJsonDocument responseDoc = QJsonDocument::fromJson(reply->readAll());
            QJsonObject responseObject = responseDoc.object();

            QString isoCode = responseObject["language_code"].toString();
            setLanguageCode(isoCode);
            qDebug() << "Language code set to:" << m_languageCode;
        } else {
            qWarning() << "Failed to set language:" << reply->errorString();
        }
        reply->deleteLater();
        activeClientsCount--;
        if (activeClientsCount == 0) emit allTranslationsCompleted();
    });
}

bool TSTranslationModel::isDurty() const
{
    return m_isDurty;
}


void TSTranslationModel::generateTranslation(const QString &interface, bool generateAll)
{
    for (const QString &context : translations.keys()) {
        auto &messages = translations[context];

        for (auto &message : messages) {
            qDebug() << "Source text:" << message.source;
            if (!generateAll && !message.translation.isEmpty()) {
                continue;
            }

            ChatGptClient *client = new ChatGptClient(interface);
            client->setLanguage(m_language);

            // Envoyer le texte pour traduction
            client->sendMessage(message.source);
            activeClientsCount++;

            // Gérer la réponse et mettre à jour la traduction générée
            connect(client, &ChatGptClient::messageReceived, this, [this, client, context, &message](const QString &response) {
                message.generatedTranslation = response;
                emit durtyChanged(true);
                qDebug() << "GPT response for" << message.source << ":" << response;
                client->deleteLater();
                activeClientsCount--;

                emit dataChanged(index(0, 3), index(rowCount() - 1, 3)); // Actualise la colonne de suggestions
                if (activeClientsCount == 0) emit allTranslationsCompleted();
            });

            // Gérer les erreurs de traduction
            connect(client, &ChatGptClient::errorOccurred, this, [this, client, context, &message](const QString &error) {
                qWarning() << "Erreur de traduction pour" << context << ":" << message.source << ":" << error;
                client->deleteLater();
                activeClientsCount--;

                if (activeClientsCount == 0) emit allTranslationsCompleted();
            });
        }
    }
}

void TSTranslationModel::generateTranslation(const QString &interface, const QString &context, const QString &source)
{
    auto &messages = translations[context];

    for (auto &message : messages) {
        if (message.source != source) {
            continue;
        }

        ChatGptClient *client = new ChatGptClient(interface);
        client->setLanguage(m_language);

        // Envoyer le texte pour traduction
        client->sendMessage(source);
        activeClientsCount++;

        // Gérer la réponse et mettre à jour la traduction générée
        connect(client, &ChatGptClient::messageReceived, this, [this, client, context, &message](const QString &response) {
            message.generatedTranslation = response;
            emit durtyChanged(true);
            qDebug() << "GPT response for" << message.source << ":" << response;
            client->deleteLater();
            activeClientsCount--;

            emit dataChanged(index(0, 3), index(rowCount() - 1, 3)); // Actualise la colonne de suggestions
            if (activeClientsCount == 0) emit allTranslationsCompleted();
        });

        // Gérer les erreurs de traduction
        connect(client, &ChatGptClient::errorOccurred, this, [this, client, context, &message](const QString &error) {
            qWarning() << "Erreur de traduction pour" << context << ":" << message.source << ":" << error;
            client->deleteLater();
            activeClientsCount--;
            if (activeClientsCount == 0) emit allTranslationsCompleted();
        });
        break;
    }
}


bool TSTranslationModel::isEmpty() const {
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

bool TSTranslationModel::isFullyTranslated() const {
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



bool TSTranslationModel::merge(const QString &context, const QString &source) {
    if (translations.contains(context)) {
        auto &messages = translations[context];

        for (auto &message : messages) {
            // Vérifie si le message source correspond
            if (message.source == source) {
                // Met à jour translation avec la valeur de generatedTranslation
                message.translation = message.generatedTranslation;
                emit durtyChanged(true);
                emit dataChanged(index(0, 2), index(rowCount() - 1, 2));
                return true;
            }
        }
    }
    return false; // Message non trouvé
}

void TSTranslationModel::mergeAll() {
    // Parcourt tous les contextes dans translations
    for (auto &messages : translations) {
        // Parcourt chaque message dans le contexte
        for (auto &message : messages) {
            // Met à jour translation avec la valeur de generatedTranslation
            if(message.generatedTranslation != "") {
                message.translation = message.generatedTranslation;
                emit durtyChanged(true);
                emit dataChanged(index(0, 2), index(rowCount() - 1, 2));
            }
        }
    }
}

bool TSTranslationModel::updateTranslation(const QString &context, const QString &source, const QString &newTranslation) {
    if (translations.contains(context)) {
        auto &messages = translations[context];

        for (auto &message : messages) {
            qDebug() << message.source;
            if (message.source == source) {
                message.translation = newTranslation;
                emit durtyChanged(true);
                emit dataChanged(index(0, 2), index(rowCount() - 1, 2));
                return true; // Mise à jour réussie
            }
        }
    }
    return false; // Message non trouvé
}

bool TSTranslationModel::updateGeneratedTranslation(const QString &context, const QString &source, const QString &newTranslation) {
    if (translations.contains(context)) {
        auto &messages = translations[context];

        for (auto &message : messages) {
            qDebug() << message.source;
            if (message.source == source) {
                message.generatedTranslation = newTranslation;
                emit durtyChanged(true);
                emit dataChanged(index(0, 3), index(rowCount() - 1, 3));
                return true; // Mise à jour réussie
            }
        }
    }
    return false; // Message non trouvé
}

void TSTranslationModel::waitForFinish() const
{
    if (activeClientsCount > 0) {
        QEventLoop loop;

        // Connecte le signal qui indique que toutes les traductions sont terminées pour quitter la boucle
        connect(this, &TSTranslationModel::allTranslationsCompleted, &loop, &QEventLoop::quit);

        loop.exec(); // Lance la boucle d'attente
    }
}
