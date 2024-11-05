#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidgetItem>
#include "lupdate/translationextractor.h"


class LReleasePageControler;
class LupdatePageController;
class LinguistpageController;

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

//    void save(const QString &filePath);
    bool load(const QString &filePath);

    void setInterface(const QString interface);
    LReleasePageControler *getLRelease();
    LupdatePageController *getLUpdate();
    LinguistpageController *getLInguist();

private slots:
    void on_toolButtonAddInterface_clicked();

    void on_comboBoxInterface_currentTextChanged(const QString &arg1);

private:
    Ui::MainWindow *ui;


};
#endif // MAINWINDOW_H
