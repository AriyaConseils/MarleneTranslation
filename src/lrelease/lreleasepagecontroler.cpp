#include "lreleasepagecontroler.h"
#include "ui_lreleasepagecontroler.h"
#include <QDir>
#include <QFileInfo>
#include <QDebug>
#include <QProcess>
#include <QEventLoop>

LReleasePageControler::LReleasePageControler(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LReleasePageControler)
{
    ui->setupUi(this);
    ui->widgetProjectVue->setMode(ProjectVue::LReleaseView);
}

LReleasePageControler::~LReleasePageControler()
{
    delete ui;
}

void LReleasePageControler::updateFilesList(const QString &path)
{
    m_tsDirectory = path;
}


void LReleasePageControler::loadProject(const QString &path)
{
    ui->widgetProjectVue->loadProject(path);

}

void LReleasePageControler::build()
{
    if (ui->widgetProjectVue->isCustomCompilation()) {
        QString cmd = ui->widgetProjectVue->getCmd();
        QString tsFilesPath = ui->widgetProjectVue->getTsFilesPath();

        // Obtenir tous les fichiers .ts dans le répertoire tsFilesPath
        QDir dir(tsFilesPath);
        QStringList tsFiles = dir.entryList(QStringList() << "*.ts", QDir::Files);

        foreach (const QString &tsFileName, tsFiles) {
            QString tmpCmd = cmd;

            QString filePath = QDir::toNativeSeparators(dir.absoluteFilePath(tsFileName));
            QString fileDirectory = QDir::toNativeSeparators(dir.absolutePath());
            QString baseName = QFileInfo(tsFileName).completeBaseName();

            // Remplacer les placeholders
            tmpCmd.replace("${sourceDir}", fileDirectory);
            tmpCmd.replace("${sourcePath}", filePath);
            tmpCmd.replace("${sourceName}", baseName);

            // Commande complète avec cmd.exe
            QString fullCmd = QString("cmd.exe /C \"%1\"").arg(tmpCmd);

            qDebug() << "Commande exécutée pour" << tsFileName << ":" << fullCmd;

            QProcess process;
            QEventLoop loop;

            connect(&process, &QProcess::readyReadStandardOutput, [&process]() {
                qDebug() << process.readAllStandardOutput();
            });
            connect(&process, &QProcess::readyReadStandardError, [&process]() {
                qDebug() << process.readAllStandardError();
            });
            connect(&process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
                    &loop, &QEventLoop::quit);

            process.start("cmd.exe", QStringList() << "/C" << tmpCmd);

            if (!process.waitForStarted()) {
                qWarning() << "Le processus n'a pas pu démarrer pour" << tsFileName;
                continue;
            }

            loop.exec();

            // Vérifier les erreurs après la fin du processus
            if (process.exitStatus() != QProcess::NormalExit || process.exitCode() != 0) {
                qWarning() << "Erreur lors de l'exécution de la commande pour" << tsFileName;
            }
        }
    }
}

void LReleasePageControler::on_pushButtonBuild_clicked()
{
   build();
}


