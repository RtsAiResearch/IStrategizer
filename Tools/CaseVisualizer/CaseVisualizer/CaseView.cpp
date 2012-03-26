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

#ifndef GAMESTATEEX_H
#include "GameStateEx.h"
#endif

using namespace MetaData;

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
#include "PlanGraphView.h"
#include "GraphScene.h"

using namespace std;

CaseView::CaseView(CrossMap<unsigned, string>* p_idLookup, QWidget *parent)
    : QWidget(parent)
{
	m_idLookup		= p_idLookup;
	m_currentCase	= NULL;

    ui.setupUi(this);
	CreatePlanView();
}
//----------------------------------------------------------------------------------------------
void CaseView::SetMode(int p_mode)
{
	 m_graphView->SetMode(p_mode);
}
//----------------------------------------------------------------------------------------------
void CaseView::OnCellChanged(int p_row, int p_column)
{
	if(p_column == 0)
		return;

	stringstream stream;
	double val;
	QTableWidgetItem* item = ui.tblGameState->item(p_row, p_column);
	stream.str(item->text().toStdString());

	stream >> val;
	ShallowFeaturesEx &sfeatures = m_currentCase->GameState()->ShallowFeatures();
	sfeatures[p_row] = val;
}
//----------------------------------------------------------------------------------------------
void CaseView::CreatePlanView()
{
	m_graphScene = new GraphScene(m_idLookup);
	m_graphView = new PlanGraphView(m_graphScene, m_idLookup);

	QLayout* frmLayout = new QGridLayout(ui.frmPlanView);
	frmLayout->setMargin(0);
	frmLayout->addWidget(m_graphView);
	ui.frmPlanView->setLayout(frmLayout);
}
//----------------------------------------------------------------------------------------------
void CaseView::View(CaseEx* p_case)
{
	m_currentCase = p_case;

	if(p_case == NULL)
	{
		ViewGoal(NULL);
		ViewGameState(NULL);
		ViewPlanGraph(NULL, NULL);
	}
	else
	{
		ViewGoal(p_case->Goal());
		ViewGameState(p_case->GameState());
		ViewPlanGraph(p_case->Goal(), p_case->GetPlanGraph());
	}
}
//----------------------------------------------------------------------------------------------
void CaseView::ViewGoal(GoalEx* p_goal)
{
	if(p_goal == NULL)
	{
		ui.lblGoalName->setText(QString::fromStdString("Goal Name"));
		ViewGoalParameters(NULL);
	}
	else
	{
		string goalName = m_idLookup->GetByFirst(p_goal->StepTypeId());
		ui.lblGoalName->setText(QString::fromStdString(goalName));
		ViewGoalParameters(&p_goal->Parameters());
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
	ui.tblParameters->setSelectionMode(QAbstractItemView::NoSelection);
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
		cell = new QTableWidgetItem(QString::fromStdString(m_idLookup->GetByFirst(itr->first)));
		cell->setFont(paramNameCellFont);
		maxParamNameWidth = max(maxParamNameWidth, paramNameFontMetric.width(cell->text()));

		ui.tblParameters->setItem(row, 0, cell);

		if(!m_idLookup->ContainsFirst(itr->second))
		{
			cell = new QTableWidgetItem(tr("%1").arg(itr->second));
		}
		else
		{
			cell = new QTableWidgetItem(QString::fromStdString(m_idLookup->GetByFirst(itr->second)));
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
void CaseView::ViewGameState(GameStateEx* p_gameState)
{
    ui.tblGameState->clear();

	if(p_gameState == NULL)
		return;

	disconnect(ui.tblGameState, SIGNAL(cellChanged(int,int)), this, SLOT(OnCellChanged(int, int)));

	const ShallowFeaturesEx& sfeatures = p_gameState->ShallowFeatures();

    ui.tblGameState->setColumnCount(2);
    ui.tblGameState->setRowCount(sfeatures.size());

    int columnWidth = ui.tblGameState->width() / 2;
    ui.tblGameState->setColumnWidth(0, ui.tblGameState->width() * 3.0 / 4.0 - 10);
    ui.tblGameState->setColumnWidth(1, ui.tblGameState->width() * 1.0 / 4.0 - 10);
    ui.tblGameState->setSelectionMode(QAbstractItemView::NoSelection);
    QTableWidgetItem* cell = NULL;
    int row = 0;

    ui.tblGameState->horizontalHeader()->hide();
    ui.tblGameState->verticalHeader()->hide();

    for(int i = 0, size = sfeatures.size(); i < size; ++i)
    {
        cell = new QTableWidgetItem(QString::fromStdString(m_idLookup->GetByFirst(GET(i, ShallowFeatureType))));
		cell->setFlags(Qt::NoItemFlags);
        ui.tblGameState->setItem(row, 0, cell);

        cell = new QTableWidgetItem(tr("%1").arg(sfeatures[i]));
        ui.tblGameState->setItem(row, 1, cell);

        ui.tblGameState->setRowHeight(row, 20);
        ++row;
    }

	connect(ui.tblGameState, SIGNAL(cellChanged(int,int)), SLOT(OnCellChanged(int, int)));
}
//----------------------------------------------------------------------------------------------
void CaseView::ViewPlanGraph(GoalEx* p_caseGoal, PlanGraph* p_planGraph)
{
	if(p_caseGoal == NULL || p_planGraph == NULL)
	{
		m_graphScene->View(NULL, NULL);
	}
	else
	{
		m_graphScene->View(p_caseGoal, p_planGraph);
	}
}

//----------------------------------------------------------------------------------------------
CaseView::~CaseView()
{

}