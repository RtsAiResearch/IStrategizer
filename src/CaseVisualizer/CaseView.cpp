#include "CaseView.h"

#ifndef CASEEX_H
#include "CaseEx.h"
#endif
#ifndef ENGINEDATA_H
#include "EngineData.h"
#endif
#ifndef TOOLBOX_H
#include "Toolbox.h"
#endif

#pragma warning(push, 3)
#include <QtWidgets>
#include <QTableWidget>
#include <fstream>
#include <cassert>
#include <string>
#include <QHeaderView>
#include <sstream>
#include <QLayout>
#include <QScrollBar>
#include <QGridLayout>
#include <QFont>
#include <QFontMetrics>
#include <QGraphicsView>
#pragma warning(pop)

#include "PlanGraphView.h"
#include "GraphScene.h"
#include "GoalEx.h"
#include "ParameterEdit.h"
#include "WorldMap.h"

using namespace std;
using namespace IStrategizer;

CaseView::CaseView(CrossMap<unsigned, string>* p_idLookup, QWidget *parent)
    : QWidget(parent)
{
	m_idLookup		= p_idLookup;
	m_currentCase	= NULL;

    ui.setupUi(this);
	CreatePlanView();
}
//----------------------------------------------------------------------------------------------
void CaseView::SetMode(GraphScene::PointerMode p_mode)
{
	 m_graphView->SetMode(p_mode);
}
//----------------------------------------------------------------------------------------------
void CaseView::OnCellChanged(int p_row, int p_column)
{
	if(p_column == 0)
		return;

	stringstream stream;
	float val;
	QTableWidgetItem* item = ui.tblGameState->item(p_row, p_column);
	stream.str(string(item->text().toLocal8Bit()));

	stream >> val;
	ShallowFeaturesEx &sfeatures = ShallowFeaturesEx(); //m_currentCase->GameState()->ShallowFeatures();
	sfeatures[p_row] = val;
}
//----------------------------------------------------------------------------------------------
void CaseView::CreatePlanView()
{
	GraphScene* pGraphScene = new GraphScene(m_idLookup);
	m_graphView = new PlanGraphView(pGraphScene, m_idLookup);

    ui.planViewLayout->addWidget(m_graphView);
}
//----------------------------------------------------------------------------------------------
void CaseView::View(CaseEx* p_case)
{
	m_currentCase = p_case;

	if(p_case == NULL)
	{
		ViewCaseGoal(NULL);
		ViewGameState(NULL);
		ViewPlanGraph(NULL, NULL);
		ViewPerformance(NULL);
	}
	else
	{
		ViewCaseGoal(p_case);
		ViewGameState(p_case->GameState());
		ViewPlanGraph(p_case->Goal(), p_case->Plan());
		ViewPerformance(p_case);
	}
}
//----------------------------------------------------------------------------------------------
void CaseView::ViewCaseGoal(CaseEx* pCase)
{
	if(pCase == NULL)
	{
		ui.lblGoalName->setText(QString::fromLocal8Bit("<Case Goal Name>"));
		ViewGoalParameters(NULL);
        ui.txtCaseName->setText("<Case Name>");
	}
	else
	{
        GoalEx* p_goal = pCase->Goal();
		string goalName = m_idLookup->GetByFirst(p_goal->StepTypeId());
		ui.lblGoalName->setText(QString::fromLocal8Bit(goalName.c_str()));
		ViewGoalParameters(&p_goal->Parameters());
        ui.txtCaseName->setText(pCase->Name().c_str());
	}
}
//----------------------------------------------------------------------------------------------
void CaseView::ViewGoalParameters( PlanStepParameters* p_params )
{
	ui.tblParameters->clear();

	if(p_params == NULL)
		return;

	ui.tblParameters->setColumnCount(2);
	ui.tblParameters->setRowCount(p_params->size());

	int columnWidth = (ui.tblParameters->width() / 2) - 10;
	ui.tblParameters->setColumnWidth(0, columnWidth);
	ui.tblParameters->setColumnWidth(1, columnWidth);
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

	ui.tblParameters->setColumnWidth(0, maxParamNameWidth + 10);
	ui.tblParameters->setColumnWidth(1, maxParamValWidth + 10);
}
//----------------------------------------------------------------------------------------------
void CaseView::ViewGameState(RtsGame* p_gameState)
{
    ui.tblGameState->clear();

	if(p_gameState == nullptr)
		return;

    p_gameState->Init();
    ui.tblGameState->setColumnCount(2);
    ui.tblGameState->setRowCount(COUNT(RtsGameModelAttribute));

    //ui.tblGameState->setColumnWidth(0, (int)(ui.tblGameState->width() * (3.0f / 4.0f)) - 10);
    //ui.tblGameState->setColumnWidth(1, (int)(ui.tblGameState->width() * (1.0f / 4.0f)) - 10);

    QTableWidgetItem* cell = NULL;
    RtsGameModel model(*p_gameState);
    int row = 0;

    ui.tblGameState->horizontalHeader()->hide();
    ui.tblGameState->verticalHeader()->hide();

    for(RtsGameModelAttribute attr = START(RtsGameModelAttribute);
        attr != END(RtsGameModelAttribute);
        attr = RtsGameModelAttribute(int(attr) + 1))
    {
        cell = new QTableWidgetItem(QString::fromLocal8Bit(m_idLookup->GetByFirst(attr).c_str()));
		cell->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        ui.tblGameState->setItem(row, 0, cell);

        auto attrVal = model.Attr(attr);

        if (attr == RTSMODATTR_MapArea)
        {
            char buff[32];
            sprintf_s(buff, "%dx%d", TileCoordFromUnitCoord(p_gameState->Map()->Width()),
                TileCoordFromUnitCoord(p_gameState->Map()->Height()));
            cell = new QTableWidgetItem(QString::fromLocal8Bit(buff));
        }
        else
        {
            cell = new QTableWidgetItem(tr("%1").arg(attrVal));
        }

        cell->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        ui.tblGameState->setItem(row, 1, cell);

        ui.tblGameState->setRowHeight(row, 20);
        ++row;
    }

    ui.tblGameState->resizeColumnsToContents();
}
//----------------------------------------------------------------------------------------------
void CaseView::ViewPlanGraph(GoalEx* p_caseGoal, IOlcbpPlan* p_planGraph)
{
    m_graphView->View(p_planGraph);
    m_graphView->NotifyGraphStructureChange(p_planGraph);
}
//----------------------------------------------------------------------------------------------
void CaseView::ViewPerformance(CaseEx *p_pCase)
{
	if (p_pCase)
	{
		ui.txtSuccessCount->setText(tr("%1").arg(p_pCase->SuccessCount()));
		ui.txtTrialCount->setText(tr("%1").arg(p_pCase->TrialCount()));
	}
	else
	{
		ui.txtSuccessCount->setText(tr("0"));
		ui.txtTrialCount->setText(tr("0"));
	}	
}
//----------------------------------------------------------------------------------------------
void CaseView::on_txtSuccessCount_textChanged(const QString &p_newText)
{
    if (m_currentCase != nullptr)
    {
	    int newSuccessCount = p_newText.toInt();
	    m_currentCase->SuccessCount(newSuccessCount);
    }
}
//----------------------------------------------------------------------------------------------
void CaseView::on_txtTrialCount_textChanged(const QString &p_newText)
{
    if (m_currentCase != nullptr)
    {
	    int newTrialCount = p_newText.toInt();
	    m_currentCase->TrialCount(newTrialCount);
    }
}
//----------------------------------------------------------------------------------------------
CaseView::~CaseView()
{

}
//----------------------------------------------------------------------------------------------
void CaseView::on_tblParameters_itemDoubleClicked(QTableWidgetItem* p_item)
{
    EditSelectedParameter();
}
//----------------------------------------------------------------------------------------------
void CaseView::EditSelectedParameter()
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

        PlanStepParameters& params = m_currentCase->Goal()->Parameters();
        params[key] = newValue;
        this->View(m_currentCase);
    }
}
//////////////////////////////////////////////////////////////////////////
void CaseView::on_txtCaseName_editingFinished()
{
    m_currentCase->Name(string(ui.txtCaseName->text().toLocal8Bit()));
}