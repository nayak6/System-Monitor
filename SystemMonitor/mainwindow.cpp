#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QDebug>
#include <sys/utsname.h>
#include <sys/sysinfo.h>
#include <sys/statvfs.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <fnmatch.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

struct dirent **listdir;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->treeWidget->setColumnCount(6);
    ui->treeWidget->setHeaderLabels(QStringList() << "Name" <<"State" << "%CPU" << "ID" << "Memory Used" <<"PPID");
    ui->treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->treeWidget, &QTreeWidget::customContextMenuRequested, this, &MainWindow::on_treeWidget_customContextMenuRequested);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    ui->listWidget->clear();
    ui->treeWidget->clear();
    ui->listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    ui->listWidget->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    QFile file("/proc/cpuinfo");
    if(!file.open(QIODevice::ReadOnly))
        QMessageBox::information(0, "info", file.errorString());
    QTextStream in(&file);
    QString line = in.readLine();
    while(!line.isNull()) {
        line = in.readLine();
        if (line.contains("model name", Qt::CaseInsensitive)) {
            QString procc = "Processor:";
            QStringList pieces = line.split(":");
            QString wordd = pieces.value(pieces.length() - 1);
            procc.append(wordd);
            ui->listWidget->addItem(procc);
            break;
        }
    }
    struct utsname unameData;
    uname(&unameData);
    QString releaseName = "Kernel Release: ";
    releaseName.append(unameData.release);
    QString versionName = "Kernel Version: ";
    versionName.append(unameData.version);

    struct sysinfo sys_info;
    sysinfo(&sys_info);
    QString availableMem = "Available Mem: ";
    availableMem.append(QString::number(((sys_info.totalram * (unsigned long long) sys_info.mem_unit) / (1024 * 1024 * 1024))));

    struct statvfs stat;
    if (statvfs("/dev/null", &stat) != 0) {
        printf("fdjngvflkd");
    }
    else {
        ui->listWidget->addItem(releaseName);
        ui->listWidget->addItem(versionName);
        ui->listWidget->addItem(availableMem);
        QString diskStorage = "Disk Storage: ";
        diskStorage.append(QString::number((stat.f_blocks * stat.f_frsize) / (1024 * 1024 * 1024)));
        ui->listWidget->addItem(diskStorage);
    }


}

int filter(const struct dirent *dir)
{
    uid_t user;
    struct stat dirinfo;
    int len = strlen(dir->d_name) + 7;
    char path[len];

    strcpy(path, "/proc/");
    strcat(path, dir->d_name);
    user = getuid();
    if (stat(path, &dirinfo) < 0) {
        perror("processdir() ==> stat()");
        exit(EXIT_FAILURE);
    }
    return !fnmatch("[1-9]*", dir->d_name, 0) && user == dirinfo.st_uid;
}

double calculateCpuTime(QString pidd) {
    QString filename = "/proc/";
    filename += pidd;
    filename += ("/stat");
    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly))
        QMessageBox::information(0, "info", file.errorString());
    QTextStream in(&file);
    QString line = in.readLine();
    QStringList list = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);
    QString uTime = list.at(13);
    QString sTime = list.at(14);

    QFile file2("/proc/stat");
    if(!file2.open(QIODevice::ReadOnly))
        QMessageBox::information(0, "info", file2.errorString());
    QTextStream in2(&file2);
    QString line2 = in2.readLine();
    QStringList list2 = line2.split(QRegExp("\\s+"), QString::SkipEmptyParts);
    QString userTime = list2.at(0);
    QString niceTime = list2.at(1);
    QString systemTime = list2.at(2);

    bool ok = false;
    double cpu_time = (uTime.toDouble(&ok) + sTime.toDouble(&ok)) / (userTime.toDouble(&ok) + niceTime.toDouble(&ok) + systemTime.toDouble(&ok));
    return cpu_time;
}

//my implementation

//parent processes in a treewidget
int MainWindow::AddParent(QString name, QString status, QString cpu, QString id, QString memory, QString ppid, int k)
{
    int count = 0;
    QTreeWidgetItem *item = new QTreeWidgetItem();
    item->setText(0, name);
    item->setText(1, status);
    item->setText(2, cpu);
    item->setText(3, id);
    item->setText(4, memory);
    item->setText(5, ppid);
    ui->treeWidget->addTopLevelItem(item);
    //make a loop here and implement a counter for all the processes
    //while subprocesses are there, addChild and increment count by 1.
    //MainWindow::AddChild(item, name, status, cpu, id, memory);
    //return the counter.

//    int counter = k;
//    while(counter--) {
//        if(listname[counter]->d_name)
//    }
    return count;
}

