/********************************************************************************
** Form generated from reading ui file 'IMViewWidget.ui'
**
** Created: Sun Mar 25 13:53:12 2012
**      by: Qt User Interface Compiler version 4.4.2
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_IMVIEWWIDGET_H
#define UI_IMVIEWWIDGET_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGridLayout>
#include <QtGui/QLabel>
#include <QtGui/QSpacerItem>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_IMViewWidgetClass
{
public:
    QGridLayout *imViewWidgetLayout;
    QGridLayout *gridLayout;
    QLabel *lblWorldSize;
    QLabel *lblWorlSizeData;
    QLabel *lblGridSize;
    QLabel *lblGridSizeData;
    QLabel *lblCellSize;
    QLabel *lblCellSizeData;
    QLabel *lblNumRegObj;
    QLabel *lblNumRegObjData;
    QSpacerItem *horizontalSpacer;
    QSpacerItem *horizontalSpacer_2;
    QSpacerItem *imCanvasSpacer;

    void setupUi(QWidget *IMViewWidgetClass)
    {
    if (IMViewWidgetClass->objectName().isEmpty())
        IMViewWidgetClass->setObjectName(QString::fromUtf8("IMViewWidgetClass"));
    IMViewWidgetClass->resize(800, 678);
    QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(IMViewWidgetClass->sizePolicy().hasHeightForWidth());
    IMViewWidgetClass->setSizePolicy(sizePolicy);
    IMViewWidgetClass->setMinimumSize(QSize(800, 650));
    IMViewWidgetClass->setMaximumSize(QSize(800, 678));
    IMViewWidgetClass->setAutoFillBackground(true);
    imViewWidgetLayout = new QGridLayout(IMViewWidgetClass);
    imViewWidgetLayout->setSpacing(0);
    imViewWidgetLayout->setMargin(0);
    imViewWidgetLayout->setObjectName(QString::fromUtf8("imViewWidgetLayout"));
    gridLayout = new QGridLayout();
    gridLayout->setSpacing(0);
    gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
    gridLayout->setSizeConstraint(QLayout::SetDefaultConstraint);
    gridLayout->setContentsMargins(0, -1, -1, -1);
    lblWorldSize = new QLabel(IMViewWidgetClass);
    lblWorldSize->setObjectName(QString::fromUtf8("lblWorldSize"));
    lblWorldSize->setMargin(10);

    gridLayout->addWidget(lblWorldSize, 1, 0, 1, 1);

    lblWorlSizeData = new QLabel(IMViewWidgetClass);
    lblWorlSizeData->setObjectName(QString::fromUtf8("lblWorlSizeData"));
    lblWorlSizeData->setMargin(10);

    gridLayout->addWidget(lblWorlSizeData, 1, 1, 1, 1);

    lblGridSize = new QLabel(IMViewWidgetClass);
    lblGridSize->setObjectName(QString::fromUtf8("lblGridSize"));
    lblGridSize->setMargin(10);

    gridLayout->addWidget(lblGridSize, 2, 0, 1, 1);

    lblGridSizeData = new QLabel(IMViewWidgetClass);
    lblGridSizeData->setObjectName(QString::fromUtf8("lblGridSizeData"));
    lblGridSizeData->setMargin(10);

    gridLayout->addWidget(lblGridSizeData, 2, 1, 1, 1);

    lblCellSize = new QLabel(IMViewWidgetClass);
    lblCellSize->setObjectName(QString::fromUtf8("lblCellSize"));
    lblCellSize->setMargin(10);

    gridLayout->addWidget(lblCellSize, 1, 3, 1, 1);

    lblCellSizeData = new QLabel(IMViewWidgetClass);
    lblCellSizeData->setObjectName(QString::fromUtf8("lblCellSizeData"));
    lblCellSizeData->setMargin(10);

    gridLayout->addWidget(lblCellSizeData, 1, 4, 1, 1);

    lblNumRegObj = new QLabel(IMViewWidgetClass);
    lblNumRegObj->setObjectName(QString::fromUtf8("lblNumRegObj"));
    lblNumRegObj->setMargin(10);

    gridLayout->addWidget(lblNumRegObj, 2, 3, 1, 1);

    lblNumRegObjData = new QLabel(IMViewWidgetClass);
    lblNumRegObjData->setObjectName(QString::fromUtf8("lblNumRegObjData"));
    lblNumRegObjData->setMargin(10);

    gridLayout->addWidget(lblNumRegObjData, 2, 4, 1, 1);

    horizontalSpacer = new QSpacerItem(200, 0, QSizePolicy::Preferred, QSizePolicy::Minimum);

    gridLayout->addItem(horizontalSpacer, 1, 2, 1, 1);

    horizontalSpacer_2 = new QSpacerItem(200, 0, QSizePolicy::Preferred, QSizePolicy::Minimum);

    gridLayout->addItem(horizontalSpacer_2, 1, 5, 1, 1);

    imCanvasSpacer = new QSpacerItem(0, 600, QSizePolicy::Minimum, QSizePolicy::Fixed);

    gridLayout->addItem(imCanvasSpacer, 0, 3, 1, 1);


    imViewWidgetLayout->addLayout(gridLayout, 0, 0, 1, 2);


    retranslateUi(IMViewWidgetClass);

    QMetaObject::connectSlotsByName(IMViewWidgetClass);
    } // setupUi

    void retranslateUi(QWidget *IMViewWidgetClass)
    {
    IMViewWidgetClass->setWindowTitle(QApplication::translate("IMViewWidgetClass", "IMViewWidget", 0, QApplication::UnicodeUTF8));
    lblWorldSize->setText(QApplication::translate("IMViewWidgetClass", "World Size", 0, QApplication::UnicodeUTF8));
    lblWorlSizeData->setText(QApplication::translate("IMViewWidgetClass", "<Width * Height>", 0, QApplication::UnicodeUTF8));
    lblGridSize->setText(QApplication::translate("IMViewWidgetClass", "Grid Size", 0, QApplication::UnicodeUTF8));
    lblGridSizeData->setText(QApplication::translate("IMViewWidgetClass", "<Width * Height>", 0, QApplication::UnicodeUTF8));
    lblCellSize->setText(QApplication::translate("IMViewWidgetClass", "Cell Size", 0, QApplication::UnicodeUTF8));
    lblCellSizeData->setText(QApplication::translate("IMViewWidgetClass", "<Width * Height>", 0, QApplication::UnicodeUTF8));
    lblNumRegObj->setText(QApplication::translate("IMViewWidgetClass", "Registered Objects", 0, QApplication::UnicodeUTF8));
    lblNumRegObjData->setText(QApplication::translate("IMViewWidgetClass", "<Count>", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(IMViewWidgetClass);
    } // retranslateUi

};

namespace Ui {
    class IMViewWidgetClass: public Ui_IMViewWidgetClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_IMVIEWWIDGET_H
