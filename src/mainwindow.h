#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "TSTranslationReader.h"
#include <QListWidgetItem>
#include "lupdate/translationextractor.h"



QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void save(const QString &filePath);
    bool load(const QString &filePath);

    void updateTranslation(const QString language, bool fullUpdate);
    void setInterface(const QString interface);

private slots:
    void on_toolButtonTsFilesPath_clicked();

    void on_pushButtonUpdate_clicked();

    void on_pushButtonSave_clicked();

    void mergeSelectedRow();
    void mergeAllSelectedRows();
    void mergeAllRows();
    void updateSelectedRows();

    void on_toolButtonAddInterface_clicked();

    void on_comboBoxInterface_currentTextChanged(const QString &arg1);

    void on_pushButtonSaveAs_clicked();

    void on_language_Changed(const QString &arg1);

    void on_listWidgetTsList_itemSelectionChanged();

    void on_pushButtonUpdateTsFiles_clicked();

    void on_toolButtonSourceCodePath_clicked();

private:
    Ui::MainWindow *ui;
    TSTranslationReader translator;
    TranslationExtractor *m_lupdate;
    void populateTableWidget(const QMap<QString, QVector<TranslationMessage>> &translations);
    void manageLanguage();
    void initializeLanguageList();
    void refreshTsFiles(const QString &path);

};
#endif // MAINWINDOW_H
