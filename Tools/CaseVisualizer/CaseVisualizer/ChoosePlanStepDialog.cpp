#include "ChoosePlanStepDialog.h"

#include <QStringList>
#include <QKeyEvent>
#include <QCompleter> 

#ifndef TOOLBOX_H
#include "Toolbox.h"
#endif
#ifndef METADATA_H
#include "MetaData.h"
#endif
using namespace MetaData;

ChoosePlanStepDialog::ChoosePlanStepDialog(CrossMap<unsigned, string>* p_idLookup, bool p_goals, bool p_actions, QWidget *parent)
: QDialog(parent, Qt::FramelessWindowHint)
{
	ui.setupUi(this);
	m_idLookup = p_idLookup;
	InitializePlanStepList(p_goals, p_actions);
}
//----------------------------------------------------------------------------------------------
void ChoosePlanStepDialog::InitializePlanStepList(bool p_goals, bool p_actions)
{
	QStringList planStepList;
	if(p_goals)
	{
		for(unsigned i = START(GoalType); i < END(GoalType); ++i)
		{
			planStepList << QString::fromStdString(m_idLookup->GetByFirst(i));
		}
	}

	if(p_actions)
	{
		for(unsigned i = START(ActionType); i < END(ActionType); ++i)
		{
			planStepList << QString::fromStdString(m_idLookup->GetByFirst(i));
		}
	}

	m_autoComplete = new QCompleter(planStepList);
	ui.comboBox->setCompleter(m_autoComplete);
	ui.comboBox->setEditable(true);
	ui.comboBox->addItems(planStepList);
}
//----------------------------------------------------------------------------------------------
void ChoosePlanStepDialog::on_comboBox_currentIndexChanged(QString p_item)
{
	m_selectedPlanStepId = m_idLookup->GetBySecond(p_item.toStdString());
	if(BELONG(GoalType, m_selectedPlanStepId))
	{
		ui.lblPlanStepType->setText("Goal");
	}
	else
	{
		ui.lblPlanStepType->setText("Action");
	}
}
//----------------------------------------------------------------------------------------------
void ChoosePlanStepDialog::on_btnOK_clicked()
{
	string selectedItem = ui.comboBox->currentText().toStdString();
	m_selectedPlanStepId = m_idLookup->GetBySecond(selectedItem);

	this->accept();
}
//----------------------------------------------------------------------------------------------
void ChoosePlanStepDialog::on_btnCancel_clicked()
{
	this->reject();
}
//----------------------------------------------------------------------------------------------
ChoosePlanStepDialog::~ChoosePlanStepDialog()
{
	Toolbox::MemoryClean(m_autoComplete);
}
