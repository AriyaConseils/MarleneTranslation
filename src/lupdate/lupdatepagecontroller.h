#ifndef LUPDATEPAGECONTROLLER_H
#define LUPDATEPAGECONTROLLER_H

#include <QWidget>
#include "translationextractor.h"

namespace Ui {
class LupdatePageController;
}

class LupdatePageController : public QWidget
{
    Q_OBJECT

public:
    explicit LupdatePageController(QWidget *parent = nullptr);
    ~LupdatePageController();

public slots:
    void loadProject(const QString &path);

private slots:
    void on_pushButtonUpdateTsFiles_clicked();

signals:
    void tsDirectoryChanged(const QString &);
    void tsFilesUpdated();

private:
    Ui::LupdatePageController *ui;
    TranslationExtractor *m_lupdate = nullptr;

};

#endif // LUPDATEPAGECONTROLLER_H
