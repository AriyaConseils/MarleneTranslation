#include "linguistpagecontroller.h"
#include "ui_linguistpagecontroller.h"
#include <QMenu>
#include <QDir>
#include <QFileDialog>
#include "InterfaceDictionary.h"

LinguistpageController::LinguistpageController(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LinguistpageController)
{
    ui->setupUi(this);
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
    connect(actionMerge, &QAction::triggered, this, &LinguistpageController::mergeSelectedRow);
    connect(actionMergeAllSelected, &QAction::triggered, this, &LinguistpageController::mergeAllSelectedRows);
    connect(actionMergeAll, &QAction::triggered, this, &LinguistpageController::mergeAllRows);
    connect(actionUpdate, &QAction::triggered, this, &LinguistpageController::updateSelectedRows);

    ui->comboBoxLanguage->setCurrentIndex(-1);
    connect(ui->comboBoxLanguage, &QComboBox::currentTextChanged, this, &LinguistpageController::updateLanguage);


    // modification des traductions à la main
    connect(ui->tableWidgetTsValues, &QTableWidget::itemChanged, this, [=](QTableWidgetItem *item) {
        // Vérifiez si l'item modifié est dans la colonne de traduction (colonne 1)
        if (item && item->column() == 1) {
            int row = item->row();

            // Récupère les valeurs nécessaires
            QString newTranslation = item->text(); // Nouvelle traduction entrée par l'utilisateur
            QTableWidgetItem *sourceItem = ui->tableWidgetTsValues->item(row, 0);
            QTableWidgetItem *contextItem = ui->tableWidgetTsValues->item(row, 3);

            // Vérifie que les items source et contexte existent
            if (sourceItem && contextItem) {
                QString sourceText = sourceItem->text();
                QString contextText = contextItem->text();

                // Mise à jour de la traduction dans translator
                translator.updateTranslation(contextText, sourceText, newTranslation);
                qDebug() << "Mise à jour de la traduction:" << newTranslation << "pour le contexte:" << contextText << "et source:" << sourceText;
            }
        }
    });


}

LinguistpageController::~LinguistpageController()
{
    delete ui;
}


void LinguistpageController::mergeSelectedRow()
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

void LinguistpageController::mergeAllSelectedRows()
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

void LinguistpageController::mergeAllRows()
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

void LinguistpageController::updateSelectedRows()
{
    if(m_interface == ""){
        qDebug() << "Interface not defined!";
    }

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
            translator.generateTranslation(m_interface, contextItem->text(), sourceItem->text());
        }
    }
}

void LinguistpageController::setInterfaceName(const QString &interface)
{
    m_interface = InterfaceDictionary::interfaceFromName(interface);
}

void LinguistpageController::updateLanguage(const QString &arg1)
{
    translator.setLanguage(arg1);
}

void LinguistpageController::manageLanguage()
{
    if(translator.language() != ""){
        ui->comboBoxLanguage->setCurrentText(translator.language());
    } else {
        ui->comboBoxLanguage->setCurrentIndex(-1);
    }

}

void LinguistpageController::updateTranslation(const QString language, bool fullUpdate)
{
    if(m_interface == ""){
        qDebug() << "Interface not defined!";
    }
    if(translator.language() != language){
        translator.setLanguage(language);
    }
    translator.generateTranslation(m_interface, fullUpdate);
    translator.waitForFinish();
    translator.mergeAll();
}


void LinguistpageController::on_pushButtonUpdate_clicked()
{
    if(m_interface == ""){
        qDebug() << "Interface not defined!";
    }
    // Récupérer la langue sélectionnée dans la comboBox
    QString selectedLanguage = ui->comboBoxLanguage->currentText();
    translator.generateTranslation(m_interface);
}

