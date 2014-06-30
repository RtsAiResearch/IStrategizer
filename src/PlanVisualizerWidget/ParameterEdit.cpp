#include "stdafx.h"
#include "ParameterEdit.h"
#ifndef TOOLBOX_H
#include "Toolbox.h"
#endif

using namespace IStrategizer;

ParameterEdit::ParameterEdit(string p_paramName, string p_paramValue, CrossMap<unsigned, string>* p_idLookup, QWidget *parent)
: QDialog(parent, Qt::FramelessWindowHint),
m_autoComplete(nullptr),
m_initialized(false)
{
    ui.setupUi(this);

    m_paramName = p_paramName;
    m_paramValue = p_paramValue;
    m_idLookup = p_idLookup;
}
//----------------------------------------------------------------------------------------------
void ParameterEdit::InitializeAutoComplete()
{
    vector<string> stdStringValues;
    m_idLookup->SecondValues(stdStringValues);

    QStringList qtStringParamValues;
    for(int i = 0, size = stdStringValues.size(); i < size; ++i)
    {
        qtStringParamValues << QString::fromLocal8Bit(stdStringValues[i].c_str());
    }

    m_autoComplete = new QCompleter(qtStringParamValues, this);
    m_autoComplete->setCompletionMode(QCompleter::PopupCompletion);
    m_autoComplete->setCaseSensitivity(Qt::CaseInsensitive);
    ui.lineEditParamValue->setCompleter(m_autoComplete);
}
//----------------------------------------------------------------------------------------------
int ParameterEdit::exec()
{
    if (!m_initialized)
    {
        InitializeAutoComplete();
        ui.lblActualParamName->setText(QString::fromLocal8Bit(m_paramName.c_str()));
        ui.lineEditParamValue->setText(QString::fromLocal8Bit(m_paramValue.c_str()));

        m_initialized = true;
    }


    return QDialog::exec();
}
//----------------------------------------------------------------------------------------------
void ParameterEdit::on_btnOK_clicked()
{
    m_paramValue = ui.lineEditParamValue->text().toLocal8Bit();

    this->accept();
}
//----------------------------------------------------------------------------------------------
void ParameterEdit::on_btnCancel_clicked()
{
    this->reject();
}
