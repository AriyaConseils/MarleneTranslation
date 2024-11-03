#ifndef CHATGPTCLIENT_H
#define CHATGPTCLIENT_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>

class ChatGptClient : public QObject
{
    Q_OBJECT

public:
    explicit ChatGptClient(const QString &interface = "", QObject *parent = nullptr);
    void sendMessage(const QString &message);
    void setLanguage(const QString &language){ m_language = language;}
    QString language(){return m_language;}

signals:
    void messageReceived(const QString &response);
    void errorOccurred(const QString &errorString);

private slots:
    void onReplyFinished(QNetworkReply *reply);

private:
    QNetworkAccessManager networkManager;
    QString m_interface;
    QString m_language;
};

#endif // CHATGPTCLIENT_H
