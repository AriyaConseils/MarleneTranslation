#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include "linguist/InterfaceDictionary.h"
#include "interfacemanager.h"
#include <QSettings>
#include <QShortcut>
#include <QFileDialog>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->widgetInterface->hide();
    new QShortcut(QKeySequence("Ctrl+I"), this, [this]() {
        ui->widgetInterface->setVisible(!ui->widgetInterface->isVisible());
    });

    InterfaceDictionary dictionary;
    QStringList awtFilePaths = dictionary.getAwtFilePaths();
    QStringList awtFileNames = dictionary.getNames();
    QSettings settings("AriyaConsulting", "MarleneTranslation");
    QString lastInterfaceUsed = settings.value("lastInterfaceUsed", "").toString();

    int index = 0;
    for (const QString &filePath : awtFilePaths) {
        // Ajouter le nom du fichier dans la QComboBox et stocker le chemin complet dans les données
        ui->comboBoxInterface->addItem(awtFileNames.at(index), filePath);
        index++;
    }

    ui->comboBoxInterface->setCurrentText(lastInterfaceUsed);

    // Ajuste chaque colonne pour qu'elle prenne toute la largeur du widget


    connect(ui->widgetWelcomePage, &WelcomePageController::openProject, this, [this](QString){
        ui->tabWidget->setCurrentIndex(1);
    });

    connect(ui->widgetLupadatePage, &LupdatePageController::tsFilesUpdated, this, [this](){
        ui->tabWidget->setCurrentIndex(2);
    });

    connect(ui->widgetWelcomePage, &WelcomePageController::openProject, ui->widgetLupadatePage, &LupdatePageController::loadProject);
    connect(ui->widgetLupadatePage, &LupdatePageController::tsDirectoryChanged, ui->widgetLinguistPage, &LinguistpageController::setTsFilesDirectory);
    connect(ui->widgetLupadatePage, &LupdatePageController::tsFilesUpdated, ui->widgetLinguistPage, &LinguistpageController::updateFilesList);

}

MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::setInterface(const QString interface)
{
    ui->comboBoxInterface->setCurrentText(interface);
}



//bool MainWindow::load(const QString &filePath)
//{
//    if (translator.load(filePath)) {
//        populateTableWidget(translator.getTranslations());
//    } else {
//        qWarning() << "Erreur de chargement du fichier TS.";
//        return false;
//    }
//    return true;
//}

//void MainWindow::save(const QString &filePath)
//{
//    if (filePath.isEmpty()) {
//        qWarning() << "Aucun fichier sélectionné pour la sauvegarde.";
//        return;
//    }

//    // Sauvegarde des traductions mises à jour dans le fichier .ts
//    if (translator.save(filePath)) {
//        qDebug() << "Sauvegarde réussie dans" << filePath;
//    } else {
//        qWarning() << "Erreur lors de la sauvegarde du fichier TS: " << filePath;
//    }
//}

void MainWindow::on_toolButtonAddInterface_clicked()
{
    InterfaceManager addInterfaceDialog;
    // Affiche la boîte de dialogue et vérifie si elle est acceptée
    if (addInterfaceDialog.exec() == QDialog::Accepted) {
        ui->comboBoxInterface->clear(); // Efface les éléments actuels de la ComboBox

        InterfaceDictionary dictionary;
        QStringList awtFilePaths = dictionary.getAwtFilePaths();
        QSettings settings("AriyaConsulting", "MarleneTranslation");
        QString lastInterfaceUsed = settings.value("lastInterfaceUsed", "").toString();

        for (const QString &filePath : awtFilePaths) {
            QFileInfo fileInfo(filePath);
            QString fileName = fileInfo.fileName(); // Obtenir le nom du fichier

            // Ajouter le nom du fichier dans la QComboBox et stocker le chemin complet dans les données
            ui->comboBoxInterface->addItem(fileName, filePath);
        }


        ui->comboBoxInterface->setCurrentText(lastInterfaceUsed);
    }
}



void MainWindow::on_comboBoxInterface_currentTextChanged(const QString &arg1)
{
    QSettings settings("AriyaConsulting", "MarleneTranslation");
    settings.setValue("lastInterfaceUsed", arg1);

    ui->widgetLinguistPage->setInterfaceName(arg1);
}