void LinguistpageController::updateFilesList()
{
    QDir dir(m_currentTsFilesDirectory);
    QStringList tsFiles = dir.entryList(QStringList() << "*.ts", QDir::Files);

    // Vide la liste actuelle
    ui->listWidgetTsList->clear();

    // Ajoute chaque fichier .ts avec le drapeau correspondant dans le ListWidget
    for (const QString &fileName : tsFiles) {
        // Extraction du code de langue depuis le nom du fichier
        QString languageCode = fileName.split(".ts").first().split("_").last();

        // Récupère l'icône du drapeau pour ce code de langue
        QIcon flagIcon = FlagsPickerBox::getFlagIcon(languageCode);

        // Crée un item pour le fichier .ts
        QListWidgetItem *item = new QListWidgetItem(flagIcon, fileName);

        // Charge le fichier dans le `translator` pour vérifier l'état des traductions
        translator.load(m_currentTsFilesDirectory + "/" + fileName);

        // Détermine la couleur de fond semi-transparente de l'item
        if (translator.isEmpty()) {
            item->setBackground(QBrush(QColor(255, 0, 0, 30)));   // Rouge transparent
        } else if (translator.isFullyTranslated()) {
            item->setBackground(QBrush(QColor(0, 255, 0, 30)));   // Vert transparent
        } else {
            item->setBackground(QBrush(QColor(255, 165, 0, 30))); // Orange transparent
        }

        // Ajoute l'item dans le ListWidget
        ui->listWidgetTsList->addItem(item);
    }
}




void LinguistpageController::setTsFilesDirectory(const QString &path)
{
    m_currentTsFilesDirectory = path;
    updateFilesList();
}

void LinguistpageController::populateTableWidget(const QMap<QString, QVector<TranslationMessage>> &translations)
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

            // Crée les items pour le message source, traduction, et contexte
            QTableWidgetItem *sourceItem = new QTableWidgetItem(message.source);
            QTableWidgetItem *translationItem = new QTableWidgetItem(message.translation);
            QTableWidgetItem *generatedTranslationItem = new QTableWidgetItem(message.generatedTranslation);
            QTableWidgetItem *contextItem = new QTableWidgetItem(context);

            // Rendre les cellules source et contexte en lecture seule
            sourceItem->setFlags(sourceItem->flags() & ~Qt::ItemIsEditable);
            generatedTranslationItem->setFlags(generatedTranslationItem->flags() & ~Qt::ItemIsEditable);
            contextItem->setFlags(contextItem->flags() & ~Qt::ItemIsEditable);

            // Ajoute les items dans le tableau
            ui->tableWidgetTsValues->setItem(row, 0, sourceItem);
            ui->tableWidgetTsValues->setItem(row, 1, translationItem);
            ui->tableWidgetTsValues->setItem(row, 2, generatedTranslationItem);
            ui->tableWidgetTsValues->setItem(row, 3, contextItem);

            row++;
        }
    }
}






void LinguistpageController::on_pushButtonSaveAs_clicked()
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

void LinguistpageController::save(const QString &filePath)
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

bool LinguistpageController::load(const QString &filePath)
{
    if (translator.load(filePath)) {
        populateTableWidget(translator.getTranslations());
    } else {
        qWarning() << "Erreur de chargement du fichier TS.";
        return false;
    }
    return true;
}

void LinguistpageController::on_pushButtonSave_clicked()
{
    QList<QListWidgetItem *> selectedItems = ui->listWidgetTsList->selectedItems();

    if (!m_currentTsFilesDirectory.isEmpty() && !selectedItems.isEmpty()) {
        // Récupère le premier élément sélectionné
        QString fileName = selectedItems.first()->text();

        // Construit le chemin complet
        QString fullPath = m_currentTsFilesDirectory + "/" + fileName;

        // Appelle la fonction de sauvegarde avec le chemin complet
        save(fullPath);
    } else {
        // Gestion d'erreur, par exemple afficher un message si rien n'est sélectionné
        qDebug() << "Erreur : aucun fichier sélectionné ou chemin non défini.";
    }
}


void LinguistpageController::on_listWidgetTsList_itemSelectionChanged()
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
    QString filePath = QDir(m_currentTsFilesDirectory).filePath(fileName);

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
