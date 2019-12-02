#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "memmapmainwindow.h"
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
#include <signal.h>
#include <sys/statfs.h>

struct dirent **listdir;
QString selectedPid;
QString selectedProcess;
QString selectedState;
QString selectedCPU;
QString selectedMemory;
double cpuTime;

int selectedUserProcesses = 1;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    ui->listWidget->clear();
    ui->treeWidget->clear();
    ui->treeWidget->setColumnCount(0);
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
    QString userTime = list2.at(1);
    QString niceTime = list2.at(2);
    QString systemTime = list2.at(3);

    bool ok = false;
    cpuTime = uTime.toDouble() + sTime.toDouble();
    double cpu_time = 100 * 100 * (uTime.toDouble(&ok) + sTime.toDouble(&ok)) / (userTime.toDouble(&ok) + niceTime.toDouble(&ok) + systemTime.toDouble(&ok));
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
    item->setText(2, QString::number( cpu.toDouble(), 'f', 6));
    item->setText(3, id);
    item->setText(4, memory + " MiB");
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

void MainWindow::killItem()
{
    //printf("I am here!");
    QList<QTreeWidgetItem*> sel_items = ui->treeWidget->selectedItems();
    for(int i=0; i<sel_items.size(); i++){
        kill(selectedPid.toInt(), SIGKILL);
        delete sel_items.at(i);
    }
}

void MainWindow::stopItem()
{
    //printf("I am here!");
    QList<QTreeWidgetItem*> sel_items = ui->treeWidget->selectedItems();
    for(int i=0; i<sel_items.size(); i++){
        kill(selectedPid.toInt(), SIGSTOP);
        //delete sel_items.at(i);
    }
}

void MainWindow::continueItem()
{
    //printf("I am here!");
    QList<QTreeWidgetItem*> sel_items = ui->treeWidget->selectedItems();
    for(int i=0; i<sel_items.size(); i++){
        kill(selectedPid.toInt(), SIGCONT);
        //delete sel_items.at(i);
    }
}

bool isUserProcess(char *thePid) {
    uid_t user;
    struct stat dirinfo;
    int len = strlen(thePid) + 7;
    char path[len];

    strcpy(path, "/proc/");
    strcat(path, thePid);
    user = getuid();
    if (stat(path, &dirinfo) < 0) {
        perror("processdir() ==> stat()");
        exit(EXIT_FAILURE);
    }

    if (user == dirinfo.st_uid) {
        return true;
    }
    else {
        return false;
    }
}

void MainWindow::listProperties() {
    propertiesNewWindow = new MyProperties();
    propertiesNewWindow->showProperties(selectedPid, selectedProcess,selectedCPU,selectedMemory,selectedState, cpuTime);
    propertiesNewWindow->show();
//    this->hide(); //this will disappear main window
}

void MainWindow::openMemMap()
{
    QByteArray ba = selectedPid.toLocal8Bit();
    char *fs = ba.data();
    if (isUserProcess(fs)) {
        mapp = new MemMapMainWindow(this);
        mapp->setPid(selectedPid);
        mapp->show();
    }
    else {
        QMessageBox mBox;
        mBox.critical(0, "ERROR", "Not an user process!");
        mBox.setFixedSize(500,200);
    }
}

void MainWindow::openProcessFiles(){
    QByteArray ba = selectedPid.toLocal8Bit();
    char *fs = ba.data();
    if (isUserProcess(fs)) {
        processFilesDialog = new ProcessFiles();
        processFilesDialog->listOpenFiles(selectedPid);
        processFilesDialog->show();
    }
    else {
        QMessageBox mBox;
        mBox.critical(0, "ERROR", "Not an user process!");
        mBox.setFixedSize(500,200);
    }

}

//Menu box on right clicking a process
void MainWindow::on_treeWidget_customContextMenuRequested(const QPoint &pos)
{
    QTreeWidget *tree = ui->treeWidget;
    QTreeWidgetItem *item = tree->itemAt(pos);
    selectedPid = item->text(3);
    selectedProcess = item->text(0);
    selectedState = item->text(1);
    selectedCPU = item->text(2);
    selectedMemory = item->text(4);

    QMenu menu(this);

    //NULL in place of SLOT(newDev())
    QAction *action1 = new QAction(QIcon(":/Resource/warning32.ico"),tr("&Stop Process"), this);
    connect(action1, SIGNAL(triggered()), this, SLOT(stopItem()));
    menu.addAction(action1);

    QAction *action2 = new QAction(QIcon(":/Resource/warning32.ico"),tr("&Continue Process"), this);
    connect(action2, SIGNAL(triggered()), this, SLOT(continueItem()));
    menu.addAction(action2);

    QAction *action3 = new QAction(QIcon(":/Resource/warning32.ico"),tr("&Kill Process"), this);
    connect(action3, SIGNAL(triggered()), this, SLOT(killItem()));
    menu.addAction(action3);

    QAction *action4 = new QAction(QIcon(":/Resource/warning32.ico"),tr("&List Memory Maps"), this);
    connect(action4, SIGNAL(triggered()), this, SLOT(openMemMap()));
    menu.addAction(action4);

    QAction *action5 = new QAction(QIcon(":/Resource/warning32.ico"),tr("&List Open Files"), this);
    connect(action5, SIGNAL(triggered()), this, SLOT(openProcessFiles()));
    menu.addAction(action5);

    QAction *action6 = new QAction(QIcon(":/Resource/warning32.ico"),tr("&Properties"), this);
    connect(action6, SIGNAL(triggered()), this, SLOT(listProperties()));
    menu.addAction(action6);

    QPoint pt(pos);
    menu.exec(tree->mapToGlobal(pos));
}

