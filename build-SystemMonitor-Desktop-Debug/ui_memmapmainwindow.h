/********************************************************************************
** Form generated from reading UI file 'memmapmainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.9.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MEMMAPMAINWINDOW_H
#define UI_MEMMAPMAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MemMapMainWindow
{
public:
    QWidget *centralwidget;
    QTreeWidget *treeWidget;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MemMapMainWindow)
    {
        if (MemMapMainWindow->objectName().isEmpty())
            MemMapMainWindow->setObjectName(QStringLiteral("MemMapMainWindow"));
        MemMapMainWindow->resize(800, 600);
        centralwidget = new QWidget(MemMapMainWindow);
        centralwidget->setObjectName(QStringLiteral("centralwidget"));
        treeWidget = new QTreeWidget(centralwidget);
        QTreeWidgetItem *__qtreewidgetitem = new QTreeWidgetItem();
        __qtreewidgetitem->setText(0, QStringLiteral("1"));
        treeWidget->setHeaderItem(__qtreewidgetitem);
        treeWidget->setObjectName(QStringLiteral("treeWidget"));
        treeWidget->setGeometry(QRect(20, 10, 761, 551));
        MemMapMainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MemMapMainWindow);
        menubar->setObjectName(QStringLiteral("menubar"));
        menubar->setGeometry(QRect(0, 0, 800, 20));
        MemMapMainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MemMapMainWindow);
        statusbar->setObjectName(QStringLiteral("statusbar"));
        MemMapMainWindow->setStatusBar(statusbar);

        retranslateUi(MemMapMainWindow);

        QMetaObject::connectSlotsByName(MemMapMainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MemMapMainWindow)
    {
        MemMapMainWindow->setWindowTitle(QApplication::translate("MemMapMainWindow", "MainWindow", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class MemMapMainWindow: public Ui_MemMapMainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MEMMAPMAINWINDOW_H
