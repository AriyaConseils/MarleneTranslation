#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include "chatgptclient.h"
#include "FileAggregator.h"
#include "InterfaceDictionary.h"
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
    ui->tableWidgetTsValues->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // Créer les actions du menu contextuel
    QAction *actionMerge = new QAction(tr("Merge"), this);
    QAction *actionMergeAllSelected = new QAction(tr("Merge All Selected"), this);
    QAction *actionMergeAll = new QAction(tr("Merge All"), this);
    QAction *actionUpdate = new QAction(tr("Update translation"), this);

    // Ajouter les actions au menu contextuel
    QMenu *contextMenu = new QMenu(this);
    contextMenu->addAction(actionMerge);
    contextMenu->addAction(actionMergeAllSelected);
    contextMenu->addAction(actionMergeAll);
    contextMenu->addAction(actionUpdate);

    // Connecter le clic droit pour afficher le menu contextuel
    ui->tableWidgetTsValues->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tableWidgetTsValues, &QTableWidget::customContextMenuRequested, this, [=](const QPoint &pos) {
        // Récupérer la ligne sélectionnée pour activer ou désactiver "Merge"
        QTableWidgetItem *item = ui->tableWidgetTsValues->itemAt(pos);
        bool enableMerge = false;

        if (item) {
            int row = item->row();
            QTableWidgetItem *translationItem = ui->tableWidgetTsValues->item(row, 1);
            QTableWidgetItem *generatedTranslationItem = ui->tableWidgetTsValues->item(row, 2);

            // Activer "Merge" uniquement si la colonne 1 est différente de la colonne 2
            enableMerge = (translationItem && generatedTranslationItem && translationItem->text() != generatedTranslationItem->text());
        }
        actionMerge->setEnabled(enableMerge);

        // Afficher le menu contextuel à la position du clic
        contextMenu->exec(ui->tableWidgetTsValues->viewport()->mapToGlobal(pos));
    });

    // Connecter les actions aux slots
    connect(actionMerge, &QAction::triggered, this, &MainWindow::mergeSelectedRow);
    connect(actionMergeAllSelected, &QAction::triggered, this, &MainWindow::mergeAllSelectedRows);
    connect(actionMergeAll, &QAction::triggered, this, &MainWindow::mergeAllRows);
    connect(actionUpdate, &QAction::triggered, this, &MainWindow::updateSelectedRows);

    ui->comboBoxLanguage->addItems(translator.languageNames());
    ui->comboBoxLanguage->setCurrentIndex(-1);
    connect(ui->comboBoxLanguage, &QComboBox::currentTextChanged, this, &MainWindow::on_language_Changed);
}

MainWindow::~MainWindow()
{
    delete ui;
}




void MainWindow::mergeSelectedRow()
{
    // Récupérer la ligne actuellement sélectionnée
    int row = ui->tableWidgetTsValues->currentRow();
    if (row < 0) return; // Aucune ligne sélectionnée

    QTableWidgetItem *translationItem = ui->tableWidgetTsValues->item(row, 1);
    QTableWidgetItem *generatedTranslationItem = ui->tableWidgetTsValues->item(row, 2);

    // Vérifier que les items existent et que la colonne 1 est différente de la colonne 2
    if (translationItem && generatedTranslationItem && translationItem->text() != generatedTranslationItem->text()) {
        // Mettre à jour la colonne 1 avec le texte de la colonne 2
        translationItem->setText(generatedTranslationItem->text());

        // Effacer la couleur de fond pour indiquer que la fusion a été effectuée
        generatedTranslationItem->setBackground(Qt::NoBrush);
    }
    QTableWidgetItem *sourceItem = ui->tableWidgetTsValues->item(row, 0);
    QTableWidgetItem *contextItem = ui->tableWidgetTsValues->item(row, 3);
    if(sourceItem && contextItem){
        translator.merge(contextItem->text(), sourceItem->text());
    }
}

void MainWindow::mergeAllSelectedRows()
{
    QList<QTableWidgetItem *> selectedItems = ui->tableWidgetTsValues->selectedItems();
    QSet<int> selectedRows;

    // Extraire les indices de lignes uniques des éléments sélectionnés
    for (QTableWidgetItem *item : selectedItems) {
        selectedRows.insert(item->row());
    }

    // Parcourir chaque ligne sélectionnée
    for (int row : selectedRows) {
        QTableWidgetItem *translationItem = ui->tableWidgetTsValues->item(row, 1);
        QTableWidgetItem *generatedTranslationItem = ui->tableWidgetTsValues->item(row, 2);

        // Vérifie que les items existent et que la colonne 1 est différente de la colonne 2
        if (translationItem && generatedTranslationItem && translationItem->text() != generatedTranslationItem->text()) {
            // Mettre à jour la colonne 1 avec le texte de la colonne 2
            translationItem->setText(generatedTranslationItem->text());

            // Effacer la couleur de fond pour indiquer que la fusion a été effectuée
            generatedTranslationItem->setBackground(Qt::NoBrush);
            QTableWidgetItem *sourceItem = ui->tableWidgetTsValues->item(row, 0);
            QTableWidgetItem *contextItem = ui->tableWidgetTsValues->item(row, 3);
            if(sourceItem && contextItem){
                translator.merge(contextItem->text(), sourceItem->text());
            }

        }
    }
}

