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
    typedef enum {
        FullView,
        LUpdateView,
        LReleaseView
    } ModeView;


    explicit ProjectVue(QWidget *parent = nullptr);
    ~ProjectVue();
    void setMode(ModeView type);


    QStringList getSelectedLanguages() const;
    void save();
    void loadProject(const QString &path);
    void readOnly(bool readOnly);

    QString getProjectName() const;
    QString getSourceCodeBase() const;
    QString getTsFilesPath() const;

    bool isCustomCompilation();
    QString getCmd();

private slots:
    void on_toolButtonSourceCodePath_clicked();
    void on_toolButtonTsFilesPath_clicked();

    void on_toolButtonCompilationDirectory_clicked();

    void on_toolButtonExternalCompilator_clicked();

    void on_lineEditCompilationArguments_textChanged(const QString &arg1);

private:
    void validateProjectName(const QString &text);

private:
    Ui::ProjectVue *ui;
};

#endif // PROJECTVUE_H
