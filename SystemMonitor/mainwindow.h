#ifndef MAINWINDOW_H
#define MAINWINDOW_H

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

    void cpu_graph();

    void memswap_graph();

    void network_graph();

    void realtimeDataSlot();

    void update(int graph_choice);

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
