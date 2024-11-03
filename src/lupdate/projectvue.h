#ifndef PROJECTVUE_H
#define PROJECTVUE_H

#include <QWidget>

namespace Ui {
class ProjectVue;
}

class ProjectVue : public QWidget
{
    Q_OBJECT

public:
    explicit ProjectVue(QWidget *parent = nullptr);
    ~ProjectVue();

    QStringList getSelectedLanguages() const;
    void save();
    void loadProject(const QString &path);
    void readOnly(bool readOnly);

    QString getProjectName() const;
    QString getSourceCodeBase() const;
    QString getTsFilesPath() const;

private slots:
    void on_toolButtonSourceCodePath_clicked();
    void on_toolButtonTsFilesPath_clicked();

private:
    void validateProjectName(const QString &text);

private:
    Ui::ProjectVue *ui;
};

#endif // PROJECTVUE_H
