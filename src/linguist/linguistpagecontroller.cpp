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
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);


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

    ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tableView, &QTableView::customContextMenuRequested, this, [=](const QPoint &pos) {
        QModelIndex index = ui->tableView->indexAt(pos);
        bool enableMerge = false;

        if (index.isValid()) {
            int row = index.row();

            // Récupère les indexes pour les colonnes "Translation" et "Generated Translation"
            QModelIndex translationIndex = m_currentModel->index(row, 1);
            QModelIndex generatedTranslationIndex = m_currentModel->index(row, 2);

            QString translationText = m_currentModel->data(translationIndex).toString();
            QString generatedTranslationText = m_currentModel->data(generatedTranslationIndex).toString();

            // Activer "Merge" uniquement si les textes sont différents
            enableMerge = (translationText != generatedTranslationText);
        }

        actionMerge->setEnabled(enableMerge);
        contextMenu->exec(ui->tableView->viewport()->mapToGlobal(pos));
    });

    // Connecter les actions aux slots
    connect(actionMerge, &QAction::triggered, this, &LinguistpageController::mergeSelectedRow);
    connect(actionMergeAllSelected, &QAction::triggered, this, &LinguistpageController::mergeAllSelectedRows);
    connect(actionMergeAll, &QAction::triggered, this, &LinguistpageController::mergeAllRows);
    connect(actionUpdate, &QAction::triggered, this, &LinguistpageController::updateSelectedRows);

    ui->comboBoxLanguage->setCurrentIndex(-1);
    connect(ui->comboBoxLanguage, &QComboBox::currentTextChanged, this, &LinguistpageController::updateLanguage);
}

LinguistpageController::~LinguistpageController()
{
    delete ui;
}


void LinguistpageController::mergeSelectedRow()
{
    if (!m_currentModel) {
        qDebug() << "Merge selected failed: m_currentModel = nullptr";
        return;
    }

    // Récupérer la ligne actuellement sélectionnée
    QModelIndexList selectedIndexes = ui->tableView->selectionModel()->selectedRows();
    if (selectedIndexes.isEmpty()) return; // Aucune ligne sélectionnée

    int row = selectedIndexes.first().row(); // Première ligne sélectionnée

    // Obtenir les textes de source et context
    QModelIndex contextIndex = m_currentModel->index(row, 0);
    QModelIndex sourceIndex = m_currentModel->index(row, 1);

    QString sourceText = m_currentModel->data(sourceIndex).toString();
    QString contextText = m_currentModel->data(contextIndex).toString();

    // Appeler la fonction merge du modèle
    if (m_currentModel->merge(contextText, sourceText)) {
        qDebug() << "Row merged successfully for context:" << contextText << "source:" << sourceText;
    } else {
        qDebug() << "Merge failed: context or source not found";
    }
}

void LinguistpageController::mergeAllSelectedRows()
{
    if (!m_currentModel) {
        qDebug() << "Merge All selected failed: m_currentModel = nullptr";
        return;
    }

    // Récupérer les lignes sélectionnées
    QModelIndexList selectedIndexes = ui->tableView->selectionModel()->selectedRows();
    QSet<int> selectedRows;
    for (const QModelIndex &index : selectedIndexes) {
        selectedRows.insert(index.row()); // Récupère les lignes uniques
    }

    // Parcourir chaque ligne sélectionnée
    for (int row : selectedRows) {
        QModelIndex translationIndex = m_currentModel->index(row, 1); // Colonne 1 : Translation
        QModelIndex generatedTranslationIndex = m_currentModel->index(row, 2); // Colonne 2 : Generated Translation

        QString translationText = m_currentModel->data(translationIndex).toString();
        QString generatedTranslationText = m_currentModel->data(generatedTranslationIndex).toString();

        // Vérifier que la traduction et la traduction générée sont différentes
        if (translationText != generatedTranslationText) {
            // Met à jour la traduction avec la traduction générée
            m_currentModel->setData(translationIndex, generatedTranslationText);

            // Effacer la couleur de fond pour indiquer que la fusion a été effectuée
            m_currentModel->setData(generatedTranslationIndex, QVariant(), Qt::BackgroundRole);

            // Obtenir les valeurs de "Source" et "Context" pour effectuer la fusion dans le modèle
            QModelIndex contextIndex = m_currentModel->index(row, 0);
            QModelIndex sourceIndex = m_currentModel->index(row, 1);

            QString sourceText = m_currentModel->data(sourceIndex).toString();
            QString contextText = m_currentModel->data(contextIndex).toString();

            // Appeler la méthode merge du modèle pour la ligne actuelle
            if (!m_currentModel->merge(contextText, sourceText)) {
                qWarning() << "Merge failed for context:" << contextText << "source:" << sourceText;
            }
        }
    }
}

