#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "memmapmainwindow.h"
#include <QMainWindow>
#include <QTreeWidget>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    int AddParent (QString name, QString status, QString cpu, QString id, QString memory, QString ppid, int k);

    void AddChild (QTreeWidgetItem *parent, QString name, QString status, QString cpu, QString id, QString memory);

    void on_treeWidget_customContextMenuRequested(const QPoint &pos);

    void killItem();

    void stopItem();

    void continueItem();

    void openMemMap();

private:
    Ui::MainWindow *ui;
    MemMapMainWindow *mapp;
};

#endif // MAINWINDOW_H
