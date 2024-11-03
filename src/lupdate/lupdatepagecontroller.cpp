#include "lupdatepagecontroller.h"
#include "ui_lupdatepagecontroller.h"
#include <QFileInfo>



LupdatePageController::LupdatePageController(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LupdatePageController)
{
    ui->setupUi(this);


}

LupdatePageController::~LupdatePageController()
{
    delete ui;
}


void LupdatePageController::loadProject(const QString &path)
{
    ui->widgetProjectVue->loadProject(path);

    emit tsDirectoryChanged(ui->widgetProjectVue->getTsFilesPath());
    QFileInfo fileInfo(path);
    QString directoryPath = fileInfo.absolutePath();

    // Utiliser ce chemin pour initialiser m_lupdate
    m_lupdate = new TranslationExtractor(directoryPath.toStdString());
    m_lupdate->load();
}


void LupdatePageController::on_pushButtonUpdateTsFiles_clicked()
{
    // Vérification des champs obligatoires
    if (ui->widgetProjectVue->getProjectName().isEmpty() ||
        ui->widgetProjectVue->getSourceCodeBase().isEmpty() ||
        ui->widgetProjectVue->getTsFilesPath().isEmpty()) {
        qDebug() << "Veuillez remplir tous les champs requis.";
        return;
    }

    if(!m_lupdate){
        qDebug() << "Not a valide project!";
    }

    // Nom de base pour les fichiers de traduction
    QString projectName = ui->widgetProjectVue->getProjectName();
    QString filesDirectory = ui->widgetProjectVue->getTsFilesPath();
    // Boucle sur tous les éléments de listWidgetLanguage
    foreach(QString languageName, ui->widgetProjectVue->getSelectedLanguages()) {
        // Appeler setLanguage avec le nom de la langue
        m_lupdate->setLanguage(languageName.toStdString());
        QString languageCode = m_lupdate->getAssociatedCode(languageName.toStdString()).c_str();
        languageCode = languageCode.split("_").first();
        // Construire le nom de fichier de sortie
        QString outputFileName = filesDirectory + "/" + projectName + "_" + languageCode + ".ts";

        // Sauvegarder le fichier de traduction pour cette langue
        m_lupdate->saveToTSFile(outputFileName.toStdString());
    }
    emit tsFilesUpdated();
}
