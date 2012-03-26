/********************************************************************************
** Form generated from reading ui file 'PlanStepView.ui'
**
** Created: Tue Mar 20 01:13:00 2012
**      by: Qt User Interface Compiler version 4.4.2
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_PLANSTEPVIEW_H
#define UI_PLANSTEPVIEW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QLabel>
#include <QtGui/QTableWidget>
#include <QtGui/QToolBox>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_PlanStepViewClass
{
public:
    QToolBox *toolBox;
    QWidget *Parameters;
    QTableWidget *tblParameters;
    QWidget *Conditions;
    QLabel *lblPlanStep;

    void setupUi(QWidget *PlanStepViewClass)
    {
    if (PlanStepViewClass->objectName().isEmpty())
        PlanStepViewClass->setObjectName(QString::fromUtf8("PlanStepViewClass"));
    PlanStepViewClass->resize(260, 450);
    toolBox = new QToolBox(PlanStepViewClass);
    toolBox->setObjectName(QString::fromUtf8("toolBox"));
    toolBox->setGeometry(QRect(10, 30, 241, 411));
    toolBox->setLayoutDirection(Qt::RightToLeft);
    Parameters = new QWidget();
    Parameters->setObjectName(QString::fromUtf8("Parameters"));
    Parameters->setGeometry(QRect(0, 0, 241, 355));
    tblParameters = new QTableWidget(Parameters);
    tblParameters->setObjectName(QString::fromUtf8("tblParameters"));
    tblParameters->setGeometry(QRect(10, 10, 221, 331));
    QFont font;
    font.setFamily(QString::fromUtf8("Tahoma"));
    font.setPointSize(8);
    font.setBold(false);
    font.setItalic(false);
    font.setUnderline(false);
    font.setWeight(50);
    font.setStrikeOut(false);
    tblParameters->setFont(font);
    tblParameters->setLayoutDirection(Qt::LeftToRight);
    tblParameters->setFrameShape(QFrame::StyledPanel);
    tblParameters->setFrameShadow(QFrame::Plain);
    tblParameters->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tblParameters->setSelectionMode(QAbstractItemView::SingleSelection);
    tblParameters->setSelectionBehavior(QAbstractItemView::SelectRows);
    tblParameters->setShowGrid(false);
    tblParameters->setGridStyle(Qt::NoPen);
    tblParameters->setCornerButtonEnabled(false);
    toolBox->addItem(Parameters, QApplication::translate("PlanStepViewClass", "Parameters", 0, QApplication::UnicodeUTF8));
    Conditions = new QWidget();
    Conditions->setObjectName(QString::fromUtf8("Conditions"));
    Conditions->setGeometry(QRect(0, 0, 241, 355));
    toolBox->addItem(Conditions, QApplication::translate("PlanStepViewClass", "Conditions", 0, QApplication::UnicodeUTF8));
    lblPlanStep = new QLabel(PlanStepViewClass);
    lblPlanStep->setObjectName(QString::fromUtf8("lblPlanStep"));
    lblPlanStep->setGeometry(QRect(10, 0, 241, 31));
    QFont font1;
    font1.setFamily(QString::fromUtf8("Tahoma"));
    font1.setPointSize(10);
    font1.setBold(true);
    font1.setItalic(false);
    font1.setUnderline(false);
    font1.setWeight(75);
    font1.setStrikeOut(false);
    lblPlanStep->setFont(font1);
    lblPlanStep->setMargin(4);
    lblPlanStep->setIndent(0);

    retranslateUi(PlanStepViewClass);

    toolBox->setCurrentIndex(0);


    QMetaObject::connectSlotsByName(PlanStepViewClass);
    } // setupUi

    void retranslateUi(QWidget *PlanStepViewClass)
    {
    PlanStepViewClass->setWindowTitle(QApplication::translate("PlanStepViewClass", "PlanStepView", 0, QApplication::UnicodeUTF8));
    toolBox->setItemText(toolBox->indexOf(Parameters), QApplication::translate("PlanStepViewClass", "Parameters", 0, QApplication::UnicodeUTF8));
    toolBox->setItemText(toolBox->indexOf(Conditions), QApplication::translate("PlanStepViewClass", "Conditions", 0, QApplication::UnicodeUTF8));
    lblPlanStep->setText(QApplication::translate("PlanStepViewClass", "Plan Step Name", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(PlanStepViewClass);
    } // retranslateUi

};

namespace Ui {
    class PlanStepViewClass: public Ui_PlanStepViewClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PLANSTEPVIEW_H
