#include "ChatGptClient.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QNetworkRequest>
#include <QUrl>
#include <QRegularExpression>
#include "FileAggregator.h"



ChatGptClient::ChatGptClient(const QString &interface, QObject *parent)
    : QObject(parent), m_interface(interface)
{
    connect(&networkManager, &QNetworkAccessManager::finished, this, &ChatGptClient::onReplyFinished);
}


void ChatGptClient::sendMessage(const QString &textToTranslate)
{
    if(m_language == ""){
        qDebug() << "Error: language is null";
        return;
    }

    FileAggregator customInterface;
    QStringList requestInformation = customInterface.loadAggregateFile(m_interface);

    if (requestInformation.size() < 3) {
        qDebug() << "Error: Custom interface not valid!";
        return;
    }

    QString headerStr = requestInformation.at(1);
    QString bodyStr = requestInformation.last();
    bodyStr.replace("<m_language>", m_language);
    bodyStr.replace("<textToTranslate>", textToTranslate);

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
    networkManager.post(request, requestData);
}



void ChatGptClient::onReplyFinished(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray responseData = reply->readAll();
        QJsonDocument jsonResponse = QJsonDocument::fromJson(responseData);
        QJsonObject jsonObject = jsonResponse.object();

        // Extraire le contenu du message de la réponse
        QString responseContent;
        if (jsonObject.contains("choices")) {
            QJsonArray choicesArray = jsonObject["choices"].toArray();
            if (!choicesArray.isEmpty()) {
                responseContent = choicesArray[0].toObject()["message"].toObject()["content"].toString();
            }
        }

        QRegularExpression tripleBacktickPattern("```(.*?)```", QRegularExpression::DotMatchesEverythingOption);
        QRegularExpression singleBacktickPattern("`([^`]*)`");

        // Essayer d'abord avec le bloc de code ```...```
        QRegularExpressionMatch match = tripleBacktickPattern.match(responseContent);
        QString extractedContent;

        qDebug() << "****************" << responseContent;

        if (match.hasMatch()) {
            extractedContent = match.captured(1).trimmed();  // Capturer le contenu du bloc avec trois accents graves
        } else {
            // Si aucun bloc avec ``` n'est trouvé, essayer avec un bloc de code entouré de `
            match = singleBacktickPattern.match(responseContent);
            if (match.hasMatch()) {
                extractedContent = match.captured(1).trimmed();  // Capturer le contenu du bloc avec un accent grave
            } else {
                extractedContent = responseContent.trimmed(); // Si aucun bloc de code n'est trouvé, utiliser le contenu brut
            }
        }


        emit messageReceived(extractedContent);
    } else {
        // Collecte des informations d'erreur détaillées
        QString errorDetails = QString("Erreur de transfert : %1\nCode d'erreur : %2\n")
                                   .arg(reply->errorString())
                                   .arg(reply->error());

        // Affichage des en-têtes de réponse
        QList<QByteArray> headerList = reply->rawHeaderList();
        errorDetails += "En-têtes de réponse :\n";
        for (const QByteArray &header : headerList) {
            errorDetails += QString("%1: %2\n").arg(QString(header), QString(reply->rawHeader(header)));
        }

        // Affichage du corps de la réponse si disponible
        QByteArray responseBody = reply->readAll();
        if (!responseBody.isEmpty()) {
            errorDetails += QString("Corps de la réponse :\n%1").arg(QString(responseBody));
        }

        // Émettre le signal d'erreur avec tous les détails
        emit errorOccurred(errorDetails);
    }

    reply->deleteLater();
}
