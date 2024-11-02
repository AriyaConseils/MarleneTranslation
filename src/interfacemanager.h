#ifndef INTERFACEMANAGER_H
#define INTERFACEMANAGER_H

#include <QDialog>

namespace Ui {
class InterfaceManager;
}

class InterfaceManager : public QDialog
{
    Q_OBJECT

public:
    explicit InterfaceManager(QWidget *parent = nullptr);
    ~InterfaceManager();

private slots:
    void on_toolButtonHeader_clicked();

    void on_toolButtonBody_clicked();

    void on_pushButtonGenerate_clicked();

private:
    Ui::InterfaceManager *ui;
};

#endif // INTERFACEMANAGER_H
