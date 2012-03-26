/********************************************************************************
** Form generated from reading ui file 'PlanGraphView.ui'
**
** Created: Tue Mar 20 01:13:00 2012
**      by: Qt User Interface Compiler version 4.4.2
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_PLANGRAPHVIEW_H
#define UI_PLANGRAPHVIEW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_PlanGraphViewClass
{
public:

    void setupUi(QWidget *PlanGraphViewClass)
    {
    if (PlanGraphViewClass->objectName().isEmpty())
        PlanGraphViewClass->setObjectName(QString::fromUtf8("PlanGraphViewClass"));
    PlanGraphViewClass->resize(805, 551);
    PlanGraphViewClass->setFocusPolicy(Qt::NoFocus);

    retranslateUi(PlanGraphViewClass);

    QMetaObject::connectSlotsByName(PlanGraphViewClass);
    } // setupUi

    void retranslateUi(QWidget *PlanGraphViewClass)
    {
    PlanGraphViewClass->setWindowTitle(QApplication::translate("PlanGraphViewClass", "PlanGraphView", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(PlanGraphViewClass);
    } // retranslateUi

};

namespace Ui {
    class PlanGraphViewClass: public Ui_PlanGraphViewClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PLANGRAPHVIEW_H
