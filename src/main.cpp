#include "mainwindow.h"
#include <QApplication>
#include <QCommandLineParser>
#include <QDebug>
#include "src/linguist/InterfaceDictionary.h"
#include "src/lrelease/lreleasepagecontroler.h"
#include "src/lupdate/lupdatepagecontroller.h"
#include "src/linguist/linguistpagecontroller.h"


#include "src/lrelease/translator.h"



//--source ../testTStoQM/Swiizio.rrt --i18n --translate all --l10n
int main(int argc, char *argv[])
{
    LOAD_TRANSLATIONS("build/MarleneTranslation_de.qmt");
    QApplication a(argc, argv);
    QCoreApplication::setApplicationName("MarleneTranslation");
    QCoreApplication::setApplicationVersion("1.0");

    // Création d'un analyseur de ligne de commande
    QCommandLineParser parser;
    parser.setApplicationDescription("MarleneTranslation CLI/GUI application");
    parser.addHelpOption();
    parser.addVersionOption();

    // Définition des options CLI
    QCommandLineOption sourceOption(QStringList() << "s" << "source", "Source file path.", "source");
    QCommandLineOption langOption(QStringList() << "l" << "lang", "Language code for translation.", "lang");
    QCommandLineOption tsFileOption(QStringList() << "t" << "ts-file", "Path to the .ts translation file.", "ts-file");
    QCommandLineOption fullUpdateOption("fullUpdate", "If set, the entire language file will be regenerated. If not set, only sources that have not been translated yet will be processed.");
    QCommandLineOption interfaceOption(QStringList() << "i" << "interface", "Specify the interface file to use (.awt file).", "interface");
    QCommandLineOption listInterfacesOption("list-interfaces", "List all available interfaces (.awt files).");

    // Options spécifiques pour les fonctionnalités de traduction/localisation
    QCommandLineOption i18nOption("i18n", "Generate internationalization files.");
    QCommandLineOption translateOption(QStringList() << "translate", "Translate the source file. Use 'all' to translate all entries.", "all");
    QCommandLineOption l10nOption("l10n", "Compile localization files.");

    parser.addOption(sourceOption);
    parser.addOption(langOption);
    parser.addOption(fullUpdateOption);
    parser.addOption(interfaceOption);
    parser.addOption(listInterfacesOption);
    parser.addOption(i18nOption);
    parser.addOption(translateOption);
    parser.addOption(l10nOption);
    parser.addOption(tsFileOption);


    // Analyse les arguments passés à l'application
    parser.process(a);
    MainWindow w;

    // Vérifie si l'option --list-interfaces est définie
    if (parser.isSet(listInterfacesOption)) {
        InterfaceDictionary dictionary;
        QStringList interfaceNames = dictionary.getNames();

        qDebug() << "Available interfaces:";
        for (const QString &name : interfaceNames) {
            qDebug() << " - " << name;
        }

        return 0; // Fin du programme après affichage des interfaces disponibles
    }

    // Vérifie si des arguments ont été fournis
    if (argc == 1) {
        // Aucun argument, lancer l'interface graphique
        w.setWindowIcon(QIcon(":/configurations/ressources/MarleneTranslation.ico"));
        w.setWindowTitle("MarleneTranslation");
        w.show();
        return a.exec();
    } else {
        w.hide();
        // Mode CLI
        if (!parser.isSet(sourceOption)) {
            qCritical() << "Error: --source options is mandatory in CLI mode.";
            return 1; // Code d'erreur
        }

        QString source = parser.value(sourceOption);
        QString lang = parser.value(langOption);
        QString tsFile = parser.value(tsFileOption);
        bool fullUpdate = parser.isSet(fullUpdateOption);

        // Gestion de l'interface spécifiée par l'utilisateur
        QString interfaceName;
        if (parser.isSet(interfaceOption)) {
            interfaceName = parser.value(interfaceOption);
            qDebug() << "Using interface:" << interfaceName;
        } else {
            qWarning() << "No interface specified. Default interface will be used.";
        }

        // Affiche les options reçues
        qDebug() << "Source:" << source;
        qDebug() << "Lang:" << lang;
        qDebug() << "Full Update:" << (fullUpdate ? "Yes" : "No");

        // Charger le projet
        if(w.load(source)) {
            if(interfaceName != "") {
                w.setInterface(interfaceName);
            }

            // Exécute les fonctionnalités en fonction des options fournies
            if (parser.isSet(i18nOption)) {
                qDebug() << "Generating i18n files...";
                w.getLUpdate()->createFiles();
            }

            if (parser.isSet(translateOption)) {
                QString translateOptionValue = parser.value(translateOption);
                if (translateOptionValue == "all") {
                    qDebug() << "Translating all entries...";
                    w.getLInguist()->updateAllTranslation(fullUpdate);
                } else {
                    qDebug() << "Translating specific language:" << tsFile;
                    w.getLInguist()->updateTranslation(tsFile, fullUpdate);
                }
            }

            if (parser.isSet(l10nOption)) {
                qDebug() << "Building localization files...";
                w.getLRelease()->build();
            }
        }

        return 0;
    }
}