void LinguistpageController::mergeAllRows()
{
    if (!m_currentModel) {
        qDebug() << "Merge all failed: m_currentModel = nullptr";
        return;
    }

    int rowCount = m_currentModel->rowCount();

    // Parcourir toutes les lignes pour faire la fusion
    for (int row = 0; row < rowCount; ++row) {
        QModelIndex translationIndex = m_currentModel->index(row, 1); // Colonne 1 : Translation
        QModelIndex generatedTranslationIndex = m_currentModel->index(row, 2); // Colonne 2 : Generated Translation

        QString translationText = m_currentModel->data(translationIndex).toString();
        QString generatedTranslationText = m_currentModel->data(generatedTranslationIndex).toString();

        // Vérifier que la traduction et la traduction générée sont différentes
        if (translationText != generatedTranslationText) {
            // Met à jour la traduction avec la traduction générée
            m_currentModel->setData(translationIndex, generatedTranslationText);

            // Effacer la couleur de fond pour indiquer que la fusion a été effectuée
            m_currentModel->setData(generatedTranslationIndex, QVariant(), Qt::BackgroundRole);
        }
    }

    // Appeler la méthode de fusion de toutes les lignes dans le modèle
    m_currentModel->mergeAll();
}

void LinguistpageController::updateSelectedRows()
{
    if (m_interface.isEmpty()) {
        qDebug() << "Interface not defined!";
        return;
    }

    if (!m_currentModel) {
        qDebug() << "Update selected failed: m_currentModel = nullptr";
        return;
    }

    // Obtenir les lignes sélectionnées
    QModelIndexList selectedIndexes = ui->tableView->selectionModel()->selectedRows();
    QSet<int> selectedRows;

    // Extraire les indices de lignes uniques des éléments sélectionnés
    for (const QModelIndex &index : selectedIndexes) {
        selectedRows.insert(index.row());
    }

    // Parcourir chaque ligne sélectionnée et demander la traduction
    for (int row : selectedRows) {
        QModelIndex contextIndex = m_currentModel->index(row, 0); // Colonne 0 : Context
        QModelIndex sourceIndex = m_currentModel->index(row, 1); // Colonne 1 : Source

        QString contextText = m_currentModel->data(contextIndex).toString();
        QString sourceText = m_currentModel->data(sourceIndex).toString();

        if (!sourceText.isEmpty() && !contextText.isEmpty()) {
            m_currentModel->generateTranslation(m_interface, contextText, sourceText);
        }
    }
}


void LinguistpageController::setInterfaceName(const QString &interface)
{
    m_interface = InterfaceDictionary::interfaceFromName(interface);
}

void LinguistpageController::updateLanguage(const QString &arg1)
{
    if(m_currentModel){
        m_currentModel->setLanguage(arg1);
    }
}

void LinguistpageController::manageLanguage()
{
    if(m_currentModel && m_currentModel->language() != ""){
        ui->comboBoxLanguage->setCurrentText(m_currentModel->language());
    } else {
        ui->comboBoxLanguage->setCurrentIndex(-1);
    }

}

//void LinguistpageController::updateTranslation(const QString language, bool fullUpdate)
//{
//    if(m_interface == ""){
//        qDebug() << "Interface not defined!";
//    }
//    if(translator.language() != language){
//        translator.setLanguage(language);
//    }
//    translator.generateTranslation(m_interface, fullUpdate);
//    translator.waitForFinish();
//    translator.mergeAll();
//}


void LinguistpageController::on_pushButtonUpdate_clicked()
{
    if(m_interface == ""){
        qDebug() << "Interface not defined!";
    }
    if(!m_currentModel){
        qDebug() << "Update failed: m_currentModel = nullptr";
        return;
    }

    m_currentModel->generateTranslation(m_interface);
}

