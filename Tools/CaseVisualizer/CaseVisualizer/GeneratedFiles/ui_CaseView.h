/********************************************************************************
** Form generated from reading ui file 'CaseView.ui'
**
** Created: Tue Mar 20 01:13:01 2012
**      by: Qt User Interface Compiler version 4.4.2
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_CASEVIEW_H
#define UI_CASEVIEW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QFrame>
#include <QtGui/QLabel>
#include <QtGui/QTableWidget>
#include <QtGui/QToolBox>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_CaseViewClass
{
public:
    QFrame *frmPlanView;
    QToolBox *toolBox;
    QWidget *Goal;
    QTableWidget *tblParameters;
    QLabel *lblGoalName;
    QWidget *Game_State;
    QTableWidget *tblGameState;
    QWidget *Performance;

    void setupUi(QWidget *CaseViewClass)
    {
    if (CaseViewClass->objectName().isEmpty())
        CaseViewClass->setObjectName(QString::fromUtf8("CaseViewClass"));
    CaseViewClass->resize(1020, 499);
    frmPlanView = new QFrame(CaseViewClass);
    frmPlanView->setObjectName(QString::fromUtf8("frmPlanView"));
    frmPlanView->setGeometry(QRect(260, 10, 751, 480));
    QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(frmPlanView->sizePolicy().hasHeightForWidth());
    frmPlanView->setSizePolicy(sizePolicy);
    frmPlanView->setFocusPolicy(Qt::NoFocus);
    frmPlanView->setFrameShape(QFrame::NoFrame);
    frmPlanView->setFrameShadow(QFrame::Plain);
    toolBox = new QToolBox(CaseViewClass);
    toolBox->setObjectName(QString::fromUtf8("toolBox"));
    toolBox->setGeometry(QRect(10, 10, 241, 481));
    toolBox->setFrameShadow(QFrame::Plain);
    Goal = new QWidget();
    Goal->setObjectName(QString::fromUtf8("Goal"));
    Goal->setGeometry(QRect(0, 0, 241, 388));
    tblParameters = new QTableWidget(Goal);
    tblParameters->setObjectName(QString::fromUtf8("tblParameters"));
    tblParameters->setGeometry(QRect(10, 40, 221, 341));
    QFont font;
    font.setFamily(QString::fromUtf8("Tahoma"));
    font.setPointSize(8);
    font.setBold(false);
    font.setItalic(false);
    font.setUnderline(false);
    font.setWeight(50);
    font.setStrikeOut(false);
    tblParameters->setFont(font);
    tblParameters->setFrameShape(QFrame::StyledPanel);
    tblParameters->setFrameShadow(QFrame::Plain);
    tblParameters->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tblParameters->setSelectionBehavior(QAbstractItemView::SelectRows);
    tblParameters->setShowGrid(false);
    tblParameters->setGridStyle(Qt::NoPen);
    tblParameters->setCornerButtonEnabled(false);
    lblGoalName = new QLabel(Goal);
    lblGoalName->setObjectName(QString::fromUtf8("lblGoalName"));
    lblGoalName->setGeometry(QRect(10, 10, 151, 20));
    QFont font1;
    font1.setFamily(QString::fromUtf8("MS Shell Dlg 2"));
    font1.setPointSize(12);
    font1.setBold(true);
    font1.setItalic(false);
    font1.setUnderline(false);
    font1.setWeight(75);
    font1.setStrikeOut(false);
    lblGoalName->setFont(font1);
    lblGoalName->setTextFormat(Qt::AutoText);
    toolBox->addItem(Goal, QApplication::translate("CaseViewClass", "Goal", 0, QApplication::UnicodeUTF8));
    Game_State = new QWidget();
    Game_State->setObjectName(QString::fromUtf8("Game_State"));
    Game_State->setGeometry(QRect(0, 0, 241, 388));
    tblGameState = new QTableWidget(Game_State);
    tblGameState->setObjectName(QString::fromUtf8("tblGameState"));
    tblGameState->setGeometry(QRect(10, 0, 221, 391));
    tblGameState->setFont(font);
    tblGameState->setFrameShadow(QFrame::Plain);
    tblGameState->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    tblGameState->setProperty("showDropIndicator", QVariant(false));
    tblGameState->setAlternatingRowColors(true);
    tblGameState->setSelectionMode(QAbstractItemView::NoSelection);
    tblGameState->setShowGrid(false);
    tblGameState->setGridStyle(Qt::NoPen);
    tblGameState->setCornerButtonEnabled(false);
    toolBox->addItem(Game_State, QApplication::translate("CaseViewClass", "Game State", 0, QApplication::UnicodeUTF8));
    Performance = new QWidget();
    Performance->setObjectName(QString::fromUtf8("Performance"));
    Performance->setGeometry(QRect(0, 0, 241, 388));
    toolBox->addItem(Performance, QApplication::translate("CaseViewClass", "Performance", 0, QApplication::UnicodeUTF8));

    retranslateUi(CaseViewClass);

    toolBox->setCurrentIndex(0);


    QMetaObject::connectSlotsByName(CaseViewClass);
    } // setupUi

    void retranslateUi(QWidget *CaseViewClass)
    {
    CaseViewClass->setWindowTitle(QApplication::translate("CaseViewClass", "CaseView", 0, QApplication::UnicodeUTF8));
    lblGoalName->setText(QApplication::translate("CaseViewClass", "Goal Name", 0, QApplication::UnicodeUTF8));
    toolBox->setItemText(toolBox->indexOf(Goal), QApplication::translate("CaseViewClass", "Goal", 0, QApplication::UnicodeUTF8));
    toolBox->setItemText(toolBox->indexOf(Game_State), QApplication::translate("CaseViewClass", "Game State", 0, QApplication::UnicodeUTF8));
    toolBox->setItemText(toolBox->indexOf(Performance), QApplication::translate("CaseViewClass", "Performance", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(CaseViewClass);
    } // retranslateUi

};

namespace Ui {
    class CaseViewClass: public Ui_CaseViewClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CASEVIEW_H
