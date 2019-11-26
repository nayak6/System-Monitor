#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
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
    double davkar = (uTime.toDouble(&ok) + sTime.toDouble(&ok)) / (userTime.toDouble(&ok) + niceTime.toDouble(&ok) + systemTime.toDouble(&ok));
    return davkar;
}

void MainWindow::on_pushButton_2_clicked()
{
    ui->listWidget->clear();
    struct dirent **namelist;
    int n;
    n = scandir("/proc", &namelist, filter, 0);
    if (n < 0)
        perror("Not enough memory.");
    else {

        while(n--){

            QString filename = "/proc/";
            filename += (namelist[n]->d_name);
            filename += ("/status");
            QFile file(filename);
            if(!file.open(QIODevice::ReadOnly))
                QMessageBox::information(0, "info", file.errorString());
            QTextStream in(&file);
            QString line = in.readLine();
            QString name;
            QString status;
            QString memSize;
            while(!line.isNull()) {
                if (line.contains("Name", Qt::CaseInsensitive)) {
                    name = line;
                }
                if (line.contains("State", Qt::CaseInsensitive)) {
                    status = line;
                }
                if (line.contains("VmSize", Qt::CaseInsensitive)) {
                    memSize = line;
                }
                line = in.readLine();
            }
            QString finalLine = name;
            finalLine += "            ";
            finalLine += status;
            double amrish = calculateCpuTime(namelist[n]->d_name);
            finalLine += "   CPU Time";
            finalLine += ( QString::number(amrish));
            finalLine += "   PID: ";
            finalLine += namelist[n]->d_name;
            finalLine += "   Memory Used: ";
            bool ok = false;
            //double mmm = memSize.toDouble(&ok) / 1024;
            //finalLine += ( QString::number(mmm));
            QStringList list = memSize.split(QRegExp("\\s+"), QString::SkipEmptyParts);
            finalLine += ( QString::number(list.at(1).toDouble(&ok) / 1024));
            //finalLine += memSize;
            ui->listWidget->addItem(finalLine);
            free(namelist[n]);

        }

        free(namelist);

    }
}
