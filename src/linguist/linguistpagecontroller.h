#ifndef LINGUISTPAGECONTROLLER_H
#define LINGUISTPAGECONTROLLER_H

#include <QWidget>
#include "TSTranslationReader.h"

namespace Ui {
class LinguistpageController;
}

class LinguistpageController : public QWidget
{
    Q_OBJECT

public:
    explicit LinguistpageController(QWidget *parent = nullptr);
    ~LinguistpageController();
    void updateTranslation(const QString language, bool fullUpdate);

public slots:
    void setInterfaceName(const QString &interface);
    void on_pushButtonSaveAs_clicked();
    void on_pushButtonSave_clicked();
    void setTsFilesDirectory(const QString &path);
    void updateFilesList();
private slots:
    void mergeSelectedRow();
    void mergeAllSelectedRows();
    void mergeAllRows();
    void updateSelectedRows();
    void updateLanguage(const QString &arg1);
    void on_pushButtonUpdate_clicked();
    void on_listWidgetTsList_itemSelectionChanged();




private:
    void populateTableWidget(const QMap<QString, QVector<TranslationMessage>> &translations);
    void manageLanguage();
    void save(const QString &filePath);
    bool load(const QString &filePath);

private:
    Ui::LinguistpageController *ui;
    TSTranslationReader translator;
    QString m_interface;
    QString m_currentTsFilesDirectory;

};

#endif // LINGUISTPAGECONTROLLER_H
