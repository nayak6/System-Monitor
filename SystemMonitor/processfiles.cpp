#include "processfiles.h"
#include "ui_processfiles.h"

#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include<iostream>
#include<QDebug>
#include <QFile>
#include <sstream>
#include <string>
#include <QMessageBox>


ProcessFiles::ProcessFiles(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ProcessFiles)
{
    ui->setupUi(this);
}

ProcessFiles::~ProcessFiles()
{
    delete ui;
}

void ProcessFiles::listOpenFiles(QString processID){
    ui->treeWidget->setColumnCount(3);
    ui->treeWidget->setHeaderLabels(QStringList() << "FD" <<"Type" <<"Object");

    QByteArray ba = processID.toLocal8Bit();
    char* pPid = ba.data();
    char filePath[24] = "";
    sprintf(filePath, "/proc/%s/fd",pPid);
    if (chdir(filePath) != 0) {
        perror("chdir() to filePath failed");
//        QMessageBox messageBox;
//        messageBox.critical(0,"Error","Not a User Process : Not Accessible");
//        messageBox.setFixedSize(500,200);
//        return 1;
     }

    int pipe_fd[2];
      pid_t pid;


      if (pipe(pipe_fd)==-1) {
        perror("pipe");
       }

      if ((pid = fork()) == -1) {
        perror("fork");
       }

      if(pid == 0) {

        dup2 (pipe_fd[1], STDOUT_FILENO);
//        clo
        ::close(pipe_fd[0]);
        ::close(pipe_fd[1]);
        execlp("ls", "ls","-l", NULL);
        perror("execlp");

      }
      else {
          ::close(pipe_fd[1]);

          char str[512];
          FILE* fp = fdopen(pipe_fd[0], "r");
          while (fgets (str, 512, fp)!=NULL ) {
              QTreeWidgetItem *item = new QTreeWidgetItem();

//              qDebug() << str;

              if (!strstr(str, "->")) {
                  continue;
              }

              QString string(str);
              QStringList list = string.split(QRegExp("\\s+"), QString::SkipEmptyParts);
              item->setText(0,list.at(8));

              // Contains socket

              if (strstr(str,"socket:[")){
                  item->setText(1, "local socket");
              }
              else{
                  item->setText(1, "file");
                  item->setText(2, list.at(10));
              }

              ui->treeWidget->addTopLevelItem(item);
          }

          ::close(pipe_fd[0]);
          fclose(fp);

//        qDebug() << filePath;

        wait(NULL);

      }



}