void MainWindow::mergeAllRows()
{
    int rowCount = ui->tableWidgetTsValues->rowCount();

    // Parcourir toutes les lignes pour faire la fusion
    for (int row = 0; row < rowCount; ++row) {
        QTableWidgetItem *translationItem = ui->tableWidgetTsValues->item(row, 1);
        QTableWidgetItem *generatedTranslationItem = ui->tableWidgetTsValues->item(row, 2);

        // Vérifier que les items existent et que la colonne 1 est différente de la colonne 2
        if (translationItem && generatedTranslationItem && translationItem->text() != generatedTranslationItem->text()) {
            // Mettre à jour la colonne 1 avec le texte de la colonne 2
            translationItem->setText(generatedTranslationItem->text());

            // Effacer la couleur de fond pour indiquer que la fusion a été effectuée
            generatedTranslationItem->setBackground(Qt::NoBrush);
        }
    }
    translator.mergeAll();
}

void MainWindow::updateSelectedRows()
{
    // Récupérer la langue sélectionnée dans la comboBox
    QList<QTableWidgetItem *> selectedItems = ui->tableWidgetTsValues->selectedItems();
    QSet<int> selectedRows;

    // Extraire les indices de lignes uniques des éléments sélectionnés
    for (QTableWidgetItem *item : selectedItems) {
        selectedRows.insert(item->row());
    }

    // Parcourir chaque ligne sélectionnée et demander la traduction
    for (int row : selectedRows) {
        QTableWidgetItem *sourceItem = ui->tableWidgetTsValues->item(row, 0);
        QTableWidgetItem *contextItem = ui->tableWidgetTsValues->item(row, 3);

        if (sourceItem && contextItem) {
            translator.generateTranslation(ui->comboBoxInterface->currentData().toString(), contextItem->text(), sourceItem->text());
        }
    }
}


void MainWindow::on_toolButtonTsFilesPath_clicked()
{
    // Ouvre une boîte de dialogue pour sélectionner un dossier contenant des fichiers .ts
    QString directoryPath = QFileDialog::getExistingDirectory(this,
                                                              tr("Sélectionner un dossier contenant des fichiers TS"),
                                                              QString(),
                                                              QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    // Si un dossier a été sélectionné, on le parcourt pour récupérer les fichiers .ts
    if (!directoryPath.isEmpty()) {
        ui->lineEditTsFilesPath->setText(directoryPath);
        QDir dir(directoryPath);
        QStringList tsFiles = dir.entryList(QStringList() << "*.ts", QDir::Files);

        // Ajoute chaque fichier .ts trouvé dans le ListWidget
        ui->listWidgetTsList->clear();
        for (const QString &fileName : tsFiles) {
            ui->listWidgetTsList->addItem(fileName);
        }
    }
}


void MainWindow::populateTableWidget(const QMap<QString, QVector<TranslationMessage>> &translations)
{
    // Vide le tableau existant
    ui->tableWidgetTsValues->clearContents();
    ui->tableWidgetTsValues->setRowCount(0);

    // Parcourt chaque contexte et chaque message pour remplir le tableau
    int row = 0;
    for (const auto &context : translations.keys()) {
        const auto &messages = translations[context];

        for (const auto &message : messages) {
            // Ajoute une nouvelle ligne
            ui->tableWidgetTsValues->insertRow(row);

            // Crée les items pour le message source et translation
            QTableWidgetItem *sourceItem = new QTableWidgetItem(message.source);
            QTableWidgetItem *translationItem = new QTableWidgetItem(message.translation);

            // Ajoute les items dans le tableau
            ui->tableWidgetTsValues->setItem(row, 0, sourceItem);
            ui->tableWidgetTsValues->setItem(row, 1, translationItem);



            QTableWidgetItem *contextItem = new QTableWidgetItem(context);
            ui->tableWidgetTsValues->setItem(row, 3, contextItem);

            // Passe à la ligne suivante
            row++;
        }
    }
}


void MainWindow::setInterface(const QString interface)
{
    ui->comboBoxInterface->setCurrentText(interface);
}

void MainWindow::updateTranslation(const QString language, bool fullUpdate)
{
    if(translator.language() != language){
        translator.setLanguage(language);
    }
    translator.generateTranslation(ui->comboBoxInterface->currentData().toString(), fullUpdate);
    translator.waitForFinish();
    translator.mergeAll();
}


void MainWindow::on_pushButtonUpdate_clicked()
{
    // Récupérer la langue sélectionnée dans la comboBox
    QString selectedLanguage = ui->comboBoxLanguage->currentText();
    translator.generateTranslation(ui->comboBoxInterface->currentData().toString());
}


void MainWindow::on_pushButtonSave_clicked()
{
    QString directoryPath = ui->lineEditTsFilesPath->text();
    QList<QListWidgetItem *> selectedItems = ui->listWidgetTsList->selectedItems();

    if (!directoryPath.isEmpty() && !selectedItems.isEmpty()) {
        // Récupère le premier élément sélectionné
        QString fileName = selectedItems.first()->text();

        // Construit le chemin complet
        QString fullPath = directoryPath + "/" + fileName;

        // Appelle la fonction de sauvegarde avec le chemin complet
        save(fullPath);
    } else {
        // Gestion d'erreur, par exemple afficher un message si rien n'est sélectionné
        qDebug() << "Erreur : aucun fichier sélectionné ou chemin non défini.";
    }}


bool MainWindow::load(const QString &filePath)
{
    if (translator.load(filePath)) {
        populateTableWidget(translator.getTranslations());
    } else {
        qWarning() << "Erreur de chargement du fichier TS.";
        return false;
    }
    return true;
}

void MainWindow::save(const QString &filePath)
{
    if (filePath.isEmpty()) {
        qWarning() << "Aucun fichier sélectionné pour la sauvegarde.";
        return;
    }

    // Sauvegarde des traductions mises à jour dans le fichier .ts
    if (translator.save(filePath)) {
        qDebug() << "Sauvegarde réussie dans" << filePath;
    } else {
        qWarning() << "Erreur lors de la sauvegarde du fichier TS: " << filePath;
    }
}

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
}


