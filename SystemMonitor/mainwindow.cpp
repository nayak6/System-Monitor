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
int graph_decider = 0;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->treeWidget->setColumnCount(6);
    ui->treeWidget->setHeaderLabels(QStringList() << "Name" <<"State" << "%CPU" << "ID" << "Memory Used" <<"PPID");
    ui->treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->treeWidget, &QTreeWidget::customContextMenuRequested, this, &MainWindow::on_treeWidget_customContextMenuRequested);

    ui->customPlot->setVisible(false);
    ui->listWidget->setVisible(true);
    ui->treeWidget->setVisible(false);
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

    ui->customPlot->setVisible(false);
    ui->listWidget->setVisible(true);
    ui->treeWidget->setVisible(false);

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

    ui->customPlot->setVisible(false);
    ui->listWidget->setVisible(false);
    ui->treeWidget->setVisible(true);

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

void MainWindow::cpu_graph()
{
    ui->customPlot->rescaleAxes();
    ui->customPlot->clearPlottables();
    ui->customPlot->replot();

    // set locale to english, so we get english month names:
    ui->customPlot->setLocale(QLocale(QLocale::English, QLocale::UnitedKingdom));
    // seconds of current time, we'll use it as starting point in time for data:
    double now = QDateTime::currentDateTime().toTime_t();
    srand(8); // set the random seed, so we always get the same random data
    // create multiple graphs:
    for (int gi=0; gi<5; ++gi)
    {
      ui->customPlot->addGraph();
      QColor color(20+200/4.0*gi,70*(1.6-gi/4.0), 150, 150);
      ui->customPlot->graph()->setPen(QPen(color));
      // generate random walk data:
      QVector<QCPGraphData> timeData(250);
      for (int i=0; i<250; ++i)
      {
        timeData[i].key = now + 24*3600*i;
        if (i == 0)
          timeData[i].value = (i/50.0+1)*(rand()/(double)RAND_MAX-0.5);
        else
          timeData[i].value = qFabs(timeData[i-1].value)*(1+0.02/4.0*(4-gi)) + (i/50.0+1)*(rand()/(double)RAND_MAX-0.5);
      }
      ui->customPlot->graph()->data()->set(timeData);
    }

    // configure bottom axis to show date instead of number:
    QSharedPointer<QCPAxisTickerText> ticker(new QCPAxisTickerText);
    ticker->addTick(0, "60");
    ticker->addTick(25, "45");
    ticker->addTick(50, "30");
    ticker->addTick(75, "15");
    ticker->addTick(100, "0");
    ui->customPlot->xAxis->setTicker(ticker);

    // configure left axis text labels:
    QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
    textTicker->addTick(0, "0%");
    textTicker->addTick(20, "20%");
    textTicker->addTick(40, "40%");
    textTicker->addTick(60, "60%");
    textTicker->addTick(80, "80%");
    textTicker->addTick(100, "100%");
    ui->customPlot->yAxis->setTicker(textTicker);

    // set a more compact font size for bottom and left axis tick labels:
    ui->customPlot->xAxis->setTickLabelFont(QFont(QFont().family(), 8));
    ui->customPlot->yAxis->setTickLabelFont(QFont(QFont().family(), 8));

    // set axis labels:
    ui->customPlot->xAxis->setLabel("TIME\n\nCPU HISTORY");
    ui->customPlot->yAxis->setLabel("CPU USAGE");

    // make top and right axes visible but without ticks and labels:
    ui->customPlot->xAxis2->setVisible(true);
    ui->customPlot->yAxis2->setVisible(true);
    ui->customPlot->xAxis2->setTicks(false);
    ui->customPlot->yAxis2->setTicks(false);
    ui->customPlot->xAxis2->setTickLabels(false);
    ui->customPlot->yAxis2->setTickLabels(false);
    // set axis ranges to show all data:
    ui->customPlot->xAxis->setRange(now, now+24*3600*249);
    ui->customPlot->yAxis->setRange(0, 100);
    // show legend with slightly transparent background brush:
    ui->customPlot->legend->setVisible(true);
    ui->customPlot->legend->setBrush(QColor(255, 255, 255, 150));
    ui->customPlot->replot();
}

