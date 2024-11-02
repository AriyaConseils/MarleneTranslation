#include "mainwindow.h"
#include <QApplication>
#include <QCommandLineParser>
#include <QDebug>
#include "InterfaceDictionary.h"

int main(int argc, char *argv[])
{
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
    QCommandLineOption destinationOption(QStringList() << "d" << "destination", "Destination file path.", "destination");
    QCommandLineOption fullUpdateOption("fullUpdate", "If set, the entire language file will be regenerated. If not set, only sources that have not been translated yet will be processed.");
    QCommandLineOption interfaceOption(QStringList() << "i" << "interface", "Specify the interface file to use (.awt file).", "interface");
    QCommandLineOption listInterfacesOption("list-interfaces", "List all available interfaces (.awt files).");

    parser.addOption(sourceOption);
    parser.addOption(langOption);
    parser.addOption(destinationOption);
    parser.addOption(fullUpdateOption);
    parser.addOption(interfaceOption);
    parser.addOption(listInterfacesOption);

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
        if (!parser.isSet(sourceOption) || !parser.isSet(langOption)) {
            qCritical() << "Error: --source and --lang options are mandatory in CLI mode.";
            return 1; // Code d'erreur
        }

        QString source = parser.value(sourceOption);
        QString lang = parser.value(langOption);
        QString destination = parser.isSet(destinationOption) ? parser.value(destinationOption) : source;
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
        qDebug() << "Destination:" << destination;
        qDebug() << "Full Update:" << (fullUpdate ? "Yes" : "No");

        // Exemple de traitement en mode CLI (à remplacer par votre logique réelle)
        qDebug() << "Processing with the following parameters:";
        qDebug() << "Source:" << source;
        qDebug() << "Language:" << lang;
        qDebug() << "Destination:" << destination;
        qDebug() << "Full Update:" << fullUpdate;
        //--source C:/Users/admate/Desktop/fileTranslation_en.ts --lang italian --fullUpdate --destination C:/Users/admate/Desktop/fileTranslation_it.ts

        if(w.load(source)){
            if(interfaceName != ""){
                w.setInterface(interfaceName);
            }
            w.updateTranslation(lang, fullUpdate);
            w.save(destination);
        }
        // Fin du mode CLI
        return 0;
    }
}
