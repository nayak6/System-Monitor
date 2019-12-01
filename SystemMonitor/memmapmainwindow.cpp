#include "memmapmainwindow.h"
#include "ui_memmapmainwindow.h"
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QDebug>

QString globalPid;
MemMapMainWindow::MemMapMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MemMapMainWindow)
{
    ui->setupUi(this);
    ui->treeWidget->setColumnCount(10);
    ui->treeWidget->setHeaderLabels(QStringList() << "Filename" <<"VMStart" << "VMEnd" << "VMSize" << "Flags" << "VMOffset" << "Private Clean" << "Private Dirty" << "Shared Clean" << "Shared Dirty");
}

MemMapMainWindow::~MemMapMainWindow()
{
    delete ui;
}

void MemMapMainWindow::setPid(QString myPid)
{
    globalPid = myPid;
    QString filename = "/proc/";
    filename += globalPid;
    filename += ("/smaps");
    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly))
        QMessageBox::information(0, "info", file.errorString());
    QTextStream in(&file);
    QString line = in.readLine();
    int count = 0;
    while(!line.isNull()) {
        QString filename;
        QString vmstart;
        QString vmend;
        QString vmsize;
        QString flags;
        QString vmoffset;
        QString pclean;
        QString pdirty;
        QString sclean;
        QString sdirty;

        if (line.contains("-",  Qt::CaseInsensitive)) {
            printf("%s our linne\n", line);
            QStringList list = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);
            flags = list.at(1);
            vmoffset = list.at(2);
            filename = list.at(5);

            QStringList list2 = list.at(0).split(QRegExp("-"), QString::SkipEmptyParts);
            vmstart = list2.at(0);
            vmend = list2.at(1);
            count++;
        }
        else if (line.contains("Shared_Clean",  Qt::CaseInsensitive)) {
            QStringList list = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);
            sclean = list.at(1);
            count++;
        }
        else if (line.contains("Shared_Dirty",  Qt::CaseInsensitive)) {
            QStringList list = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);
            sdirty = list.at(1);
            count++;
        }
        else if (line.contains("Private_Clean",  Qt::CaseInsensitive)) {
            QStringList list = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);
            pclean = list.at(1);
            count++;
        }
        else if (line.contains("Private_Dirty",  Qt::CaseInsensitive)) {
            QStringList list = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);
            pdirty = list.at(1);
            count++;
        }

        if (count == 5) {
            QTreeWidgetItem *item = new QTreeWidgetItem();
            item->setText(0, filename);
            item->setText(1, vmstart);
            item->setText(2, vmend);
            item->setText(3, vmsize);
            item->setText(4, flags);
            item->setText(5, vmoffset);
            item->setText(6, pclean);
            item->setText(7, pdirty);
            item->setText(8, sclean);
            item->setText(9, sdirty);
            ui->treeWidget->addTopLevelItem(item);
            count = 0;
        }

        line = in.readLine();
    }

}
