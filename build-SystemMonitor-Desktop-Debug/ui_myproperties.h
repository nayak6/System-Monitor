/********************************************************************************
** Form generated from reading UI file 'myproperties.ui'
**
** Created by: Qt User Interface Compiler version 5.9.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MYPROPERTIES_H
#define UI_MYPROPERTIES_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MyProperties
{
public:
    QWidget *centralwidget;
    QListWidget *listWidget;
    QMenuBar *menubar;
    QMenu *menuProperties;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MyProperties)
    {
        if (MyProperties->objectName().isEmpty())
            MyProperties->setObjectName(QStringLiteral("MyProperties"));
        MyProperties->resize(800, 600);
        centralwidget = new QWidget(MyProperties);
        centralwidget->setObjectName(QStringLiteral("centralwidget"));
        listWidget = new QListWidget(centralwidget);
        listWidget->setObjectName(QStringLiteral("listWidget"));
        listWidget->setGeometry(QRect(10, 10, 531, 321));
        MyProperties->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MyProperties);
        menubar->setObjectName(QStringLiteral("menubar"));
        menubar->setGeometry(QRect(0, 0, 800, 20));
        menuProperties = new QMenu(menubar);
        menuProperties->setObjectName(QStringLiteral("menuProperties"));
        MyProperties->setMenuBar(menubar);
        statusbar = new QStatusBar(MyProperties);
        statusbar->setObjectName(QStringLiteral("statusbar"));
        MyProperties->setStatusBar(statusbar);

        menubar->addAction(menuProperties->menuAction());
        menuProperties->addSeparator();

        retranslateUi(MyProperties);

        QMetaObject::connectSlotsByName(MyProperties);
    } // setupUi

    void retranslateUi(QMainWindow *MyProperties)
    {
        MyProperties->setWindowTitle(QApplication::translate("MyProperties", "MainWindow", Q_NULLPTR));
        menuProperties->setTitle(QApplication::translate("MyProperties", "Properties", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class MyProperties: public Ui_MyProperties {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MYPROPERTIES_H
