#include "projectvue.h"
#include "ui_projectvue.h"
#include <QFileDialog>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QMessageBox>

#include "../linguist/widgets/FlagsPickerBox.h"


ProjectVue::ProjectVue(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ProjectVue)
{
    ui->setupUi(this);
    connect(ui->lineEditProjectName, &QLineEdit::textChanged, this, &ProjectVue::validateProjectName);

    // Utilise la liste des langues disponibles à partir de FlagsPickerBox
    foreach (const QString &languageName, FlagsPickerBox::availableLanguages()) {
        // Obtenir l'icône de drapeau correspondant au code de langue/région
        QIcon flagIcon = FlagsPickerBox::getFlagIcon(languageName);
        // Vérification si l'icône de drapeau est valide
        if (flagIcon.isNull()) {
            qDebug() << "Drapeau non trouvé pour le code de langue :" << languageName;
            continue;
        }
        // Créer un QListWidgetItem avec l'icône de drapeau et le nom de la langue
        QListWidgetItem *item = new QListWidgetItem(flagIcon, languageName);
        item->setCheckState(Qt::Unchecked); // Définir l'état non coché par défaut

        // Ajouter l'élément dans la liste des langues
        ui->listWidgetLanguage->addItem(item);
    }
}



ProjectVue::~ProjectVue()
{
    delete ui;
}


QStringList ProjectVue::getSelectedLanguages() const
{
    QStringList selectedLanguages;
    for (int i = 0; i < ui->listWidgetLanguage->count(); ++i) {
        QListWidgetItem *item = ui->listWidgetLanguage->item(i);
        if (item->checkState() == Qt::Checked) {
            selectedLanguages.append(item->text());
        }
    }
    return selectedLanguages;
}

void ProjectVue::save()
{
    // Vérifier que le nom du projet est valide
    QString projectName = getProjectName();
    if (projectName.isEmpty()) {
        QMessageBox::warning(this, "Invalid Project Name", "Please enter a valid project name.");
        return;
    }

    if (getSourceCodeBase().isEmpty()) {
        QMessageBox::warning(this, "Empty Project path", "Please enter a valid project path.");
        return;
    }

    // Construire le chemin complet du fichier .rrt
    QString filePath = getSourceCodeBase() + "/" + projectName + ".rrt";

    // Créer l'objet JSON avec les informations du projet
    QJsonObject projectJson;
    projectJson["project_name"] = projectName;
    projectJson["ts_files_path"] = getTsFilesPath();

    // Ajouter la liste des langues sélectionnées
    QJsonArray languagesArray;
    for (const QString &language : getSelectedLanguages()) {
        languagesArray.append(language);
    }
    projectJson["languages"] = languagesArray;

    // Sauvegarder dans un fichier JSON
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(this, "Save Error", "Could not save the project file.");
        return;
    }

    QJsonDocument jsonDoc(projectJson);
    file.write(jsonDoc.toJson());
    file.close();

    QMessageBox::information(this, "Save Successful", "Project saved successfully.");
}

void ProjectVue::loadProject(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, "Load Error", "Could not open the project file.");
        return;
    }

    QJsonDocument jsonDoc = QJsonDocument::fromJson(file.readAll());
    file.close();

    if (!jsonDoc.isObject()) {
        QMessageBox::warning(this, "Load Error", "Invalid project file format.");
        return;
    }

    QJsonObject projectJson = jsonDoc.object();

    // Charger les informations dans l'interface utilisateur
    ui->lineEditProjectName->setText(projectJson["project_name"].toString());

    // Obtenir le dossier contenant le fichier path
    QFileInfo fileInfo(path);
    QString sourceCodeBasePath = fileInfo.absolutePath();
    ui->lineEditSourceCodeBase->setText(sourceCodeBasePath);

    ui->lineEditTsFilesPath->setText(projectJson["ts_files_path"].toString());

    // Charger les langues sélectionnées
    QJsonArray languagesArray = projectJson["languages"].toArray();
    QStringList selectedLanguages;
    for (const QJsonValue &lang : languagesArray) {
        selectedLanguages.append(lang.toString());
    }

    // Mettre à jour l'état des langues dans listWidgetLanguage
    for (int i = 0; i < ui->listWidgetLanguage->count(); ++i) {
        QListWidgetItem *item = ui->listWidgetLanguage->item(i);
        item->setCheckState(selectedLanguages.contains(item->text()) ? Qt::Checked : Qt::Unchecked);
    }
    readOnly(true);
}

void ProjectVue::readOnly(bool readOnly)
{
    ui->lineEditProjectName->setReadOnly(readOnly);

    // Afficher uniquement les langues sélectionnées
    for (int i = 0; i < ui->listWidgetLanguage->count(); ++i) {
        QListWidgetItem *item = ui->listWidgetLanguage->item(i);
        item->setHidden(readOnly && item->checkState() != Qt::Checked);
    }

    // Masquer les boutons de sélection de chemin si en mode lecture seule
    ui->toolButtonSourceCodePath->setVisible(!readOnly);
    ui->toolButtonTsFilesPath->setVisible(!readOnly);
}

void ProjectVue::validateProjectName(const QString &text)
{
    QRegularExpression re("[\\\\/:*?\"<>|]");
    QRegularExpressionMatch match = re.match(text);
    if (text.isEmpty() || match.hasMatch()) {
        ui->lineEditProjectName->setStyleSheet("color: red;");
    } else {
        ui->lineEditProjectName->setStyleSheet("");
    }
}

QString ProjectVue::getProjectName() const
{
    return ui->lineEditProjectName->text();
}

QString ProjectVue::getSourceCodeBase() const
{
    return ui->lineEditSourceCodeBase->text();
}

QString ProjectVue::getTsFilesPath() const
{
    return ui->lineEditTsFilesPath->text();
}

void ProjectVue::on_toolButtonSourceCodePath_clicked()
{
    // Ouvre une boîte de dialogue pour sélectionner un dossier contenant des fichiers sources (cpp et ui)
    QString directoryPath = QFileDialog::getExistingDirectory(this,
                                                              tr("Sélectionner un dossier contenant des fichiers src"),
                                                              QString(),
                                                              QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    // Si un dossier a été sélectionné, on le parcourt pour récupérer les fichiers .ts
    if (!directoryPath.isEmpty()) {
        ui->lineEditSourceCodeBase->setText(directoryPath);
    }
}


void ProjectVue::on_toolButtonTsFilesPath_clicked()
{
    QString directoryPath = QFileDialog::getExistingDirectory(this,
                                                              tr("Sélectionner un dossier contenant des fichiers TS"),
                                                              QString(),
                                                              QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    // Si un dossier a été sélectionné, on le parcourt pour récupérer les fichiers .ts
    if (!directoryPath.isEmpty()) {
        ui->lineEditTsFilesPath->setText(directoryPath);
    }
}
