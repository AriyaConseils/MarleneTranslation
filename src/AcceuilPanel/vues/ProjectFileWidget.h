#ifndef PROJECTFILEWIDGET_H
#define PROJECTFILEWIDGET_H

#include "qapplication.h"
#include <QWidget>
#include <QGridLayout>
#include <QLabel>
#include <QPixmap>
#include <QEvent>
#include <QDebug>


class ProjectFileWidget : public QWidget {
    Q_OBJECT

public:
    explicit ProjectFileWidget(QWidget *parent = nullptr) : QWidget(parent), layout(new QGridLayout(this)), folderIcon(new QLabel(this)), projectName(new QLabel(this)), projectPath(new QLabel(this)) {
        // Configuration de l'icône de dossier
        numberVerstionLabel = new QLabel("", this);
        numberVerstionLabel->move(16,7);
        if (isDarkTheme()) {
            // Thème sombre
            folderIcon->setPixmap(QPixmap(":/icon/ressources/assets/welcome/file-and-folder_dark.png"));
            projectName->setStyleSheet("color: #CCCCCC; font-size: 15px; font-weight: bold;"); // Texte en gras pour le thème sombre
            projectPath->setStyleSheet("color: #CCCCCC; font-size: 13px;"); // Assurez-vous que le chemin du projet a également un style approprié
            numberVerstionLabel->setStyleSheet("color: #CCCCCC; font-size: 8px; font-weight: bold;"); // Assurez-vous que le chemin du projet a également un style approprié
        } else {
            // Thème clair
            folderIcon->setPixmap(QPixmap(":/icon/ressources/assets/welcome/file-and-folder.png"));
            projectName->setStyleSheet("color: #222222; font-size: 15px; font-weight: bold;"); // Texte en gras pour le thème clair
            projectPath->setStyleSheet("color: #222222; font-size: 13px;"); // Assurez-vous que le chemin du projet a également un style approprié
            numberVerstionLabel->setStyleSheet("color: #222222; font-size: 8px; font-weight: bold;"); // Assurez-vous que le chemin du projet a également un style approprié
        }
        folderIcon->setScaledContents(true);
        folderIcon->setFixedSize(17, 17);
        layout->addWidget(folderIcon, 0, 0);
        // Configuration du label du nom de projet
        layout->addWidget(projectName, 0, 1);

        // Configuration du label du chemin de projet
        layout->addWidget(projectPath, 1, 1);
        setFixedHeight(50); // Exemple de taille fixe
        this->setLayout(layout);

        this->installEventFilter(this);
    }

    bool eventFilter(QObject *watched, QEvent *event) override {
        if (watched == this) { // Vérifie si l'événement est pour ProjectFileWidget
            if (event->type() == QEvent::Enter) { // Survol du widget
                if (isDarkTheme()) {
                    projectName->setStyleSheet("color: #ADD8E6; font-size: 15px; font-weight: bold;"); // Couleur plus claire pour les thèmes sombres
                } else {
                    projectName->setStyleSheet("color: #3B5998; font-size: 15px; font-weight: bold;"); // Couleur foncée pour les thèmes clairs
                }
                return true;
            } else if (event->type() == QEvent::Leave) { // Fin du survol
                if (isDarkTheme()) {
                    projectName->setStyleSheet("color: #CCCCCC; font-size: 15px; font-weight: bold;"); // Gris clair pour les thèmes sombres
                } else {
                    projectName->setStyleSheet("color: #222222; font-size: 15px; font-weight: bold;"); // Gris foncé pour les thèmes clairs
                }
                return true;
            }
        }
        // Appel à la méthode de base pour les événements non gérés
        return QWidget::eventFilter(watched, event);
    }


    QString getProjectName() const {
        return projectName->text();
    }

    void setProjectName(const QString &name) {
        projectName->setText(name);
    }

    QString getProjectPath() const {
        return projectPath->text();
    }

    void setProjectPath(const QString &path) {
        projectPath->setText(path);
    }
    void setVersionNumber(const QString &version) {
        numberVerstionLabel->setText(version);
    }

    void setVersionNumber(int version) {
        QString space;
        if(version < 10) space = " ";
        numberVerstionLabel->setText(version? space + QString::number(version) : "" );
    }

    bool isDarkTheme()
    {
        QColor backgroundColor = QApplication::palette().window().color();
        int luminance = 0.299 * backgroundColor.red() + 0.587 * backgroundColor.green() + 0.114 * backgroundColor.blue();
        return luminance < 128;
    }

private:
    QGridLayout *layout;
    QLabel *folderIcon;
    QLabel *projectName;
    QLabel *projectPath;
    QLabel *numberVerstionLabel;
};

#endif // PROJECTFILEWIDGET_H
