#include "stdafx.h"

#include "PlanStepView.h"

#ifndef PLANSTEPEX_H
#include "PlanStepEx.h"
#endif
#ifndef TOOLBOX_H
#include "Toolbox.h"
#endif
#ifndef PLANGRAPHVIEW_H
#include "PlanGraphView.h"
#endif
#ifndef GRAPHSCENE_H
#include "GraphScene.h"
#endif
#ifndef ACTION_H
#include "Action.h"
#endif
#ifndef PARAMETEREDIT_H
#include "ParameterEdit.h"
#endif
#ifndef ROOTMETADATA_H
#include "RootMetaData.h"
#endif

using namespace std;
using namespace IStrategizer;

const char* NewParamText = "New Param";
const char* NewValText = "New Val";

PlanStepView::PlanStepView(CrossMap<unsigned, string>* p_idLookup, QWidget *parent)
    : QWidget(parent),
    m_paramEditDialog(nullptr)
{
    ui.setupUi(this);
    m_idLookup = p_idLookup;
}
//----------------------------------------------------------------------------------------------
void PlanStepView::on_tblParameters_itemDoubleClicked(QTableWidgetItem* p_item)
{
    EditSelectedParameter();
}
//----------------------------------------------------------------------------------------------
void PlanStepView::ViewPerformance(const PlanStepEx* p_planStep) const
{
    if(p_planStep == NULL)
    {
        ui.txtFailureProbability->setText(tr("0"));
        ui.txtVulnerableCondition->clear();
    }
    else
    {
        if (BELONG(ActionType, p_planStep->StepTypeId()))
        {
            double probability = ((Action*)p_planStep)->GetExecutionHistory().GetFailureProbability();
            ui.txtFailureProbability->setText(QString::number(probability, 'f', 5));

            ui.txtActionTrailCount->setText(QString::number(((Action*)p_planStep)->GetExecutionHistory().TrialCount()));

            ConditionEx* condition = ((Action*)p_planStep)->GetExecutionHistory().GetMostVulnerableAliveCondition();
            const char* conditionName = condition ? condition->ToString().c_str(): "";
            ui.txtVulnerableCondition->setText(QString::fromLocal8Bit(conditionName));
        }
    }
}
//----------------------------------------------------------------------------------------------
void PlanStepView::View(PlanStepEx* p_planStep)
{
    m_planStep = p_planStep;

    ViewPerformance(p_planStep);

    if(m_planStep == NULL)
    {
        ui.lblPlanStep->setText(QString::fromLocal8Bit("Plan Step Name"));
        ui.tblParameters->clear();
    }
    else
    {
        string stepName = m_idLookup->GetByFirst(p_planStep->StepTypeId());
        ui.lblPlanStep->setText(QString::fromLocal8Bit(stepName.c_str()));
        ViewParameters(&p_planStep->Parameters());
        ViewConditions(p_planStep);
    }
}
//----------------------------------------------------------------------------------------------
void PlanStepView::ViewParameters( const PlanStepParameters* p_params )
{
    ui.tblParameters->clear();
    ui.tblParameters->setColumnCount(2);
    ui.tblParameters->setRowCount(p_params->size());

    QTableWidgetItem* cell = NULL;
    int row = 0;

    ui.tblParameters->horizontalHeader()->hide();
    ui.tblParameters->verticalHeader()->hide();

    QFont paramNameCellFont = QFont("Tahoma", 8);
    paramNameCellFont.setBold(true);
    QFont paramValCellFont = QFont("Tahoma", 8);

    QFontMetrics paramNameFontMetric(paramNameCellFont);
    QFontMetrics paramValFontMetric(paramValCellFont);

    int maxParamNameWidth = INT_MIN;
    int maxParamValWidth = INT_MIN;

    for(PlanStepParameters::const_iterator itr = p_params->begin();
        itr != p_params->end();
        ++itr)
    {
        assert(m_idLookup->ContainsFirst(itr->first));
        cell = new QTableWidgetItem(QString::fromLocal8Bit(m_idLookup->GetByFirst(itr->first).c_str()));
        cell->setFont(paramNameCellFont);
        maxParamNameWidth = max(maxParamNameWidth, paramNameFontMetric.width(cell->text()));

        ui.tblParameters->setItem(row, 0, cell);

        if(!m_idLookup->ContainsFirst(itr->second))
        {
            cell = new QTableWidgetItem(tr("%1").arg(itr->second));
        }
        else
        {
            cell = new QTableWidgetItem(QString::fromLocal8Bit(m_idLookup->GetByFirst(itr->second).c_str()));
        }

        cell->setFont(paramValCellFont);
        maxParamValWidth = max(maxParamValWidth, paramValFontMetric.width(cell->text()));

        ui.tblParameters->setItem(row, 1, cell);
        ui.tblParameters->setRowHeight(row, 20);

        ++row;
    }

    ui.tblParameters->setRowHeight(row, 20);
    ui.tblParameters->setColumnWidth(0, maxParamNameWidth + 10);
    ui.tblParameters->setColumnWidth(1, maxParamValWidth + 10);
}

//----------------------------------------------------------------------------------------------
void PlanStepView::EditSelectedParameter()
{
    QList<QTableWidgetItem*> items = ui.tblParameters->selectedItems();
    // param key/value cell selected
    assert(items.size() == 2);

    string keyTxt = items[0]->text().toLocal8Bit();
    string oldValueTxt = items[1]->text().toLocal8Bit();

    ParameterType key = (ParameterType)m_idLookup->GetBySecond(keyTxt);
    ParameterType oldValue = PARAM_END;

    if (m_idLookup->ContainsSecond(oldValueTxt))
    {
        oldValue = (ParameterType)m_idLookup->GetBySecond(oldValueTxt);
    }

    m_paramEditDialog = new ParameterEdit("Param Name", "Param Value", m_idLookup, ui.tblParameters);
    m_paramEditDialog->ParamName(keyTxt);
    m_paramEditDialog->ParamValue(oldValueTxt);

    if(m_paramEditDialog->exec() == QDialog::Accepted)
    {
        string newValueTxt = m_paramEditDialog->ParamValue();
        int newValue = 0;

        if (m_idLookup->ContainsSecond(newValueTxt))
        {
            newValue = m_idLookup->GetBySecond(newValueTxt);
        }
        else
        {
            newValue = QString::fromLocal8Bit(newValueTxt.c_str()).toInt();
        }

        PlanStepParameters& params = m_planStep->Parameters();
        params[key] = newValue;
        this->View(m_planStep);
    }
}
//----------------------------------------------------------------------------------------------
void PlanStepView::ViewConditions(const PlanStepEx* p_planStep)
{
    //if(BELONG(ActionType, p_planStep->StepTypeId()))
    //{
    //    const Action* action = static_cast<const Action*>(p_planStep);
    //}
}
//----------------------------------------------------------------------------------------------
PlanStepView::~PlanStepView()
{
    Toolbox::MemoryClean(m_paramEditDialog);
}
