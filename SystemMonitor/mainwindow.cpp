#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <sys/utsname.h>
#include <sys/sysinfo.h>
#include <sys/statvfs.h>

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