void MainWindow::on_pushButtonSaveAs_clicked()
{
    // Ouvre une boîte de dialogue pour choisir un fichier .ts
    QString filePath = QFileDialog::getSaveFileName(this, tr("Enregistrer sous"), QString(), tr("Fichiers TS (*.ts)"));

    // Si l'utilisateur annule la boîte de dialogue, filePath sera vide
    if (filePath.isEmpty()) {
        return;
    }

    // Vérifie que le fichier a bien l'extension .ts
    if (!filePath.endsWith(".ts", Qt::CaseInsensitive)) {
        filePath += ".ts"; // Ajoute l'extension .ts si elle est manquante
    }

    // Appelle la fonction save avec le chemin du fichier
    save(filePath);
}

void MainWindow::on_language_Changed(const QString &arg1)
{
    translator.setLanguage(arg1);
}

void MainWindow::on_listWidgetTsList_itemSelectionChanged()
{
    // Récupère les éléments sélectionnés
    QList<QListWidgetItem *> selectedItems = ui->listWidgetTsList->selectedItems();

    // Vérifie s'il y a un élément sélectionné
    if (selectedItems.isEmpty()) return;

    QListWidgetItem *item = selectedItems.first();
    qDebug() << "on_listWidgetTsList_selectionChanged " << item;

    // Récupère le nom du fichier sélectionné
    QString fileName = item->text();

    // Construit le chemin complet du fichier .ts à partir du dossier sélectionné
    QString directoryPath = ui->lineEditTsFilesPath->text();
    QString filePath = QDir(directoryPath).filePath(fileName);

    // Charge et traite le fichier .ts
    if (!filePath.isEmpty()) {
        load(filePath);
        manageLanguage();

        // Connecte le signal pour mettre à jour les traductions générées
        connect(&translator, &TSTranslationReader::generatedTranslationChanged, this, [=](const QString &context, const QString &source, const QString &newTranslation) {
            int rowCount = ui->tableWidgetTsValues->rowCount();
            for (int row = 0; row < rowCount; ++row) {
                QTableWidgetItem *sourceItem = ui->tableWidgetTsValues->item(row, 0);
                QTableWidgetItem *contextItem = ui->tableWidgetTsValues->item(row, 3);

                // Vérifie que les éléments existent et que la source et le contexte correspondent
                if (sourceItem && contextItem) {
                    QString sourceText = sourceItem->text();
                    QString contextText = contextItem->text();

                    // Met à jour translationItem si la source et le contexte correspondent
                    if (sourceText == source && contextText == context) {

                        QTableWidgetItem *generatedTranslationItem = ui->tableWidgetTsValues->item(row, 2);
                        if (!generatedTranslationItem) {
                            generatedTranslationItem = new QTableWidgetItem();
                            ui->tableWidgetTsValues->setItem(row, 2, generatedTranslationItem);
                        }
                        generatedTranslationItem->setText(newTranslation);
                        QTableWidgetItem *expectedTranslationItem = ui->tableWidgetTsValues->item(row, 1);

                        if (expectedTranslationItem && !expectedTranslationItem->text().isEmpty() && expectedTranslationItem->text() != newTranslation) {
                            // Appliquer un fond orange à l'item pour indiquer la différence
                            ui->tableWidgetTsValues->item(row, 2)->setBackground(QColor(255, 165, 0));
                        }
                        break;
                    }
                }
            }
        });
    }
}

void MainWindow::manageLanguage(){
    if(translator.language() != ""){
        ui->comboBoxLanguage->setCurrentText(translator.language());
    } else {
        ui->comboBoxLanguage->setCurrentIndex(-1);
    }

}
