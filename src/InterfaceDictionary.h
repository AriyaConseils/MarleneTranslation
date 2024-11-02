#ifndef INTERFACEDICTIONARY_H
#define INTERFACEDICTIONARY_H

#include <QDir>
#include <QFileInfoList>
#include <QStringList>
#include <QDebug>

class InterfaceDictionary
{
public:
    InterfaceDictionary(const QString &directoryPath = "configurations/interfaces")
        : m_directoryPath(directoryPath) {}

    QStringList getAwtFilePaths() const
    {
        QStringList awtFiles;
        awtFiles.append(":/configurations/ressources/TraductionInterface.awt");
        // Définit le dossier à parcourir
        QDir dir(m_directoryPath);

        if (!dir.exists()) {
            qDebug() << "Error: Directory does not exist -" << m_directoryPath;
            return awtFiles;
        }

        // Filtrer les fichiers avec l'extension .awt
        QStringList filters;
        filters << "*.awt";
        dir.setNameFilters(filters);

        // Obtenir la liste des fichiers
        QFileInfoList fileInfoList = dir.entryInfoList(QDir::Files);
        for (const QFileInfo &fileInfo : fileInfoList) {
            awtFiles.append(fileInfo.absoluteFilePath());
        }

        return awtFiles;
    }

    QStringList getNames() const
    {
        QStringList awtNames;
        awtNames.append("GptInterface");

        // Définit le dossier à parcourir
        QDir dir(m_directoryPath);

        if (!dir.exists()) {
            qDebug() << "Error: Directory does not exist -" << m_directoryPath;
            return awtNames;
        }

        // Filtrer les fichiers avec l'extension .awt
        QStringList filters;
        filters << "*.awt";
        dir.setNameFilters(filters);

        // Obtenir uniquement les noms des fichiers
        QFileInfoList fileInfoList = dir.entryInfoList(QDir::Files);
        for (const QFileInfo &fileInfo : fileInfoList) {
            awtNames.append(fileInfo.fileName().remove(".awt"));
        }

        return awtNames;
    }

private:
    QString m_directoryPath;
};

#endif // INTERFACEDICTIONARY_H
