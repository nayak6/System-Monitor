#include "myproperties.h"
#include "ui_myproperties.h"
#include <pwd.h>
#include<QDebug>
#include<iostream>


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

void MyProperties::showProperties(QString processID, QString processName) {
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

        QString uName = "User Name          nayak6";
        ui->listWidget->addItem(uName);

    }





}
