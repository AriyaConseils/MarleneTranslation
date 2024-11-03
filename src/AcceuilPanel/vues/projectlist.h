#ifndef PROJECTLIST_H
#define PROJECTLIST_H

#include <QWidget>
#include <QListWidgetItem>
#include <QFile>
#include <QSettings>
#include "ProjectFileWidget.h"


namespace Ui {
class ProjectList;
}

class ProjectList : public QWidget
{
    Q_OBJECT
public:
    explicit ProjectList(QWidget *parent = nullptr);
    ~ProjectList();

    void addOpenedProject(QString path);


private slots:
    void on_listWidget_itemClicked(QListWidgetItem *item);
    void on_listWidget_customContextMenuRequested(const QPoint &pos);
    void removeProjectFromList(ProjectFileWidget *projectWidget);
    void clearProjectList();

signals:
    void openProject(QString);


private:
    Ui::ProjectList *ui;
    QMap <QString, QStringList> verstionListMap;
    void saveProjectsToIni();
    QStringList loadProjectsFromIni();
    void addNewLine(QString path);

};

#endif // PROJECTLIST_H
