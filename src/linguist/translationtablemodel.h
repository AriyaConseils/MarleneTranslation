#ifndef TSTRANSLATIONMODEL_H
#define TSTRANSLATIONMODEL_H

#include <QAbstractTableModel>
#include <QMap>
#include <QVector>
#include <QObject>


struct TranslationMessage {
    QString source;
    QString translation;
    QString generatedTranslation;
};

class TSTranslationModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    TSTranslationModel(QObject *parent = nullptr);

    // Charger un fichier TS
    bool load(const QString &filePath);

    // Sauvegarder les traductions dans un fichier TS
    bool save(const QString &filePath);

    // Méthodes obligatoires pour QAbstractTableModel
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    // Fonctions supplémentaires
    void generateTranslation(const QString &interface, bool generateAll = true);
    void generateTranslation(const QString &interface, const QString &context, const QString &source);

    QString language() const {return m_language;}
    QString languageCode() const {return m_languageCode;}
    static QStringList availableLanguages();
    void setLanguageCode(const QString &newLanguageCode);
    bool isEmpty() const;
    bool isFullyTranslated() const;

    bool merge(const QString &context, const QString &source);
    void mergeAll();
    bool updateGeneratedTranslation(const QString &context, const QString &source, const QString &newTranslation);
    bool updateTranslation(const QString &context, const QString &source, const QString &newTranslation);


    bool isDurty() const;

public slots:
    void setLanguage(const QString &language);

signals:
    void generatedTranslationChanged(const QString &, const QString &, const QString &);
    void allTranslationsCompleted();
    void durtyChanged(bool);
private:
    QMap<QString, QVector<TranslationMessage>> translations;
    QStringList contexts;
    int activeClientsCount = 0;
    QMap<QString, QString> m_languageMap;
    QString m_language;
    QString m_languageCode;
    bool m_isDurty;
};

#endif // TSTRANSLATIONMODEL_H