void MainWindow::on_pushButton_2_clicked()
{
    ui->treeWidget->clear();
    ui->listWidget->clear();

    ui->treeWidget->setColumnCount(6);
    ui->treeWidget->setHeaderLabels(QStringList() << "Name" <<"State" << "%CPU" << "ID" << "Memory Used" <<"PPID");
    ui->treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->treeWidget, &QTreeWidget::customContextMenuRequested, this, &MainWindow::on_treeWidget_customContextMenuRequested);

    if (selectedUserProcesses == 0) {
        allProcess();
    }
    else {
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
//                int counter =
                MainWindow::AddParent(name, status, cputime, id, memory, ppid, k);
                //then use another loop to free all those namelist entries
    //            for(int i = 0; i < counter; i++) {
    //                free(namelist[n - i]);
    //            }

            }

            //free(namelist);

        }
    }
}


void MainWindow::on_pushButton_3_clicked() {
    ui->listWidget->clear();
    ui->treeWidget->clear();
    ui->treeWidget->setColumnCount(0);
    ui->treeWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    ui->listWidget->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
}

void MainWindow::on_pushButton_4_clicked() {
    ui->listWidget->clear();
    ui->treeWidget->setColumnCount(0);
    ui->treeWidget->clear();
    ui->treeWidget->setColumnCount(7);
    ui->treeWidget->setHeaderLabels(QStringList() << "Device" <<"Directory" << "Type" << "Total" << "Free" <<"Available" << "Used");

    QString filename = "/proc";
    filename += ("/mounts");
    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly))
        QMessageBox::information(0, "info", file.errorString());
    QTextStream in(&file);
    QString line = in.readLine();

    double freeMem;
    double totalMem;
    double usedMem;

    while(!line.isNull()) {

        QTreeWidgetItem *item = new QTreeWidgetItem();
        QStringList list = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);

        QString directory = list.at(1);
        QByteArray ba = directory.toLocal8Bit();
        char* fs = ba.data();
        struct statvfs buf;

          if (statvfs(fs, &buf) == -1) {
            line = in.readLine();
            continue;
           }
          else {
                  freeMem = (double)buf.f_bavail * buf.f_bsize;
                  totalMem = (double)buf.f_blocks * buf.f_bsize;

                  if (freeMem == 0 || totalMem == 0){
                      line = in.readLine();
                      continue;
                  }

                  usedMem = totalMem - freeMem;

          }

        // Device
        item->setText(0, list.at(0));

        //Directory

        item->setText(1, list.at(1));

        // Type

        item->setText(2, list.at(2));

        //Total

        item->setText(3, QString::number(totalMem/(1024*1024)) + " MiB");

        item->setText(4, QString::number(freeMem/(1024*1024)) + " MiB");

        item->setText(5, QString::number((freeMem/(1024*1024))) + " MiB");

        item->setText(6, QString::number(usedMem/(1024*1024)) + " MiB");

        // Add Item

        ui->treeWidget->addTopLevelItem(item);


        line = in.readLine();
    }
}

void MainWindow::on_actionAll_Processes_triggered()
{
    selectedUserProcesses = 0;
    allProcess();
}

int filter2(const struct dirent *dir)
{
    return !fnmatch("[1-9]*", dir->d_name, 0);
}

void MainWindow::allProcess() {
    ui->treeWidget->clear();
    ui->listWidget->clear();
    struct dirent **namelist;
    int n;
    n = scandir("/proc", &namelist, filter2, 0);
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
            if (memSize == "") {
                MainWindow::AddParent(name, status, cputime, id, "0", ppid, k);
            }
            else {
                QStringList list = memSize.split(QRegExp("\\s+"), QString::SkipEmptyParts);
                QString memory = ( QString::number(list.at(1).toDouble(&ok) / 1024));

//                int counter =
                MainWindow::AddParent(name, status, cputime, id, memory, ppid, k);
            }
        }
    }
}

void MainWindow::on_actionUser_Processes_triggered()
{
    selectedUserProcesses = 1;
    on_pushButton_2_clicked();
}
