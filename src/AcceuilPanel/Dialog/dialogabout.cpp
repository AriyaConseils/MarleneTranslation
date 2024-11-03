#include "dialogabout.h"
#include "ui_dialogabout.h"
#include <QFile>
#include <QSettings>


DialogAbout::DialogAbout(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DialogAbout)
{
    ui->setupUi(this);
    this->setFixedSize(710,610);

    if (isSystemInDarkMode()) {
        this->setStyleSheet(loadStyleSheet(":/stylesheet/ressources/styles/about_dark.qss"));
    } else {
        this->setStyleSheet(loadStyleSheet(":/stylesheet/ressources/styles/about_light.qss"));
    }
}

DialogAbout::~DialogAbout()
{
    delete ui;
}

void DialogAbout::on_pushButtonOk_clicked()
{
    this->accept();
}



QString DialogAbout::loadStyleSheet(const QString &sheetName)
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

bool DialogAbout::isSystemInDarkMode()
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
