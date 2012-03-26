/********************************************************************************
** Form generated from reading ui file 'CaseVisualizer.ui'
**
** Created: Tue Mar 20 01:13:01 2012
**      by: Qt User Interface Compiler version 4.4.2
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_CASEVISUALIZER_H
#define UI_CASEVISUALIZER_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDockWidget>
#include <QtGui/QFrame>
#include <QtGui/QListWidget>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QPushButton>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_CaseVisualizerClass
{
public:
    QAction *actionOpen;
    QAction *actionNew;
    QAction *actionSave;
    QAction *actionShow;
    QWidget *centralWidget;
    QFrame *caseViewFrame;
    QMenuBar *menuBar;
    QMenu *menuCaseBase;
    QDockWidget *caseBaseViewDockWidget;
    QWidget *dockWidgetContents_2;
    QListWidget *lstCases;
    QPushButton *btnDeleteCase;
    QPushButton *btnNewCase;

    void setupUi(QMainWindow *CaseVisualizerClass)
    {
    if (CaseVisualizerClass->objectName().isEmpty())
        CaseVisualizerClass->setObjectName(QString::fromUtf8("CaseVisualizerClass"));
    CaseVisualizerClass->resize(1221, 588);
    QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(CaseVisualizerClass->sizePolicy().hasHeightForWidth());
    CaseVisualizerClass->setSizePolicy(sizePolicy);
    CaseVisualizerClass->setMaximumSize(QSize(2000, 2000));
    CaseVisualizerClass->setContextMenuPolicy(Qt::NoContextMenu);
    CaseVisualizerClass->setToolButtonStyle(Qt::ToolButtonTextOnly);
    actionOpen = new QAction(CaseVisualizerClass);
    actionOpen->setObjectName(QString::fromUtf8("actionOpen"));
    actionNew = new QAction(CaseVisualizerClass);
    actionNew->setObjectName(QString::fromUtf8("actionNew"));
    actionSave = new QAction(CaseVisualizerClass);
    actionSave->setObjectName(QString::fromUtf8("actionSave"));
    actionShow = new QAction(CaseVisualizerClass);
    actionShow->setObjectName(QString::fromUtf8("actionShow"));
    centralWidget = new QWidget(CaseVisualizerClass);
    centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
    caseViewFrame = new QFrame(centralWidget);
    caseViewFrame->setObjectName(QString::fromUtf8("caseViewFrame"));
    caseViewFrame->setGeometry(QRect(10, 10, 991, 521));
    sizePolicy.setHeightForWidth(caseViewFrame->sizePolicy().hasHeightForWidth());
    caseViewFrame->setSizePolicy(sizePolicy);
    caseViewFrame->setContextMenuPolicy(Qt::NoContextMenu);
    caseViewFrame->setFrameShape(QFrame::NoFrame);
    caseViewFrame->setFrameShadow(QFrame::Plain);
    CaseVisualizerClass->setCentralWidget(centralWidget);
    menuBar = new QMenuBar(CaseVisualizerClass);
    menuBar->setObjectName(QString::fromUtf8("menuBar"));
    menuBar->setGeometry(QRect(0, 0, 1221, 25));
    menuCaseBase = new QMenu(menuBar);
    menuCaseBase->setObjectName(QString::fromUtf8("menuCaseBase"));
    CaseVisualizerClass->setMenuBar(menuBar);
    caseBaseViewDockWidget = new QDockWidget(CaseVisualizerClass);
    caseBaseViewDockWidget->setObjectName(QString::fromUtf8("caseBaseViewDockWidget"));
    caseBaseViewDockWidget->setMinimumSize(QSize(208, 540));
    caseBaseViewDockWidget->setMaximumSize(QSize(208, 540));
    caseBaseViewDockWidget->setAllowedAreas(Qt::LeftDockWidgetArea|Qt::RightDockWidgetArea);
    dockWidgetContents_2 = new QWidget();
    dockWidgetContents_2->setObjectName(QString::fromUtf8("dockWidgetContents_2"));
    lstCases = new QListWidget(dockWidgetContents_2);
    lstCases->setObjectName(QString::fromUtf8("lstCases"));
    lstCases->setGeometry(QRect(10, 10, 191, 401));
    QFont font;
    font.setFamily(QString::fromUtf8("Tahoma"));
    font.setPointSize(8);
    font.setBold(false);
    font.setItalic(false);
    font.setUnderline(false);
    font.setWeight(50);
    font.setStrikeOut(false);
    lstCases->setFont(font);
    lstCases->setFrameShadow(QFrame::Plain);
    lstCases->setSpacing(2);
    btnDeleteCase = new QPushButton(dockWidgetContents_2);
    btnDeleteCase->setObjectName(QString::fromUtf8("btnDeleteCase"));
    btnDeleteCase->setGeometry(QRect(10, 420, 91, 23));
    btnNewCase = new QPushButton(dockWidgetContents_2);
    btnNewCase->setObjectName(QString::fromUtf8("btnNewCase"));
    btnNewCase->setGeometry(QRect(10, 450, 91, 23));
    caseBaseViewDockWidget->setWidget(dockWidgetContents_2);
    CaseVisualizerClass->addDockWidget(static_cast<Qt::DockWidgetArea>(2), caseBaseViewDockWidget);
    QWidget::setTabOrder(lstCases, btnDeleteCase);
    QWidget::setTabOrder(btnDeleteCase, btnNewCase);

    menuBar->addAction(menuCaseBase->menuAction());
    menuCaseBase->addAction(actionOpen);
    menuCaseBase->addAction(actionNew);
    menuCaseBase->addAction(actionSave);
    menuCaseBase->addSeparator();
    menuCaseBase->addAction(actionShow);

    retranslateUi(CaseVisualizerClass);

    QMetaObject::connectSlotsByName(CaseVisualizerClass);
    } // setupUi

    void retranslateUi(QMainWindow *CaseVisualizerClass)
    {
    CaseVisualizerClass->setWindowTitle(QApplication::translate("CaseVisualizerClass", "Case Visualizer", 0, QApplication::UnicodeUTF8));
    actionOpen->setText(QApplication::translate("CaseVisualizerClass", "Open", 0, QApplication::UnicodeUTF8));
    actionNew->setText(QApplication::translate("CaseVisualizerClass", "New", 0, QApplication::UnicodeUTF8));
    actionSave->setText(QApplication::translate("CaseVisualizerClass", "Save", 0, QApplication::UnicodeUTF8));
    actionShow->setText(QApplication::translate("CaseVisualizerClass", "Show Explorer", 0, QApplication::UnicodeUTF8));
    actionShow->setShortcut(QApplication::translate("CaseVisualizerClass", "Ctrl+B", 0, QApplication::UnicodeUTF8));
    menuCaseBase->setTitle(QApplication::translate("CaseVisualizerClass", "Case Base", 0, QApplication::UnicodeUTF8));
    caseBaseViewDockWidget->setWindowTitle(QApplication::translate("CaseVisualizerClass", "Case Base Explorer", 0, QApplication::UnicodeUTF8));
    btnDeleteCase->setText(QApplication::translate("CaseVisualizerClass", "Delete Case", 0, QApplication::UnicodeUTF8));
    btnNewCase->setText(QApplication::translate("CaseVisualizerClass", "New Case", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class CaseVisualizerClass: public Ui_CaseVisualizerClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CASEVISUALIZER_H
