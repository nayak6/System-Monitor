#ifndef MEMMAPMAINWINDOW_H
#define MEMMAPMAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MemMapMainWindow;
}

class MemMapMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MemMapMainWindow(QWidget *parent = 0);
    ~MemMapMainWindow();
    void setPid(QString myPid);

private:
    Ui::MemMapMainWindow *ui;
};

#endif // MEMMAPMAINWINDOW_H
