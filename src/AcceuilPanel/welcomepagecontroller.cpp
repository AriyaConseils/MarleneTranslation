#include "welcomepagecontroller.h"
#include "ui_welcomepagecontroller.h"
#include <QSettings>
#include <QStandardPaths>
#include <QFile>
#include <QDesktopServices>
#include <QMouseEvent>
#include <QTimer>
#include <QLabel>
#include <QPushButton>
#include <QCheckBox>
#include <QRadioButton>
#include <QGroupBox>
#include <QComboBox>
#include "Dialog/newprojectdialog.h"
#include "Dialog/dialogabout.h"
#include "src/lrelease/translator.h"

WelcomePageController::WelcomePageController(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::WelcomePageController)
{
    ui->setupUi(this);

    m_projetsWidget = new ProjectList();

    connect(m_projetsWidget, &ProjectList::openProject, this, &WelcomePageController::addOpenedProject);
    ui->stackedWidget->addWidget(m_projetsWidget);

    on_pushButtonProjets_clicked();
    if (isSystemInDarkMode()) {
        this->setStyleSheet(loadStyleSheet(":/stylesheet/ressources/styles/welcome_dark.qss"));
    } else {
        this->setStyleSheet(loadStyleSheet(":/stylesheet/ressources/styles/welcome_light.qss"));
    }
}

WelcomePageController::~WelcomePageController()
{
    delete ui;
}


void WelcomePageController::addOpenedProject(QString path)
{
    m_projetsWidget->addOpenedProject(path);
    emit openProject(path);
}



void WelcomePageController::on_pushButtonOpenProject_clicked()
{
    // Ouvrir une boîte de dialogue pour sélectionner un fichier avec l'extension .rrt
    QString filePath = QFileDialog::getOpenFileName(this,
                                                    gtr("Open Project File").c_str(),
                                                    QString(),
                                                    gtr("Project Files (*.rrt)").c_str());

    // Vérifier si un fichier a été sélectionné
    if (filePath.isEmpty()) {
        QMessageBox::information(this, gtr("No File Selected").c_str(), gtr("Please select a valid project file.").c_str());
        return;
    }

    // Charger le fichier .rrt sélectionné
    // Par exemple, appeler une méthode de chargement de projet ici
    this->addOpenedProject(filePath);
}


void WelcomePageController::on_pushButtonNewProjet_clicked()
{
    NewProjectDialog newProject;
    if(newProject.exec() == QDialog::Accepted){
        this->addOpenedProject(newProject.getProjectPath());
    }
}


void WelcomePageController::on_pushButtonProjets_clicked()
{
    ui->labelProjet->setText(ui->pushButtonProjets->text());
    ui->pushButtonProjets->setChecked(true);
    ui->stackedWidget->setCurrentWidget(m_projetsWidget);
}



void WelcomePageController::on_pushButtonAbout_clicked()
{
    DialogAbout about;
    about.exec();
}


QString WelcomePageController::loadStyleSheet(const QString &sheetName)
{
    QFile file(sheetName);
    if (!file.open(QFile::ReadOnly)) {
        qWarning("Unable to open stylesheet file: %s", qPrintable(sheetName));
        return "";
    }
    QString styleSheet = QLatin1String(file.readAll());
    file.close();
    return styleSheet;
}

bool WelcomePageController::isSystemInDarkMode()
{
#ifdef Q_OS_WIN
    QSettings settings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", QSettings::NativeFormat);
    return settings.value("AppsUseLightTheme", 1).toInt() == 0;
#elif defined(Q_OS_MAC)
    QSettings settings("Apple Global Domain", QSettings::NativeFormat);
    return settings.value("AppleInterfaceStyle", "").toString() == "Dark";
#else
    // Pour les autres systèmes, il faudra implémenter une méthode personnalisée.
    // On retourne false par défaut.
    return false;
#endif
}