void MainWindow::memswap_graph()
{
    ui->customPlot->rescaleAxes();
    ui->customPlot->clearPlottables();
    ui->customPlot->replot();

    // set locale to english, so we get english month names:
    ui->customPlot->setLocale(QLocale(QLocale::English, QLocale::UnitedKingdom));
    // seconds of current time, we'll use it as starting point in time for data:
    double now = QDateTime::currentDateTime().toTime_t();
    srand(8); // set the random seed, so we always get the same random data
    // create multiple graphs:
    for (int gi=0; gi<5; ++gi)
    {
      ui->customPlot->addGraph();
      QColor color(20+200/4.0*gi,70*(1.6-gi/4.0), 150, 150);
      ui->customPlot->graph()->setPen(QPen(color));
      // generate random walk data:
      QVector<QCPGraphData> timeData(250);
      for (int i=0; i<250; ++i)
      {
        timeData[i].key = now + 24*3600*i;
        if (i == 0)
          timeData[i].value = (i/50.0+1)*(rand()/(double)RAND_MAX-0.5);
        else
          timeData[i].value = qFabs(timeData[i-1].value)*(1+0.02/4.0*(4-gi)) + (i/50.0+1)*(rand()/(double)RAND_MAX-0.5);
      }
      ui->customPlot->graph()->data()->set(timeData);
    }

    // configure bottom axis to show date instead of number:
    QSharedPointer<QCPAxisTickerText> ticker(new QCPAxisTickerText);
    ticker->addTick(0, "60");
    ticker->addTick(25, "45");
    ticker->addTick(50, "30");
    ticker->addTick(75, "15");
    ticker->addTick(100, "0");
    ui->customPlot->xAxis->setTicker(ticker);

    // configure left axis text labels:
    QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
    textTicker->addTick(0, "0%");
    textTicker->addTick(20, "20%");
    textTicker->addTick(40, "40%");
    textTicker->addTick(60, "60%");
    textTicker->addTick(80, "80%");
    textTicker->addTick(100, "100%");
    ui->customPlot->yAxis->setTicker(textTicker);

    // set a more compact font size for bottom and left axis tick labels:
    ui->customPlot->xAxis->setTickLabelFont(QFont(QFont().family(), 8));
    ui->customPlot->yAxis->setTickLabelFont(QFont(QFont().family(), 8));

    // set axis labels:
    ui->customPlot->xAxis->setLabel("TIME\n\nMEMORY AND SWAP HISTORY");
    ui->customPlot->yAxis->setLabel("MEMORY AND SWAP % USAGE");

    // make top and right axes visible but without ticks and labels:
    ui->customPlot->xAxis2->setVisible(true);
    ui->customPlot->yAxis2->setVisible(true);
    ui->customPlot->xAxis2->setTicks(false);
    ui->customPlot->yAxis2->setTicks(false);
    ui->customPlot->xAxis2->setTickLabels(false);
    ui->customPlot->yAxis2->setTickLabels(false);

    // set axis ranges to show all data:
    ui->customPlot->xAxis->setRange(now, now+24*3600*249);
    ui->customPlot->yAxis->setRange(0, 100);

    // show legend with slightly transparent background brush:
    ui->customPlot->legend->setVisible(true);
    ui->customPlot->legend->setBrush(QColor(255, 255, 255, 150));
    ui->customPlot->replot();
}

