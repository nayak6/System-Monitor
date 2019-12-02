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
QTimer *dataTimer = new QTimer();

int graph_decider = 0;
int graph_choice = 1;
double *cpu_usage;
double swap_usage;
double memory_usage;
double cpuArray[8];

double previousReceived = 0;
double previousSent = 0;

double diffReceived = 1000;
double diffSent = 1000;

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
        }
    }
}

void MainWindow::update(int graph_choice) {
    //cpu usage
    if(graph_choice == 1) {
        QString filename = "/proc/stat";
        QFile file(filename);
        if(!file.open(QIODevice::ReadOnly))
            QMessageBox::information(0, "info", file.errorString());
        QTextStream in(&file);
        QString line = in.readLine();
        int count = 0;
        double totalCpu = 0;
        bool ok = false;
        while(!line.isNull()) {
            if (line.contains("cpu")) {
                if (count == 0) {
                    QStringList list = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);
                    for (int i = 1; i <= 9; i++) {
                        totalCpu += list.at(i).toDouble(&ok);
                    }
                }
                else {
                    QStringList list = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);
                    for (int i = 1; i <= 9; i++) {
                        cpuArray[count - 1] += list.at(i).toDouble(&ok);
                    }
                    cpuArray[count - 1] = (cpuArray[count - 1] / totalCpu) * 100;
                }
                count++;
            }
            line = in.readLine();
        }
    }

    //mem and swap
    else if(graph_choice == 2) {
        QString filename = "/proc/meminfo";
        QFile file(filename);
        if(!file.open(QIODevice::ReadOnly))
            QMessageBox::information(0, "info", file.errorString());
        QTextStream in(&file);
        QString line = in.readLine();
        QString memory_available;
        QString memory_total;
        QString swap_memory_cache;
        QString swap_memory_total;
        while(!line.isNull()) {
            if (line.contains("MemAvailable", Qt::CaseInsensitive)) {
                QStringList list = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);
                memory_available = list.at(1);

            }
            if (line.contains("MemTotal", Qt::CaseInsensitive)) {
                QStringList list = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);
                memory_total = list.at(1);
            }
            if (line.contains("SwapCached", Qt::CaseInsensitive)) {
                QStringList list = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);
                swap_memory_cache = list.at(1);
            }
            if (line.contains("SwapTotal", Qt::CaseInsensitive)) {
                QStringList list = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);
                swap_memory_total = list.at(1);
            }
            line = in.readLine();
        }

        bool ok = false;
        memory_usage = 100 * ((memory_available.toDouble(&ok)) / (memory_total.toDouble(&ok)));
        swap_usage = 100 * ((swap_memory_cache.toDouble(&ok)) / (swap_memory_total.toDouble(&ok)));
    }
    else if (graph_choice == 3) {
        QString filename = "/proc/net/dev";
        QFile file(filename);
        if(!file.open(QIODevice::ReadOnly))
            QMessageBox::information(0, "info", file.errorString());
        QTextStream in(&file);
        QString line = in.readLine();
        line = in.readLine();
        line = in.readLine();

        bool ok = false;
        double received = 0;
        double sent = 0;

        while(!line.isNull()) {
           QStringList list = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);
           received += list.at(1).toDouble(&ok);
           sent += list.at(9).toDouble(&ok);
           line = in.readLine();
        }

        diffReceived = received - previousReceived;
        diffSent = sent - previousSent;

        previousReceived = received;
        previousSent = sent;
    }
}

void MainWindow::realtimeDataSlot()
{
    static QTime time(QTime::currentTime());
    // calculate two new data points:
    double key = time.elapsed()/1000.0; // time elapsed since start of demo, in seconds
    static double lastPointKey = 0;
    if (key-lastPointKey > 1) // at most add point every 2 ms
    {
        if (graph_choice == 1) {
            update(1);
            for(int i =0; i < 8; i++) {
                ui->customPlot->graph(i)->addData(key, cpuArray[i]);
                //ui->customPlot->graph(i)->addData(key,cpu_usage[i]);
            }
        }

        else if (graph_choice == 2) {
            //mem and swap
            update(2);
            qDebug("%f %f",memory_usage, swap_usage);
            ui->customPlot->graph(0)->addData(key, memory_usage);
            ui->customPlot->graph(1)->addData(key, swap_usage);
        }

        else {
            //network
            update(3);
            qDebug("Rec : %f Sent : %f",diffReceived/1024, diffSent/1024);
            ui->customPlot->graph(0)->addData(key, diffReceived / 1024);
            ui->customPlot->graph(1)->addData(key, diffSent / 1024);
        }
      lastPointKey = key;
    }
    // make key axis range scroll with the data (at a constant range size of 8):
    ui->customPlot->xAxis->setRange(key, 8, Qt::AlignRight);
    ui->customPlot->replot();

    // calculate frames per second:
    static double lastFpsKey;
    static int frameCount;
    ++frameCount;
    if (key-lastFpsKey > 2) // average fps over 2 seconds
    {
      ui->statusBar->showMessage(
            QString("%1 FPS, Total Data points: %2")
            .arg(frameCount/(key-lastFpsKey), 0, 'f', 0)
            .arg(ui->customPlot->graph(0)->data()->size()+ui->customPlot->graph(1)->data()->size())
            , 0);
      lastFpsKey = key;
      frameCount = 0;
    }
}

