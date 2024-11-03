#ifndef NEWPROJECTDIALOG_H
#define NEWPROJECTDIALOG_H

#include <QDialog>
#include <QFileDialog>
#include <QRegularExpression>
#include <QStandardPaths>
#include <QMessageBox>
#include <QSettings>
#include <QTimer>
#include <QToolButton>

namespace Ui {
class NewProjectDialog;
}

class NewProjectDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewProjectDialog(QWidget *parent = nullptr);
    ~NewProjectDialog();

    void editProject(const QString &path);

    QString getProjectName() const;

    QString getTsFilesPath() const;

    QString getSourceCodeBase() const;

    QString getProjectPath() const;

private slots:
    void on_pushButtonCancel_clicked();
    void on_pushButtonOk_clicked();

private:
    Ui::NewProjectDialog *ui;

    QString loadStyleSheet(const QString &sheetName);
    bool isSystemInDarkMode();
    QString getIconPath(const QString &baseName);
};

#endif // NEWPROJECTDIALOG_H
