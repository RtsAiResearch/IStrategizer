/********************************************************************************
** Form generated from reading ui file 'ParameterEdit.ui'
**
** Created: Tue Mar 20 01:13:00 2012
**      by: Qt User Interface Compiler version 4.4.2
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_PARAMETEREDIT_H
#define UI_PARAMETEREDIT_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_ParameterEditClass
{
public:
    QComboBox *lineEditParamValue;
    QLabel *lblActualParamName;
    QLabel *lblParamValue;
    QLabel *lblParamName;
    QPushButton *btnOK;
    QPushButton *btnCancel;

    void setupUi(QDialog *ParameterEditClass)
    {
    if (ParameterEditClass->objectName().isEmpty())
        ParameterEditClass->setObjectName(QString::fromUtf8("ParameterEditClass"));
    ParameterEditClass->resize(170, 130);
    lineEditParamValue = new QComboBox(ParameterEditClass);
    lineEditParamValue->setObjectName(QString::fromUtf8("lineEditParamValue"));
    lineEditParamValue->setGeometry(QRect(10, 70, 151, 22));
    lblActualParamName = new QLabel(ParameterEditClass);
    lblActualParamName->setObjectName(QString::fromUtf8("lblActualParamName"));
    lblActualParamName->setGeometry(QRect(5, 27, 160, 16));
    lblActualParamName->setFrameShape(QFrame::StyledPanel);
    lblParamValue = new QLabel(ParameterEditClass);
    lblParamValue->setObjectName(QString::fromUtf8("lblParamValue"));
    lblParamValue->setGeometry(QRect(5, 49, 160, 16));
    QFont font;
    font.setFamily(QString::fromUtf8("Tahoma"));
    font.setPointSize(8);
    font.setBold(true);
    font.setItalic(false);
    font.setUnderline(false);
    font.setWeight(75);
    font.setStrikeOut(false);
    lblParamValue->setFont(font);
    lblParamName = new QLabel(ParameterEditClass);
    lblParamName->setObjectName(QString::fromUtf8("lblParamName"));
    lblParamName->setGeometry(QRect(5, 5, 160, 16));
    lblParamName->setFont(font);
    btnOK = new QPushButton(ParameterEditClass);
    btnOK->setObjectName(QString::fromUtf8("btnOK"));
    btnOK->setGeometry(QRect(10, 100, 71, 25));
    QFont font1;
    font1.setFamily(QString::fromUtf8("MS Shell Dlg 2"));
    font1.setPointSize(8);
    font1.setBold(true);
    font1.setItalic(false);
    font1.setUnderline(false);
    font1.setWeight(75);
    font1.setStrikeOut(false);
    btnOK->setFont(font1);
    btnCancel = new QPushButton(ParameterEditClass);
    btnCancel->setObjectName(QString::fromUtf8("btnCancel"));
    btnCancel->setGeometry(QRect(90, 100, 71, 25));
    btnCancel->setFont(font1);

    retranslateUi(ParameterEditClass);

    QMetaObject::connectSlotsByName(ParameterEditClass);
    } // setupUi

    void retranslateUi(QDialog *ParameterEditClass)
    {
    ParameterEditClass->setWindowTitle(QApplication::translate("ParameterEditClass", "ParameterEdit", 0, QApplication::UnicodeUTF8));
    lblActualParamName->setText(QApplication::translate("ParameterEditClass", "Param Name", 0, QApplication::UnicodeUTF8));
    lblParamValue->setText(QApplication::translate("ParameterEditClass", "Param Value", 0, QApplication::UnicodeUTF8));
    lblParamName->setText(QApplication::translate("ParameterEditClass", "Param Name", 0, QApplication::UnicodeUTF8));
    btnOK->setText(QApplication::translate("ParameterEditClass", "OK", 0, QApplication::UnicodeUTF8));
    btnCancel->setText(QApplication::translate("ParameterEditClass", "Cancel", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(ParameterEditClass);
    } // retranslateUi

};

namespace Ui {
    class ParameterEditClass: public Ui_ParameterEditClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PARAMETEREDIT_H
