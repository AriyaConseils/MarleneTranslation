#ifndef WELCOMEPAGECONTROLLER_H
#define WELCOMEPAGECONTROLLER_H

#include <QWidget>
#include <QListWidgetItem>
#include <QTranslator>
#include "vues/projectlist.h"


namespace Ui {
class WelcomePageController;
}

class WelcomePageController : public QWidget
{
    Q_OBJECT

public:
    explicit WelcomePageController(QWidget *parent = nullptr);
    ~WelcomePageController();

public slots:
    void addOpenedProject(QString path);

private slots:
    void on_pushButtonOpenProject_clicked();
    void on_pushButtonNewProjet_clicked();
    void on_pushButtonProjets_clicked();

    void on_pushButtonAbout_clicked();


signals:
    void openProject(QString);


private:
    QString loadStyleSheet(const QString &sheetName);
    bool isSystemInDarkMode();

private:
    Ui::WelcomePageController *ui;
    ProjectList *m_projetsWidget;
    QTranslator translator;

};

#endif // WELCOMEPAGECONTROLLER_H