void MainWindow::cpu_graph()
{
    ui->customPlot->rescaleAxes();
    ui->customPlot->clearPlottables();
    ui->customPlot->replot();

    graph_choice = 1;
    dataTimer->stop();

    QString filename = "/proc/stat";
    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly))
        QMessageBox::information(0, "info", file.errorString());
    QTextStream in(&file);
    QString line = in.readLine();
    int count = 0;
    double totalCpu = 0;
    bool ok = false;
    while(!line.isNull()) {
        if (line.contains("cpu")) {
            if (count == 0) {
                QStringList list = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);
                for (int i = 1; i <= 9; i++) {
                    totalCpu += list.at(i).toDouble(&ok);
                }
            }
            else {
                QStringList list = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);
                for (int i = 1; i <= 9; i++) {
                    cpuArray[count - 1] += list.at(i).toDouble(&ok);
                }
                cpuArray[count - 1] = (cpuArray[count - 1] / totalCpu) * 100;
                ui->customPlot->addGraph();
                ui->customPlot->graph(count-1)->setPen(QPen(QColor(count - 1, 2 * (count - 1), 2 * (count - 1) )));
            }
            count++;
        }
        line = in.readLine();
    }


    QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
    timeTicker->setTimeFormat("%h:%m:%s");
    ui->customPlot->xAxis->setTicker(timeTicker);

    ui->customPlot->axisRect()->setupFullAxesBox();
    ui->customPlot->yAxis->setRange(12.5, 12.55);

    // make left and bottom axes transfer their ranges to right and top axes:
    connect(ui->customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->customPlot->xAxis2, SLOT(setRange(QCPRange)));
    connect(ui->customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->customPlot->yAxis2, SLOT(setRange(QCPRange)));

    // setup a timer that repeatedly calls MainWindow::realtimeDataSlot:
    connect(dataTimer, SIGNAL(timeout()), this, SLOT(realtimeDataSlot()));
    dataTimer->start(0); // Interval 0 means to refresh as fast as possible
    ui->customPlot->replot();


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
    graph_choice = 2;
    dataTimer->stop();

    QString filename = "/proc/meminfo";
    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly))
        QMessageBox::information(0, "info", file.errorString());
    QTextStream in(&file);
    QString line = in.readLine();
    QString memory_available;
    QString memory_total;
    QString swap_memory_cache;
    QString swap_memory_total;
    while(!line.isNull()) {
        if (line.contains("MemAvailable", Qt::CaseInsensitive)) {
            QStringList list = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);
            memory_available = list.at(1);

        }
        if (line.contains("MemTotal", Qt::CaseInsensitive)) {
            QStringList list = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);
            memory_total = list.at(1);
        }
        if (line.contains("SwapCached", Qt::CaseInsensitive)) {
            QStringList list = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);
            swap_memory_cache = list.at(1);
        }
        if (line.contains("SwapTotal", Qt::CaseInsensitive)) {
            QStringList list = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);
            swap_memory_total = list.at(1);
        }
        line = in.readLine();
    }

    bool ok = false;
    memory_usage = 100 * ((memory_available.toDouble(&ok)) / (memory_total.toDouble(&ok)));
    swap_usage = 100 * ((swap_memory_cache.toDouble(&ok)) / (swap_memory_total.toDouble(&ok)));

    ui->customPlot->addGraph();
    ui->customPlot->graph(0)->setPen(QPen(QColor(40, 110, 255)));
    ui->customPlot->addGraph();
    ui->customPlot->graph(1)->setPen(QPen(QColor(255, 110, 40)));

    QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
    timeTicker->setTimeFormat("%h:%m:%s");
    ui->customPlot->xAxis->setTicker(timeTicker);

    ui->customPlot->axisRect()->setupFullAxesBox();
    ui->customPlot->yAxis->setRange(0, 100);

    // make left and bottom axes transfer their ranges to right and top axes:
    connect(ui->customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->customPlot->xAxis2, SLOT(setRange(QCPRange)));
    connect(ui->customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->customPlot->yAxis2, SLOT(setRange(QCPRange)));

    // setup a timer that repeatedly calls MainWindow::realtimeDataSlot:
    connect(dataTimer, SIGNAL(timeout()), this, SLOT(realtimeDataSlot()));
    dataTimer->start(0); // Interval 0 means to refresh as fast as possible
    ui->customPlot->replot();


    // set a more compact font size for bottom and left axis tick labels:
    ui->customPlot->xAxis->setTickLabelFont(QFont(QFont().family(), 8));
    ui->customPlot->yAxis->setTickLabelFont(QFont(QFont().family(), 8));

    // set axis labels:
    ui->customPlot->xAxis->setLabel("TIME\n\nMEMORY AND SWAP HISTORY");
    ui->customPlot->yAxis->setLabel("MEMORY AND SWAP USAGE");

    // make top and right axes visible but without ticks and labels:
    ui->customPlot->xAxis2->setVisible(true);
    ui->customPlot->yAxis2->setVisible(true);
    ui->customPlot->xAxis2->setTicks(false);
    ui->customPlot->yAxis2->setTicks(false);
    ui->customPlot->xAxis2->setTickLabels(false);
    ui->customPlot->yAxis2->setTickLabels(false);

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
    graph_choice = 3;
    dataTimer->stop();

    QString filename = "/proc/net/dev";
    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly))
        QMessageBox::information(0, "info", file.errorString());
    QTextStream in(&file);
    QString line = in.readLine();
    line = in.readLine();
    line = in.readLine();

    bool ok = false;
    double received = 0;
    double sent = 0;

    while(!line.isNull()) {
       QStringList list = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);
       received += list.at(1).toDouble(&ok);
       sent += list.at(9).toDouble(&ok);
       line = in.readLine();
    }

    previousReceived = received;
    previousSent = sent;


