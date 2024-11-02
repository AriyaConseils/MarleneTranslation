#ifndef TSTRANSLATIONREADER_H
#define TSTRANSLATIONREADER_H

#include <QString>
#include <QMap>
#include <QVector>
#include <QObject>

struct TranslationMessage {
    QString source;
    QString translation;
    QString generatedTranslation;
};

class TSTranslationReader: public QObject
{
    Q_OBJECT
public:
    TSTranslationReader(QObject *parent = nullptr);
    bool load(const QString &filePath);
    QMap<QString, QVector<TranslationMessage>> getTranslations() const;

    // Méthode pour mettre à jour une traduction
    bool updateTranslation(const QString &context, const QString &source, const QString &newTranslation);
    bool updateGeneratedTranslation(const QString &context, const QString &source, const QString &newTranslation);
    void generateTranslation(const QString &interface, bool generateAll = true);
    void generateTranslation(const QString &interface, const QString & context, const QString &source);
    void waitForFinish() const;

    bool merge(const QString &context, const QString &source);
    void mergeAll();
    // Méthode pour sauvegarder les traductions dans un fichier TS
    bool save(const QString &filePath) const;
    void setLanguage(const QString &language);

    QString language() const;

    QString languageCode() const;

    QStringList languageNames() const;

    void setLanguageCode(const QString &newLanguageCode);

signals:
    void generatedTranslationChanged(const QString &, const QString &, const QString &);
    void allTranslationsCompleted();

private:
    QMap<QString, QVector<TranslationMessage>> translations;
    int activeClientsCount = 0;
    QMap<QString, QString> m_languageMap;
    QString m_language;
    QString m_languageCode;
};

#endif // TSTRANSLATIONREADER_H
