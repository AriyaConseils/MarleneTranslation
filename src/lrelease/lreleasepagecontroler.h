#ifndef LRELEASEPAGECONTROLER_H
#define LRELEASEPAGECONTROLER_H

#include <QWidget>

namespace Ui {
class LReleasePageControler;
}

class LReleasePageControler : public QWidget
{
    Q_OBJECT

public:
    explicit LReleasePageControler(QWidget *parent = nullptr);
    ~LReleasePageControler();

    void build();
public slots:
    void updateFilesList(const QString &path);
    void loadProject(const QString &path);

private slots:
    void on_pushButtonBuild_clicked();

private:
    Ui::LReleasePageControler *ui;
    QString m_tsDirectory;
};

#endif // LRELEASEPAGECONTROLER_H
