#ifndef MYPROPERTIES_H
#define MYPROPERTIES_H

#include <QMainWindow>

namespace Ui {
class MyProperties;
}

class MyProperties : public QMainWindow
{
    Q_OBJECT

public:
    explicit MyProperties(QWidget *parent = 0);
    ~MyProperties();
    void showProperties(QString processID, QString processName, QString CPU_Per, QString Memory_MIB, QString CurrentState, double cpuTime);

private:
    Ui::MyProperties *ui;
};

#endif // MYPROPERTIES_H
