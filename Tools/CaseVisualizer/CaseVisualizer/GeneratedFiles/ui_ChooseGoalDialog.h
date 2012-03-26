/********************************************************************************
** Form generated from reading ui file 'ChooseGoalDialog.ui'
**
** Created: Sun May 22 09:54:35 2011
**      by: Qt User Interface Compiler version 4.4.2
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_CHOOSEGOALDIALOG_H
#define UI_CHOOSEGOALDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>

QT_BEGIN_NAMESPACE

class Ui_ChooseGoalDialogClass
{
public:

    void setupUi(QDialog *ChooseGoalDialogClass)
    {
    if (ChooseGoalDialogClass->objectName().isEmpty())
        ChooseGoalDialogClass->setObjectName(QString::fromUtf8("ChooseGoalDialogClass"));
    ChooseGoalDialogClass->resize(180, 69);
    ChooseGoalDialogClass->setFocusPolicy(Qt::StrongFocus);

    retranslateUi(ChooseGoalDialogClass);

    QMetaObject::connectSlotsByName(ChooseGoalDialogClass);
    } // setupUi

    void retranslateUi(QDialog *ChooseGoalDialogClass)
    {
    ChooseGoalDialogClass->setWindowTitle(QApplication::translate("ChooseGoalDialogClass", "ChooseGoalDialog", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(ChooseGoalDialogClass);
    } // retranslateUi

};

namespace Ui {
    class ChooseGoalDialogClass: public Ui_ChooseGoalDialogClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CHOOSEGOALDIALOG_H
