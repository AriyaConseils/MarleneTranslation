#include "projectlist.h"
#include "ui_projectlist.h"
#include <QMenu>
#include <QInputDialog>
#include <QMessageBox>
#include <QDir>
#include <QFile>
#include <QStandardPaths>

ProjectList::ProjectList(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ProjectList)
{
    ui->setupUi(this);
    ui->listWidget->setContextMenuPolicy(Qt::CustomContextMenu);

    foreach(QString path, loadProjectsFromIni()){
        if (!QFile::exists(path)) {
            continue;
        }
        addNewLine(path);
    }

}


ProjectList::~ProjectList()
{
    delete ui;
}



void ProjectList::on_listWidget_itemClicked(QListWidgetItem *item)
{
    ProjectFileWidget* projectWidget = qobject_cast<ProjectFileWidget*>(ui->listWidget->itemWidget(item));
    if (projectWidget) {
        QString projectName = projectWidget->getProjectName();
        QString filePath = projectWidget->getProjectPath();
        emit openProject(filePath + projectName + QString(".rrt"));
    }
}

void ProjectList::addNewLine(QString path){
    QString fileName = path.replace("\\", "/").split("/").last().remove(".rrt");
    QString projectPath = path.replace(fileName + ".rrt", "");
    for (int i = 0; i < ui->listWidget->count(); ++i) {
        QListWidgetItem* existingItem = ui->listWidget->item(i);
        ProjectFileWidget* projectWidget = qobject_cast<ProjectFileWidget*>(ui->listWidget->itemWidget(existingItem));

        if (projectWidget) {
            QString existingFileName = projectWidget->getProjectName();
            QString existingFilePath = projectWidget->getProjectPath();

            // Vérifier si le nom du fichier et le chemin correspondent
            if (existingFileName == fileName && existingFilePath == projectPath) {
                // Supprimer l'élément existant
                delete ui->listWidget->takeItem(i);
                break; // Sortir de la boucle après la suppression
            }
        }
    }

    // Ajouter le projet mis à jour en haut de la liste
    QListWidgetItem* listItem = new QListWidgetItem();
    listItem->setSizeHint(QSize(100, 50)); // Hauteur de l'élément de la liste

    ProjectFileWidget* newProjectWidget = new ProjectFileWidget();


    newProjectWidget->setProjectName(fileName);
    newProjectWidget->setProjectPath(projectPath);

    // Insérer le nouvel élément en haut de la liste
    ui->listWidget->insertItem(0, listItem);
    ui->listWidget->setItemWidget(listItem, newProjectWidget);
    ui->listWidget->scrollToItem(listItem);
}


void ProjectList::addOpenedProject(QString path)
{
    addNewLine(path);
    saveProjectsToIni();
}

void ProjectList::saveProjectsToIni() {
    QSettings settings("AriyaConsulting", "MarleneTranslation");

    settings.beginWriteArray("projects");
    for (int i = 0; i < ui->listWidget->count(); ++i) {
        settings.setArrayIndex(i);
        QListWidgetItem* item = ui->listWidget->item(i);
        ProjectFileWidget* projectWidget = qobject_cast<ProjectFileWidget*>(ui->listWidget->itemWidget(item));
        if (projectWidget) {
            settings.setValue("path", projectWidget->getProjectPath() + projectWidget->getProjectName() + QString(".rrt"));
        }
    }
    settings.endArray();
}


void ProjectList::removeProjectFromList(ProjectFileWidget *projectWidget)
{
    QString projectName = projectWidget->getProjectName();

    for (int i = 0; i < ui->listWidget->count(); ++i) {
        QListWidgetItem* item = ui->listWidget->item(i);
        ProjectFileWidget* widget = qobject_cast<ProjectFileWidget*>(ui->listWidget->itemWidget(item));
        if (widget && widget == projectWidget) {
            delete ui->listWidget->takeItem(i);
            saveProjectsToIni();
            return;
        }
    }
}

void ProjectList::clearProjectList()
{
    ui->listWidget->clear();
    QSettings settings("AriyaConsulting", "MarleneTranslation");
    settings.beginGroup("projects");
    settings.remove("");
    settings.endGroup();
}


QStringList ProjectList::loadProjectsFromIni() {
    QStringList projectPaths;
    QSettings settings("AriyaConsulting", "MarleneTranslation");
    int size = settings.beginReadArray("projects");
    for (int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        QString path = settings.value("path").toString();
        if (!path.isEmpty()) {
            projectPaths.append(path);
        }
    }
    settings.endArray();

    std::reverse(projectPaths.begin(), projectPaths.end());

    return projectPaths;
}





void ProjectList::on_listWidget_customContextMenuRequested(const QPoint &pos)
{
    QListWidgetItem* item = ui->listWidget->itemAt(pos);
    if (item) {
        ProjectFileWidget* projectWidget = qobject_cast<ProjectFileWidget*>(ui->listWidget->itemWidget(item));
        if (projectWidget) {
            QMenu contextMenu(tr("Context menu"), this);

            QAction actionDeleteRecentProjects("Supprimer des projets récents", this);
            actionDeleteRecentProjects.setIcon(QIcon(":/icon/ressources/assets/welcome/delete-file.png"));
            connect(&actionDeleteRecentProjects, &QAction::triggered, this, [this, projectWidget]() {
                removeProjectFromList(projectWidget);
            });
            contextMenu.addAction(&actionDeleteRecentProjects);

            QAction actionCleanRecentProjectsList("Nettoyer la liste des projets récents", this);
            actionCleanRecentProjectsList.setIcon(QIcon(":/icon/ressources/assets/welcome/delete-folder.png"));
            connect(&actionCleanRecentProjectsList, &QAction::triggered, this, [this]() {
                QMessageBox::StandardButton reply;
                reply = QMessageBox::question(this, tr("Confirmation"),
                                              tr("Êtes-vous sûr de vouloir nettoyer la liste des projets récents ?"),
                                              QMessageBox::Yes | QMessageBox::No);
                if (reply == QMessageBox::Yes) {
                    clearProjectList();
                }
            });
            contextMenu.addAction(&actionCleanRecentProjectsList);

            contextMenu.exec(ui->listWidget->mapToGlobal(pos));
            projectWidget->setVersionNumber(verstionListMap.value(projectWidget->getProjectName()).count());
        }
    }
}

