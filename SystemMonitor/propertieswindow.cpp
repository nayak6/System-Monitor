#include "PropertiesWindow.h"
#include "ui_PropertiesWindow.h"

PropertiesWindow::PropertiesWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::PropertiesWindow)
{
    ui->setupUi(this);
}

PropertiesWindow::~PropertiesWindow()
{
    delete ui;
}
