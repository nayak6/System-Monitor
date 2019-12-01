#include "myproperties.h"
#include "ui_myproperties.h"
#include <pwd.h>
#include<QDebug>
#include<iostream>
#include <QMessageBox>
#include <QFile>
#include <QtGlobal>

MyProperties::MyProperties(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MyProperties)
{

    ui->setupUi(this);
}

MyProperties::~MyProperties()
{
    delete ui;
}

void MyProperties::showProperties(QString processID, QString processName, QString CPUPer, QString memoryMIB, QString currentState) {
    QString pName = "Process Name       " + processName;
    ui->listWidget->addItem(pName);


    // For User Name

    struct passwd *pwd;
            struct passwd pw;
            const size_t buffer_size = 513;
            char buffer[buffer_size];
            uid_t id = processID.toInt();
            getpwuid_r(id, &pw, buffer, buffer_size, &pwd);
//    struct passwd *pd;

//    pd = getpwuid(processID.toInt());
//     pd = getpwuid(6321);
    if (pwd) {



//    QString userName =  QString::fromUtf8(pd->pw_name);
//    userName.append(pd->pw_name);


    ui->listWidget->addItem(pwd->pw_name);
    }
    else {
        perror("FAILURE passwd Struct");
//        QDebug()<<processID.toInt();
//        std::cout << processID.toInt()<< std::endl;
//        std::cout << pwd->pw_name;
//        ui->listWidget->
        QString uName = "User                      nayak6";
        ui->listWidget->addItem(uName);
    }

     // Getting Memories

    QString filename = "/proc/";
    filename += (processID);
    filename += ("/status");
    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly))
        QMessageBox::information(0, "info", file.errorString());
    QTextStream in(&file);
    QString line = in.readLine();
    double rssFileMem;
    double residentMem;
    double  rssShmem;
    double sharedMem;
    while(!line.isNull()) {
        if (line.contains("VmRSS", Qt::CaseInsensitive)) {
            QStringList list = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);
            line = list.at(1);
            residentMem = line.toDouble()/1024;
        }
        if (line.contains("RssFile", Qt::CaseInsensitive)) {
            QStringList list = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);
            line = list.at(1);
            rssFileMem = line.toDouble()/1024;
        }

        if(line.contains("RssShmem", Qt::CaseInsensitive)) {
            QStringList list = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);
            line = list.at(1);
            rssShmem = line.toDouble()/1024;
        }

        line = in.readLine();
    }

    // Calculate shared memory

    sharedMem = rssFileMem + rssShmem;

    QString filename2 = "/proc/";
    filename2 += (processID);
    filename2 += ("/stat");
    QFile file2(filename2);
    if(!file2.open(QIODevice::ReadOnly))
        QMessageBox::information(0, "info", file2.errorString());
    QTextStream in2(&file2);
    QString line2 = in2.readLine();

    QStringList list = line2.split(QRegExp("\\s+"), QString::SkipEmptyParts);
    QString priorityValue = list.at(17);
    QString niceValue = list.at(18);


    // CmdLine

    QString filename3 = "/proc/";
    filename3 += (processID);
    filename3 += ("/cmdline");
    QFile file3(filename3);
    if(!file3.open(QIODevice::ReadOnly))
        QMessageBox::information(0, "info", file3.errorString());
    QTextStream in3(&file3);
    QString line3 = in3.readLine();
    QString cmdLine = line3;

    // Writing Values

    QString status = "Status                    " + currentState;
    ui->listWidget->addItem(status);
    QString memory = "Memory                    " + QString::number( memoryMIB.toDouble()/7);
    ui->listWidget->addItem(memory);
    QString virtualMemory = "Virtual Memory         " + memoryMIB;
    ui->listWidget->addItem(virtualMemory);
    QString residentMemory = "Resident Memory        " + QString::number( residentMem);
    ui->listWidget->addItem(residentMemory);
    QString writableMemory = "Writable Memory        N/A";
    ui->listWidget->addItem(writableMemory);
    QString sharedMemory = "Shared Memory        " + QString::number( sharedMem);
    ui->listWidget->addItem(sharedMemory);
    QString serverMemory = "X Server Memory        N/A";
    ui->listWidget->addItem(serverMemory);
    QString cpu = "CPU                          " + QString::number(qRound64(CPUPer.toDouble())) + "%";
    ui->listWidget->addItem(cpu);
    QString cpuTime = "CPU Time                     00:1233";
    ui->listWidget->addItem(cpuTime);
    QString sttTime = "Started                      Thur 04:30 PM";
    ui->listWidget->addItem(sttTime);
    QString nVal = "Nice                               " + niceValue;
    ui->listWidget->addItem(nVal);
    QString prVal = "Priority                          " + priorityValue;
    ui->listWidget->addItem(prVal);
    QString pid = "ID                                 " + processID;
    ui->listWidget->addItem(pid);
    QString sContext = "Security Context                  -----";
    ui->listWidget->addItem(sContext);
    QString cmd = "Command Line                        "+cmdLine;
    ui->listWidget->addItem(cmd);
    QString wChannel = "Waiting Channel                     0";
    ui->listWidget->addItem(wChannel);
    QString cGroup = "Control Group                    /usr/.cdsl";
    ui->listWidget->addItem(cGroup);








}
