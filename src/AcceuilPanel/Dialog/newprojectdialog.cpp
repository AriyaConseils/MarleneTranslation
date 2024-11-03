#include "newprojectdialog.h"
#include "ui_newprojectdialog.h"
#include "../../linguist/tstranslationreader.h"

NewProjectDialog::NewProjectDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewProjectDialog)
{
    ui->setupUi(this);
    ui->widgetProjectVue->readOnly(false);

    if (isSystemInDarkMode()) {
        this->setStyleSheet(loadStyleSheet(":/stylesheet/ressources/styles/NewProjectDialog_dark.qss"));
    } else {
        this->setStyleSheet(loadStyleSheet(":/stylesheet/ressources/styles/NewProjectDialog_light.qss"));
    }

}

NewProjectDialog::~NewProjectDialog()
{
    delete ui;
}



QString NewProjectDialog::getProjectName() const
{
    return ui->widgetProjectVue->getProjectName();
}

QString NewProjectDialog::getSourceCodeBase() const
{
    return ui->widgetProjectVue->getSourceCodeBase();
}

QString NewProjectDialog::getTsFilesPath() const
{
    return ui->widgetProjectVue->getTsFilesPath();
}

QString NewProjectDialog::getProjectPath() const
{
    return ui->widgetProjectVue->getSourceCodeBase() + "/" + ui->widgetProjectVue->getProjectName() + ".rrt";
}



void NewProjectDialog::on_pushButtonCancel_clicked()
{
    reject();
}


void NewProjectDialog::editProject(const QString &path)
{
    ui->widgetProjectVue->loadProject(path);
    ui->pushButtonOk->setText(tr("Save Project"));
}

void NewProjectDialog::on_pushButtonOk_clicked()
{
    if(getProjectName() != "" && getSourceCodeBase() != "" && getTsFilesPath() != ""){
        accept();
        ui->widgetProjectVue->save();
        return;
    }
    QMessageBox::warning(this, "Incomplete Information", "Please fill in all fields to create a new project.");
}


QString NewProjectDialog::loadStyleSheet(const QString &sheetName)
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

bool NewProjectDialog::isSystemInDarkMode()
{
#ifdef Q_OS_WIN
    QSettings settings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", QSettings::NativeFormat);
    return settings.value("AppsUseLightTheme", 1).toInt() == 0;
#elif defined(Q_OS_MAC)
    QSettings settings("Apple Global Domain", QSettings::NativeFormat);
    return settings.value("AppleInterfaceStyle", "").toString() == "Dark";
#else
    return false;
#endif
}

QString NewProjectDialog::getIconPath(const QString &baseName)
{
    QString iconColor = isSystemInDarkMode() ? "white" : "gray";
    return QString(":/welcome/ressources/welcome/%1_%2.png").arg(baseName, iconColor);
}
