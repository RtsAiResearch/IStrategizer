/********************************************************************************
** Form generated from reading ui file 'ChoosePlanStepDialog.ui'
**
** Created: Tue Mar 20 01:13:01 2012
**      by: Qt User Interface Compiler version 4.4.2
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_CHOOSEPLANSTEPDIALOG_H
#define UI_CHOOSEPLANSTEPDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_ChoosePlanStepDialogClass
{
public:
    QPushButton *btnOK;
    QPushButton *btnCancel;
    QComboBox *comboBox;
    QLabel *lblPlanStepType;

    void setupUi(QDialog *ChoosePlanStepDialogClass)
    {
    if (ChoosePlanStepDialogClass->objectName().isEmpty())
        ChoosePlanStepDialogClass->setObjectName(QString::fromUtf8("ChoosePlanStepDialogClass"));
    ChoosePlanStepDialogClass->resize(170, 72);
    btnOK = new QPushButton(ChoosePlanStepDialogClass);
    btnOK->setObjectName(QString::fromUtf8("btnOK"));
    btnOK->setGeometry(QRect(10, 40, 71, 25));
    btnCancel = new QPushButton(ChoosePlanStepDialogClass);
    btnCancel->setObjectName(QString::fromUtf8("btnCancel"));
    btnCancel->setGeometry(QRect(90, 40, 71, 25));
    comboBox = new QComboBox(ChoosePlanStepDialogClass);
    comboBox->setObjectName(QString::fromUtf8("comboBox"));
    comboBox->setGeometry(QRect(10, 10, 101, 20));
    comboBox->setFocusPolicy(Qt::WheelFocus);
    lblPlanStepType = new QLabel(ChoosePlanStepDialogClass);
    lblPlanStepType->setObjectName(QString::fromUtf8("lblPlanStepType"));
    lblPlanStepType->setGeometry(QRect(120, 10, 31, 20));

    retranslateUi(ChoosePlanStepDialogClass);

    QMetaObject::connectSlotsByName(ChoosePlanStepDialogClass);
    } // setupUi

    void retranslateUi(QDialog *ChoosePlanStepDialogClass)
    {
    ChoosePlanStepDialogClass->setWindowTitle(QApplication::translate("ChoosePlanStepDialogClass", "ChoosePlanStepDialog", 0, QApplication::UnicodeUTF8));
    btnOK->setText(QApplication::translate("ChoosePlanStepDialogClass", "OK", 0, QApplication::UnicodeUTF8));
    btnCancel->setText(QApplication::translate("ChoosePlanStepDialogClass", "Cancel", 0, QApplication::UnicodeUTF8));
    lblPlanStepType->setText(QApplication::translate("ChoosePlanStepDialogClass", "Type", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(ChoosePlanStepDialogClass);
    } // retranslateUi

};

namespace Ui {
    class ChoosePlanStepDialogClass: public Ui_ChoosePlanStepDialogClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CHOOSEPLANSTEPDIALOG_H
