#include "interfacemanager.h"
#include "ui_interfacemanager.h"
#include "FileAggregator.h"
#include <QFileDialog>



InterfaceManager::InterfaceManager(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::InterfaceManager)
{
    ui->setupUi(this);
}

InterfaceManager::~InterfaceManager()
{
    delete ui;
}


void InterfaceManager::on_toolButtonHeader_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this,
                                                    tr("Sélectionner un fichier Header"),
                                                    QString(),
                                                    tr("Fichiers Header (*.json)"));

    // Si un fichier a été sélectionné, on met à jour le lineEdit et on remplit le tableau
    if (!filePath.isEmpty()) {
        ui->lineEditHeader->setText(filePath);
    }
}


void InterfaceManager::on_toolButtonBody_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this,
                                                    tr("Sélectionner un fichier Body"),
                                                    QString(),
                                                    tr("Fichiers Body (*.json)"));

    // Si un fichier a été sélectionné, on met à jour le lineEdit et on remplit le tableau
    if (!filePath.isEmpty()) {
        ui->lineEditBody->setText(filePath);
    }
}


void InterfaceManager::on_pushButtonGenerate_clicked()
{
    QDir dir;
    if (!dir.exists("configurations/interfaces")) {
        dir.mkpath("configurations/interfaces");
    }

    QList<QString> path;
    path.append(ui->lineEditHeader->text());
    path.append(ui->lineEditBody->text());

    FileAggregator newInterface(QUrl(ui->lineEditUrl->text()),  path);
    newInterface.generateAggregateFile(QString("configurations/interfaces/%1.awt").arg(ui->lineEditInterfaceName->text()));
    qDebug() << newInterface.loadAggregateFile(QString("configurations/interfaces/%1.awt").arg(ui->lineEditInterfaceName->text()));
    accept();
}

