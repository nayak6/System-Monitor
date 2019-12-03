/********************************************************************************
** Form generated from reading UI file 'processfiles.ui'
**
** Created by: Qt User Interface Compiler version 5.9.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PROCESSFILES_H
#define UI_PROCESSFILES_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QTreeWidget>

QT_BEGIN_NAMESPACE

class Ui_ProcessFiles
{
public:
    QTreeWidget *treeWidget;

    void setupUi(QDialog *ProcessFiles)
    {
        if (ProcessFiles->objectName().isEmpty())
            ProcessFiles->setObjectName(QStringLiteral("ProcessFiles"));
        ProcessFiles->resize(400, 300);
        treeWidget = new QTreeWidget(ProcessFiles);
        treeWidget->headerItem()->setText(0, QString());
        treeWidget->setObjectName(QStringLiteral("treeWidget"));
        treeWidget->setGeometry(QRect(0, 0, 401, 301));

        retranslateUi(ProcessFiles);

        QMetaObject::connectSlotsByName(ProcessFiles);
    } // setupUi

    void retranslateUi(QDialog *ProcessFiles)
    {
        ProcessFiles->setWindowTitle(QApplication::translate("ProcessFiles", "Dialog", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class ProcessFiles: public Ui_ProcessFiles {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PROCESSFILES_H