//Child processes in a treewidget
void MainWindow::AddChild (QTreeWidgetItem *parent, QString name, QString status, QString cpu, QString id, QString memory)
{
    QTreeWidgetItem *item = new QTreeWidgetItem();
    item->setText(0, name);
    item->setText(1, status);
    item->setText(2, cpu);
    item->setText(3, id);
    item->setText(4, memory);
    parent->addChild(item);
}

//Menu box on right clicking a process
void MainWindow::on_treeWidget_customContextMenuRequested(const QPoint &pos)
{
    QTreeWidget *tree = ui->treeWidget;
    QTreeWidgetItem *item = tree->itemAt(pos);
    qDebug() << pos << item->text(0);
    QMenu menu(this);

    //NULL in place of SLOT(newDev())
    QAction *action1 = new QAction(QIcon(":/Resource/warning32.ico"),tr("&Stop Process"), this);
    action1->setStatusTip(tr("new sth"));
    connect(action1, SIGNAL(triggered()), this, SLOT(newDev()));
    menu.addAction(action1);

    QAction *action2 = new QAction(QIcon(":/Resource/warning32.ico"),tr("&Continue Process"), this);
    action2->setStatusTip(tr("new sth"));
    connect(action2, SIGNAL(triggered()), this, SLOT(newDev()));
    menu.addAction(action2);

    QAction *action3 = new QAction(QIcon(":/Resource/warning32.ico"),tr("&Kill Process"), this);
    action3->setStatusTip(tr("new sth"));
    connect(action3, SIGNAL(triggered()), this, SLOT(newDev()));
    menu.addAction(action3);

    QAction *action4 = new QAction(QIcon(":/Resource/warning32.ico"),tr("&List Memory Maps"), this);
    action4->setStatusTip(tr("new sth"));
    connect(action4, SIGNAL(triggered()), this, SLOT(newDev()));
    menu.addAction(action4);

    QAction *action5 = new QAction(QIcon(":/Resource/warning32.ico"),tr("&List Open Files"), this);
    action5->setStatusTip(tr("new sth"));
    connect(action5, SIGNAL(triggered()), this, SLOT(newDev()));
    menu.addAction(action5);

    QPoint pt(pos);
    menu.exec(tree->mapToGlobal(pos));
}

void MainWindow::on_pushButton_2_clicked()
{
    ui->treeWidget->clear();
    ui->listWidget->clear();
    struct dirent **namelist;
    int n;
    n = scandir("/proc", &namelist, filter, 0);
    if (n < 0)
        perror("Not enough memory.");
    else {
        int k = n;
        while(n--){

            QString filename = "/proc/";
            filename += (namelist[n]->d_name);
            filename += ("/status");
            QFile file(filename);
            if(!file.open(QIODevice::ReadOnly))
                QMessageBox::information(0, "info", file.errorString());
            QTextStream in(&file);
            QString line = in.readLine();
            QString ppid;
            QString name;
            QString status;
            QString memSize;
            while(!line.isNull()) {
                if (line.contains("Name", Qt::CaseInsensitive)) {
                    QStringList list = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);
                    line = list.at(1);
                    name = line;
                }
                if (line.contains("State", Qt::CaseInsensitive)) {
                    QStringList list = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);
                    line = list.at(2);
                    status = line;
                }
                if (line.contains("VmSize", Qt::CaseInsensitive)) {
                    memSize = line;
                }

                if(line.contains("PPid", Qt::CaseInsensitive)) {
                    QStringList list = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);
                    line = list.at(1);
                    ppid = line;
                }
                line = in.readLine();
            }
            double cpu = calculateCpuTime(namelist[n]->d_name);
            QString cputime = QString::number(cpu);
            QString id = namelist[n]->d_name;
            bool ok = false;
            QStringList list = memSize.split(QRegExp("\\s+"), QString::SkipEmptyParts);
            QString memory = ( QString::number(list.at(1).toDouble(&ok) / 1024));

            //returns i number of subprocesses that we can subtract from the n count loop
            int counter = MainWindow::AddParent(name, status, cputime, id, memory, ppid, k);
            //then use another loop to free all those namelist entries
//            for(int i = 0; i < counter; i++) {
//                free(namelist[n - i]);
//            }

        }

        //free(namelist);

    }
}


void MainWindow::on_pushButton_3_clicked()
{
    ui->treeWidget->clear();
    ui->treeWidget->clear();
    ui->treeWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    ui->listWidget->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
}

void MainWindow::on_pushButton_4_clicked()
{
    ui->listWidget->clear();
    ui->treeWidget->clear();
    ui->listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    ui->listWidget->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

}