void MainWindow::network_graph()
{
    ui->customPlot->rescaleAxes();
    ui->customPlot->clearPlottables();
    ui->customPlot->replot();

    // set locale to english, so we get english month names:
    ui->customPlot->setLocale(QLocale(QLocale::English, QLocale::UnitedKingdom));
    // seconds of current time, we'll use it as starting point in time for data:
    double now = QDateTime::currentDateTime().toTime_t();
    srand(8); // set the random seed, so we always get the same random data
    // create multiple graphs:
    for (int gi=0; gi<5; ++gi)
    {
      ui->customPlot->addGraph();
      QColor color(20+200/4.0*gi,70*(1.6-gi/4.0), 150, 150);
      ui->customPlot->graph()->setPen(QPen(color));
      // generate random walk data:
      QVector<QCPGraphData> timeData(250);
      for (int i=0; i<250; ++i)
      {
        timeData[i].key = now + 24*3600*i;
        if (i == 0)
          timeData[i].value = (i/50.0+1)*(rand()/(double)RAND_MAX-0.5);
        else
          timeData[i].value = qFabs(timeData[i-1].value)*(1+0.02/4.0*(4-gi)) + (i/50.0+1)*(rand()/(double)RAND_MAX-0.5);
      }
      ui->customPlot->graph()->data()->set(timeData);
    }

    // configure bottom axis to show date instead of number:
    QSharedPointer<QCPAxisTickerText> ticker(new QCPAxisTickerText);
    ticker->addTick(0, "60");
    ticker->addTick(25, "45");
    ticker->addTick(50, "30");
    ticker->addTick(75, "15");
    ticker->addTick(100, "0");
    ui->customPlot->xAxis->setTicker(ticker);

    // configure left axis text labels:
    QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
    textTicker->addTick(0, "0%");
    textTicker->addTick(20, "20%");
    textTicker->addTick(40, "40%");
    textTicker->addTick(60, "60%");
    textTicker->addTick(80, "80%");
    textTicker->addTick(100, "100%");
    ui->customPlot->yAxis->setTicker(textTicker);

    // set a more compact font size for bottom and left axis tick labels:
    ui->customPlot->xAxis->setTickLabelFont(QFont(QFont().family(), 8));
    ui->customPlot->yAxis->setTickLabelFont(QFont(QFont().family(), 8));

    // set axis labels:
    ui->customPlot->xAxis->setLabel("TIME\n\nNETWORK HISTORY");
    ui->customPlot->yAxis->setLabel("NETWORK % USAGE");

    // make top and right axes visible but without ticks and labels:
    ui->customPlot->xAxis2->setVisible(true);
    ui->customPlot->yAxis2->setVisible(true);
    ui->customPlot->xAxis2->setTicks(false);
    ui->customPlot->yAxis2->setTicks(false);
    ui->customPlot->xAxis2->setTickLabels(false);
    ui->customPlot->yAxis2->setTickLabels(false);

    // set axis ranges to show all data:
    ui->customPlot->xAxis->setRange(now, now+24*3600*249);
    ui->customPlot->yAxis->setRange(0, 100);

    // show legend with slightly transparent background brush:
    ui->customPlot->legend->setVisible(true);
    ui->customPlot->legend->setBrush(QColor(255, 255, 255, 150));
    ui->customPlot->replot();
}


void MainWindow::on_pushButton_3_clicked()
{
    ui->listWidget->clear();
    ui->treeWidget->clear();

    ui->customPlot->setVisible(true);
    ui->listWidget->setVisible(false);
    ui->treeWidget->setVisible(false);

    graph_decider++;
    if(graph_decider % 3 == 1) {
        cpu_graph();
    }
    else if(graph_decider % 3 == 2) {
        memswap_graph();
    }
    else {
        network_graph();
    }


}

void MainWindow::on_pushButton_4_clicked()
{
    ui->listWidget->clear();
    ui->treeWidget->clear();

    ui->customPlot->setVisible(false);
    ui->listWidget->setVisible(true);
    ui->treeWidget->setVisible(false);

    ui->listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    ui->listWidget->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

}
