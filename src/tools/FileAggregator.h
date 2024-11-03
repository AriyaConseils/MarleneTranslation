#ifndef FILEAGGREGATOR_H
#define FILEAGGREGATOR_H

#include <QFile>
#include <QByteArray>
#include <QString>
#include <QList>
#include <QUrl>
#include <QDebug>

class FileAggregator
{
public:
    FileAggregator(const QUrl& url, const QList<QString>& paths, const QByteArray& encryptionKey = "Marlene")
        : m_url(url), m_paths(paths), m_encryptionKey(encryptionKey) {}

    FileAggregator(const QByteArray& encryptionKey = "Marlene")
        : m_url(""), m_paths(""), m_encryptionKey(encryptionKey) {}

    bool generateAggregateFile(const QString& outputFilePath)
    {
        QFile outputFile(outputFilePath);
        if (!outputFile.open(QIODevice::WriteOnly)) {
            qDebug() << "Failed to open output file for writing:" << outputFilePath;
            return false;
        }

        QByteArray urlData = m_url.toString().toUtf8();
        if (!m_encryptionKey.isEmpty()) {
            urlData = encryptDecrypt(urlData);
        }

        outputFile.write(intToByteArray(urlData.size()));  // Écriture de la taille de l'URL en 4 octets
        outputFile.write(urlData);                         // Écriture de l'URL chiffrée

        for (const QString& path : m_paths) {
            QFile inputFile(path);
            if (inputFile.open(QIODevice::ReadOnly)) {
                QByteArray content = inputFile.readAll();
                if (!m_encryptionKey.isEmpty()) {
                    content = encryptDecrypt(content); // Crypter le contenu
                }
                outputFile.write(intToByteArray(content.size())); // Taille en 4 octets
                outputFile.write(content); // Contenu chiffré
                inputFile.close();
            } else {
                qDebug() << "Failed to open input file:" << path;
            }
        }

        outputFile.close();
        return true;
    }

    QStringList loadAggregateFile(const QString& aggregateFilePath)
    {
        QStringList contentList;

        QFile aggregateFile(aggregateFilePath);
        if (!aggregateFile.open(QIODevice::ReadOnly)) {
            qDebug() << "Failed to open aggregate file for reading:" << aggregateFilePath;
            return contentList;
        }

        // Lire l'URL avec vérification de la taille
        QByteArray urlSizeData = aggregateFile.read(4);
        if (urlSizeData.size() != 4) {
            qDebug() << "Error: URL size header is not 4 bytes.";
            return contentList;
        }

        int urlSize = byteArrayToInt(urlSizeData);
        QByteArray urlData = aggregateFile.read(urlSize);
        if (urlData.size() != urlSize) {
            qDebug() << "Error: URL data size does not match the expected size.";
            return contentList;
        }

        if (!m_encryptionKey.isEmpty()) {
            urlData = encryptDecrypt(urlData); // Déchiffrer l'URL
        }
        contentList.append(QString::fromUtf8(urlData));

        // Lecture des contenus de chaque fichier
        while (!aggregateFile.atEnd()) {
            QByteArray sizeData = aggregateFile.read(4); // Taille en 4 octets
            if (sizeData.size() != 4) {
                qDebug() << "Error: Content size header is not 4 bytes.";
                break;
            }

            int contentSize = byteArrayToInt(sizeData);
            QByteArray content = aggregateFile.read(contentSize);
            if (content.size() != contentSize) {
                qDebug() << "Error: Content data size does not match the expected size. Expected"
                         << contentSize << "but got" << content.size();
                break;
            }

            if (!m_encryptionKey.isEmpty()) {
                content = encryptDecrypt(content); // Déchiffrer le contenu
            }
            contentList.append(QString::fromUtf8(content));
        }

        aggregateFile.close();
        return contentList;
    }

private:
    QUrl m_url;
    QList<QString> m_paths;
    QByteArray m_encryptionKey;

    QByteArray encryptDecrypt(const QByteArray& data) const
    {
        QByteArray result = data;
        for (int i = 0; i < result.size(); ++i) {
            result[i] = result[i] ^ m_encryptionKey.at(i % m_encryptionKey.size());
        }
        return result;
    }

    QByteArray intToByteArray(int value) const
    {
        QByteArray byteArray;
        byteArray.append((value >> 24) & 0xFF);
        byteArray.append((value >> 16) & 0xFF);
        byteArray.append((value >> 8) & 0xFF);
        byteArray.append(value & 0xFF);
        return byteArray;
    }

    int byteArrayToInt(const QByteArray& byteArray) const
    {
        if (byteArray.size() != 4) {
            qDebug() << "Error: QByteArray size is not 4 bytes.";
            return -1;
        }
        return (static_cast<unsigned char>(byteArray[0]) << 24) |
               (static_cast<unsigned char>(byteArray[1]) << 16) |
               (static_cast<unsigned char>(byteArray[2]) << 8) |
               (static_cast<unsigned char>(byteArray[3]));
    }
};

#endif // FILEAGGREGATOR_H
