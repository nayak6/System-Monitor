#ifndef PROCESSFILES_H
#define PROCESSFILES_H

#include <QDialog>

namespace Ui {
class ProcessFiles;
}

class ProcessFiles : public QDialog
{
    Q_OBJECT

public:
    explicit ProcessFiles(QWidget *parent = 0);
    ~ProcessFiles();
    void listOpenFiles(QString processID);

private:
    Ui::ProcessFiles *ui;
};

#endif // PROCESSFILES_H
