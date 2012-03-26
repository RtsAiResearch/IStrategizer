/********************************************************************************
** Form generated from reading ui file 'ClientMain.ui'
**
** Created: Sun Mar 25 13:53:12 2012
**      by: Qt User Interface Compiler version 4.4.2
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_CLIENTMAIN_H
#define UI_CLIENTMAIN_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGridLayout>
#include <QtGui/QLabel>
#include <QtGui/QMainWindow>
#include <QtGui/QMenuBar>
#include <QtGui/QStatusBar>
#include <QtGui/QTabWidget>
#include <QtGui/QToolBar>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ClientMainClass
{
public:
    QWidget *centralWidget;
    QTabWidget *tbMainView;
    QWidget *tbEngineStatus;
    QWidget *gridLayoutWidget;
    QGridLayout *gridLayout;
    QLabel *lblGameCycles;
    QLabel *lblGameCyclesData;
    QLabel *lblRatio;
    QLabel *lblEngineCycles;
    QLabel *lblEngineCycleData;
    QLabel *lblGameRatioData;
    QLabel *lblEngineRatioData;
    QLabel *lblRatioTo;
    QWidget *tbBuildingIM;
    QWidget *tbGrndCtrlIM;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *ClientMainClass)
    {
    if (ClientMainClass->objectName().isEmpty())
        ClientMainClass->setObjectName(QString::fromUtf8("ClientMainClass"));
    ClientMainClass->resize(800, 800);
    ClientMainClass->setMinimumSize(QSize(800, 800));
    ClientMainClass->setMaximumSize(QSize(800, 800));
    ClientMainClass->setContextMenuPolicy(Qt::NoContextMenu);
    centralWidget = new QWidget(ClientMainClass);
    centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
    tbMainView = new QTabWidget(centralWidget);
    tbMainView->setObjectName(QString::fromUtf8("tbMainView"));
    tbMainView->setGeometry(QRect(0, 0, 800, 750));
    QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(tbMainView->sizePolicy().hasHeightForWidth());
    tbMainView->setSizePolicy(sizePolicy);
    tbMainView->setMinimumSize(QSize(800, 750));
    tbMainView->setMaximumSize(QSize(800, 750));
    tbMainView->setAutoFillBackground(true);
    tbMainView->setTabPosition(QTabWidget::North);
    tbMainView->setTabShape(QTabWidget::Rounded);
    tbMainView->setElideMode(Qt::ElideNone);
    tbEngineStatus = new QWidget();
    tbEngineStatus->setObjectName(QString::fromUtf8("tbEngineStatus"));
    gridLayoutWidget = new QWidget(tbEngineStatus);
    gridLayoutWidget->setObjectName(QString::fromUtf8("gridLayoutWidget"));
    gridLayoutWidget->setGeometry(QRect(10, 10, 291, 92));
    gridLayout = new QGridLayout(gridLayoutWidget);
    gridLayout->setSpacing(6);
    gridLayout->setMargin(11);
    gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
    gridLayout->setContentsMargins(0, 0, 0, 0);
    lblGameCycles = new QLabel(gridLayoutWidget);
    lblGameCycles->setObjectName(QString::fromUtf8("lblGameCycles"));

    gridLayout->addWidget(lblGameCycles, 3, 0, 1, 1);

    lblGameCyclesData = new QLabel(gridLayoutWidget);
    lblGameCyclesData->setObjectName(QString::fromUtf8("lblGameCyclesData"));
    lblGameCyclesData->setAlignment(Qt::AlignCenter);

    gridLayout->addWidget(lblGameCyclesData, 3, 1, 1, 1);

    lblRatio = new QLabel(gridLayoutWidget);
    lblRatio->setObjectName(QString::fromUtf8("lblRatio"));

    gridLayout->addWidget(lblRatio, 4, 0, 1, 1);

    lblEngineCycles = new QLabel(gridLayoutWidget);
    lblEngineCycles->setObjectName(QString::fromUtf8("lblEngineCycles"));

    gridLayout->addWidget(lblEngineCycles, 3, 2, 1, 1);

    lblEngineCycleData = new QLabel(gridLayoutWidget);
    lblEngineCycleData->setObjectName(QString::fromUtf8("lblEngineCycleData"));
    lblEngineCycleData->setAlignment(Qt::AlignCenter);

    gridLayout->addWidget(lblEngineCycleData, 3, 3, 1, 1);

    lblGameRatioData = new QLabel(gridLayoutWidget);
    lblGameRatioData->setObjectName(QString::fromUtf8("lblGameRatioData"));
    lblGameRatioData->setAlignment(Qt::AlignCenter);

    gridLayout->addWidget(lblGameRatioData, 4, 1, 1, 1);

    lblEngineRatioData = new QLabel(gridLayoutWidget);
    lblEngineRatioData->setObjectName(QString::fromUtf8("lblEngineRatioData"));
    lblEngineRatioData->setAlignment(Qt::AlignCenter);

    gridLayout->addWidget(lblEngineRatioData, 4, 3, 1, 1);

    lblRatioTo = new QLabel(gridLayoutWidget);
    lblRatioTo->setObjectName(QString::fromUtf8("lblRatioTo"));
    lblRatioTo->setAlignment(Qt::AlignCenter);

    gridLayout->addWidget(lblRatioTo, 4, 2, 1, 1);

    tbMainView->addTab(tbEngineStatus, QString());
    tbBuildingIM = new QWidget();
    tbBuildingIM->setObjectName(QString::fromUtf8("tbBuildingIM"));
    tbMainView->addTab(tbBuildingIM, QString());
    tbGrndCtrlIM = new QWidget();
    tbGrndCtrlIM->setObjectName(QString::fromUtf8("tbGrndCtrlIM"));
    tbMainView->addTab(tbGrndCtrlIM, QString());
    ClientMainClass->setCentralWidget(centralWidget);
    menuBar = new QMenuBar(ClientMainClass);
    menuBar->setObjectName(QString::fromUtf8("menuBar"));
    menuBar->setGeometry(QRect(0, 0, 800, 27));
    ClientMainClass->setMenuBar(menuBar);
    mainToolBar = new QToolBar(ClientMainClass);
    mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
    mainToolBar->setAllowedAreas(Qt::NoToolBarArea);
    ClientMainClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
    statusBar = new QStatusBar(ClientMainClass);
    statusBar->setObjectName(QString::fromUtf8("statusBar"));
    ClientMainClass->setStatusBar(statusBar);

    retranslateUi(ClientMainClass);

    tbMainView->setCurrentIndex(0);


    QMetaObject::connectSlotsByName(ClientMainClass);
    } // setupUi

    void retranslateUi(QMainWindow *ClientMainClass)
    {
    ClientMainClass->setWindowTitle(QApplication::translate("ClientMainClass", "IStrategizer BWAPI Client", 0, QApplication::UnicodeUTF8));
    lblGameCycles->setText(QApplication::translate("ClientMainClass", "Game Cycles", 0, QApplication::UnicodeUTF8));
    lblGameCyclesData->setText(QApplication::translate("ClientMainClass", "0", 0, QApplication::UnicodeUTF8));
    lblRatio->setText(QApplication::translate("ClientMainClass", "Ratio", 0, QApplication::UnicodeUTF8));
    lblEngineCycles->setText(QApplication::translate("ClientMainClass", "Engine Cycles", 0, QApplication::UnicodeUTF8));
    lblEngineCycleData->setText(QApplication::translate("ClientMainClass", "0", 0, QApplication::UnicodeUTF8));
    lblGameRatioData->setText(QApplication::translate("ClientMainClass", "1", 0, QApplication::UnicodeUTF8));
    lblEngineRatioData->setText(QApplication::translate("ClientMainClass", "1", 0, QApplication::UnicodeUTF8));
    lblRatioTo->setText(QApplication::translate("ClientMainClass", ":", 0, QApplication::UnicodeUTF8));
    tbMainView->setTabText(tbMainView->indexOf(tbEngineStatus), QApplication::translate("ClientMainClass", "Status", 0, QApplication::UnicodeUTF8));
    tbMainView->setTabText(tbMainView->indexOf(tbBuildingIM), QApplication::translate("ClientMainClass", "Building Data IM", 0, QApplication::UnicodeUTF8));
    tbMainView->setTabText(tbMainView->indexOf(tbGrndCtrlIM), QApplication::translate("ClientMainClass", "Ground Control IM", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(ClientMainClass);
    } // retranslateUi

};

namespace Ui {
    class ClientMainClass: public Ui_ClientMainClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CLIENTMAIN_H