void LinguistpageController::updateFilesList()
{
    QDir dir(m_currentTsFilesDirectory);
    QStringList tsFiles = dir.entryList(QStringList() << "*.ts", QDir::Files);

    // Vide la liste actuelle
    ui->listWidgetTsList->clear();
    clearAndDeleteModels();
    manageLanguage();
    // Ajoute chaque fichier .ts avec le drapeau correspondant dans le ListWidget
    for (const QString &fileName : tsFiles) {
        // Extraction du code de langue depuis le nom du fichier
        QString languageCode = fileName.split(".ts").first().split("_").last();

        // Récupère l'icône du drapeau pour ce code de langue
        QIcon flagIcon = FlagsPickerBox::getFlagIcon(languageCode);

        // Crée un item pour le fichier .ts et l'ajouter
        QListWidgetItem *item = new QListWidgetItem(flagIcon, fileName);
        ui->listWidgetTsList->addItem(item);

        // Charge un nouveau model de traduction
        TSTranslationModel *currentModel = new TSTranslationModel(nullptr);
        currentModel->load(m_currentTsFilesDirectory + "/" + fileName);
        currentModel->setLanguage(FlagsPickerBox::languageFromCodeLanguage(languageCode));
        connect(currentModel, &TSTranslationModel::durtyChanged, this, [item](bool durty){
            if(durty){
                if(!item->text().contains("*")){
                    item->setText(item->text() + "*");
                }
            } else {
                item->setText(item->text().remove("*"));
            }
        });

        m_modelTsFileDictionary.insert(m_currentTsFilesDirectory + "/" + fileName, currentModel);
        // Détermine la couleur de fond semi-transparente de l'item
        if (currentModel->isEmpty()) {
            item->setBackground(QBrush(QColor(255, 0, 0, 30)));   // Rouge transparent
        } else if (currentModel->isFullyTranslated()) {
            item->setBackground(QBrush(QColor(0, 255, 0, 30)));   // Vert transparent
        } else {
            item->setBackground(QBrush(QColor(255, 165, 0, 30))); // Orange transparent
        }
    }
}




void LinguistpageController::setTsFilesDirectory(const QString &path)
{
    m_currentTsFilesDirectory = path;
    updateFilesList();
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
    if(!m_currentModel){
        qDebug() << "Save failed: m_currentModel = nullptr";
        return;
    }

    if (filePath.isEmpty()) {
        qWarning() << "Aucun fichier sélectionné pour la sauvegarde.";
        return;
    }

    if (m_currentModel->save(filePath)){
        qDebug() << "Sauvegarde réussie dans" << filePath;
    } else {
        qWarning() << "Erreur lors de la sauvegarde du fichier TS: " << filePath;
    }
}


void LinguistpageController::on_pushButtonSave_clicked()
{
    QList<QListWidgetItem *> selectedItems = ui->listWidgetTsList->selectedItems();

    if (!m_currentTsFilesDirectory.isEmpty() && !selectedItems.isEmpty()) {
        // Récupère le premier élément sélectionné
        QString fileName = selectedItems.first()->text().remove("*");

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
    QString fileName = item->text().remove("*");

    // Construit le chemin complet du fichier .ts à partir du dossier sélectionné
    QString filePath = QDir(m_currentTsFilesDirectory).filePath(fileName);

    // Change de model
    if (!filePath.isEmpty()) {
        m_currentModel = m_modelTsFileDictionary.value(filePath);
        ui->tableView->setModel(m_currentModel);
        manageLanguage();
    }
}

void LinguistpageController::clearAndDeleteModels()
{
    // S'assurer que le QTableView n'a pas de modèle actif
    ui->tableView->setModel(nullptr);

    // Parcourir et supprimer chaque modèle dans le dictionnaire
    for (auto model : m_modelTsFileDictionary) {
        delete model;
    }

    // Vider le dictionnaire
    m_modelTsFileDictionary.clear();

    // Réinitialiser le modèle actuel à nullptr pour éviter tout accès invalidé
    m_currentModel = nullptr;
}
