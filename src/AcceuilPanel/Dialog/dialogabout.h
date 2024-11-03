#ifndef DIALOGABOUT_H
#define DIALOGABOUT_H

#include <QDialog>

namespace Ui {
class DialogAbout;
}

class DialogAbout : public QDialog
{
    Q_OBJECT

public:
    explicit DialogAbout(QWidget *parent = nullptr);
    ~DialogAbout();

private slots:
    void on_pushButtonOk_clicked();

private:
    Ui::DialogAbout *ui;
    QString loadStyleSheet(const QString &sheetName);
    bool isSystemInDarkMode();
};

#endif // DIALOGABOUT_H
