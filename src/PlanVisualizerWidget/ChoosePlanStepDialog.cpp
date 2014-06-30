#include "stdafx.h"
#include "ChoosePlanStepDialog.h"

#ifndef TOOLBOX_H
#include "Toolbox.h"
#endif
#ifndef METADATA_H
#include "MetaData.h"
#endif
using namespace IStrategizer;

ChoosePlanStepDialog::ChoosePlanStepDialog(CrossMap<unsigned, string>* p_idLookup, bool p_chooseGoals, bool p_chooseActions, QWidget *parent)
: QDialog(parent, Qt::FramelessWindowHint),
m_chooseGoals(p_chooseGoals),
m_chooseActions(p_chooseActions),
m_autoComplete(nullptr),
m_initialized(false)
{
    ui.setupUi(this);
    m_idLookup = p_idLookup;
}
//----------------------------------------------------------------------------------------------
int ChoosePlanStepDialog::exec()
{
    if (!m_initialized)
    {
        InitializePlanStepList(m_chooseGoals, m_chooseActions);
        m_initialized = true;
    }

    return QDialog::exec();
}
//----------------------------------------------------------------------------------------------
void ChoosePlanStepDialog::InitializePlanStepList(bool p_goals, bool p_actions)
{
    QStringList planStepList;
    if(p_goals)
    {
        for(unsigned i = START(GoalType); i < END(GoalType); ++i)
        {
            string goalName;
            
            if (m_idLookup->TryGetByFirst(i, goalName))
            {
                planStepList << QString::fromLocal8Bit(goalName.c_str());
            }
            else
            {
                string errorMsg = "Key '";
                errorMsg += std::to_string((_ULonglong)i);
                errorMsg += "' not found";

                QMessageBox::warning(this, tr("Initialization Error"), tr("Key '%1' not found in the ID lookup file").arg(i));
            }
        }
    }

    if(p_actions)
    {
        for(unsigned i = START(ActionType); i < END(ActionType); ++i)
        {
            string actionName;
            
            if (m_idLookup->TryGetByFirst(i, actionName))
            {
                planStepList << QString::fromLocal8Bit(actionName.c_str());
            }
            else
            {
                QMessageBox::warning(this, tr("Initialization Error"), tr("Key '%1' not found in the ID lookup file").arg(i));
            }
        }
    }

    m_autoComplete = new QCompleter(planStepList, this);
    ui.comboBox->setCompleter(m_autoComplete);
    ui.comboBox->setEditable(true);
    ui.comboBox->addItems(planStepList);
}
//----------------------------------------------------------------------------------------------
void ChoosePlanStepDialog::on_comboBox_currentIndexChanged(QString p_item)
{
    std::string sItem = p_item.toLocal8Bit();

    if (m_idLookup->TryGetBySecond(sItem, m_selectedPlanStepId))
    {
        if(BELONG(GoalType, m_selectedPlanStepId))
        {
            ui.lblPlanStepType->setText("Goal");
        }
        else
        {
            ui.lblPlanStepType->setText("Action");
        }
    }
    else
    {
        QMessageBox::warning(this, tr("Initialization Error"), tr("Key '%1' not found in the ID lookup file").arg(sItem.c_str()));
    }
}
//----------------------------------------------------------------------------------------------
void ChoosePlanStepDialog::on_btnOK_clicked()
{
    string selectedItem = ui.comboBox->currentText().toLocal8Bit();

    if (m_idLookup->ContainsSecond(selectedItem))
        m_selectedPlanStepId = m_idLookup->GetBySecond(selectedItem);

    this->accept();
}
//----------------------------------------------------------------------------------------------
void ChoosePlanStepDialog::on_btnCancel_clicked()
{
    this->reject();
}