//    QFile file2(filename);
//    if(!file2.open(QIODevice::ReadOnly))
//        QMessageBox::information(0, "info", file2.errorString());
//    QTextStream in2(&file2);
//    QString line2 = in2.readLine();
//    line2 = in2.readLine();
//    line2 = in2.readLine();

//    received = 0;
//    sent = 0;
//    while(!line2.isNull()) {
//       QStringList list = line2.split(QRegExp("\\s+"), QString::SkipEmptyParts);
//       received += list.at(1).toDouble(&ok);
//       sent += list.at(9).toDouble(&ok);
//       line2 = in2.readLine();
//    }

//    diffReceived = received - previousReceived;
//    diffSent = sent - previousSent;



//    qDebug("Recent Received : %f and Sent : %f", received, sent);
//    qDebug("Prev Received : %f and Sent : %f", previousReceived, previousSent);
//    qDebug("Diff Received : %f and Sent : %f", diffReceived, diffSent);

//    previousReceived = received;
//    previousSent = sent;

    ui->customPlot->addGraph();
    ui->customPlot->graph(0)->setPen(QPen(QColor(40, 110, 255)));
    ui->customPlot->addGraph();
    ui->customPlot->graph(1)->setPen(QPen(QColor(255, 110, 40)));


    QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
    timeTicker->setTimeFormat("%h:%m:%s");
    ui->customPlot->xAxis->setTicker(timeTicker);

    ui->customPlot->axisRect()->setupFullAxesBox();
    ui->customPlot->yAxis->setRange(0.0, 10.0);

    // make left and bottom axes transfer their ranges to right and top axes:
    connect(ui->customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->customPlot->xAxis2, SLOT(setRange(QCPRange)));
    connect(ui->customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->customPlot->yAxis2, SLOT(setRange(QCPRange)));

    // setup a timer that repeatedly calls MainWindow::realtimeDataSlot:
    connect(dataTimer, SIGNAL(timeout()), this, SLOT(realtimeDataSlot()));
    dataTimer->start(0); // Interval 0 means to refresh as fast as possible
    ui->customPlot->replot();


    // set a more compact font size for bottom and left axis tick labels:
    ui->customPlot->xAxis->setTickLabelFont(QFont(QFont().family(), 8));
    ui->customPlot->yAxis->setTickLabelFont(QFont(QFont().family(), 8));

    // set axis labels:
    ui->customPlot->xAxis->setLabel("TIME\n\nNETWORK HISTORY");
    ui->customPlot->yAxis->setLabel("NETWORK USAGE");

    // make top and right axes visible but without ticks and labels:
    ui->customPlot->xAxis2->setVisible(true);
    ui->customPlot->yAxis2->setVisible(true);
    ui->customPlot->xAxis2->setTicks(false);
    ui->customPlot->yAxis2->setTicks(false);
    ui->customPlot->xAxis2->setTickLabels(false);
    ui->customPlot->yAxis2->setTickLabels(false);

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
