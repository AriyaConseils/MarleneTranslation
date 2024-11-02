#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "TSTranslationReader.h"




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
    void on_toolButtonTsFilePath_clicked();

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

private:
    Ui::MainWindow *ui;
    TSTranslationReader translator;

    void populateTableWidget(const QMap<QString, QVector<TranslationMessage>> &translations);
};
#endif // MAINWINDOW_H
