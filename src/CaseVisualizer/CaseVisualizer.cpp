#include "CaseVisualizer.h"

#pragma warning(push, 3)
#include <QFileDialog>
#include <QString>
#include <sstream>
#include <cassert>
#include <QDockWidget>
#include <QToolBar>
#include <QButtonGroup>
#include <QToolButton>
#include <QMessageBox>
#include <QApplication>
#include <QStatusBar>
#include <QTemporaryFile>
#pragma warning(pop)

#ifndef CASEBASEEX_H
#include "CaseBaseEx.h"
#endif
#ifndef OBJECTSERIALIZER_H
#include "ObjectSerializer.h"
#endif
#ifndef SERIALIZATIONESSENTIALS_H
#include "SerializationEssentials.h"
#endif
#ifndef GOALFACTORY_H
#include "GoalFactory.h"
#endif
#ifndef TOOLBOX_H
#include "Toolbox.h"
#endif
#ifndef CASEVIEW_H
#include "CaseView.h"
#endif
#ifndef WINGAMEGOAL_H
#include "WinGameGoal.h"
#endif
#ifndef CHOOSEPLANSTEPDIALOG_H
#include "ChoosePlanStepDialog.h"
#endif
#ifndef IDLOOKUPREADER_H
#include "IdLookupReader.h"
#endif
#ifndef GRAPHSCENE_H
#include "GraphScene.h"
#endif
#include "StarCraftGame.h"
#include "Action.h"
#include "GoalEx.h"
#include "ActionFactory.h"
#include "GoalFactory.h"
#include "CaseEx.h"

using namespace std;
using namespace IStrategizer;
using namespace Serialization;

CaseVisualizer::CaseVisualizer(QWidget *parent, Qt::WindowFlags flags)
: QMainWindow(parent, flags), m_pCaseBase(new CaseBaseEx)
{

    ui.setupUi(this);
    CreateToolBox();

    IStrategizer::Init();

    if (InitIdLookup())
    {
        m_pCaseView = new CaseView(&m_idLookup, nullptr);
        m_goalDialog = new ChoosePlanStepDialog(&m_idLookup, true, false, this);

        ui.caseViewLayout->addWidget(m_pCaseView);

        NewCaseBase();
        NewCase(GOALEX_WinGame);
        SelectCase(0);
		Refresh();
    }

    RtsGame* pDummGame = new StarCraftGame;
    delete pDummGame;
}
//----------------------------------------------------------------------------------------------
bool CaseVisualizer::InitIdLookup()
{
    IdLookupReader reader;

    // Read game specific IDs
    if (!reader.ReadEx(GameIdsLookupFilename, m_idLookup))
    {
        QMessageBox::warning(this, tr("Initialization Error"), tr("'%1' file is not found.").arg(GameIdsLookupFilename));
        return false;
    }

    for (unsigned currID = 0; currID < ENUMS_SIZE; ++currID)
    {
        // Read engine specific IDs form Enums to the IdLookup
        if (!BELONG(EntityClassType, currID) &&
            !BELONG(ResearchType, currID))
        {
            if (Enums[currID] != nullptr)
            {
                m_idLookup.SetByFirst(currID, string(Enums[currID]));
            }
        }
        // Read game specific IDs from Idlookup to Enums if such IDs are valid game type ids
        else if (Enums[currID] == nullptr && m_idLookup.ContainsFirst(currID))
        {
            Enums[currID] = _strdup(m_idLookup.GetByFirst(currID).c_str());
        }
    }

    return true;
}
//----------------------------------------------------------------------------------------------
void CaseVisualizer::CreateToolBox()
{
    QToolButton *pointerButton = new QToolButton;
    pointerButton->setCheckable(true);
    pointerButton->setChecked(true);
    pointerButton->setText(QString("Select"));

    QToolButton *linePointerButton = new QToolButton;
    linePointerButton->setCheckable(true);
    linePointerButton->setText(QString("Connect"));

    m_pointerTypeGroup = new QButtonGroup;
    m_pointerTypeGroup->addButton(pointerButton, (int)GraphScene::PTRMODE_Move);
    m_pointerTypeGroup->addButton(linePointerButton, (int)GraphScene::MODE_Connect);
    connect(m_pointerTypeGroup, SIGNAL(buttonClicked(int)), this, SLOT(PointerGroupClicked(int)));

    m_pointerToolbar = addToolBar(tr("Pointer type"));
    m_pointerToolbar->addWidget(pointerButton);
    m_pointerToolbar->addWidget(linePointerButton);
}
//----------------------------------------------------------------------------------------------
void CaseVisualizer::PointerGroupClicked(int)
{
    m_pCaseView->SetMode((GraphScene::PointerMode)m_pointerTypeGroup->checkedId());
}
//----------------------------------------------------------------------------------------------
void CaseVisualizer::on_actionOpen_triggered()
{
    OpenCaseBase(QString());
}
//----------------------------------------------------------------------------------------------
void CaseVisualizer::on_actionSave_triggered()
{
    SaveCaseBase();
}
//----------------------------------------------------------------------------------------------
void CaseVisualizer::on_actionSaveAs_triggered()
{
    SaveCaseBaseAs();
}
//----------------------------------------------------------------------------------------------
void CaseVisualizer::on_actionNew_triggered()
{
    NewCaseBase();
}
//----------------------------------------------------------------------------------------------
void CaseVisualizer::on_actionShow_triggered()
{
}
//----------------------------------------------------------------------------------------------
void CaseVisualizer::on_lstCases_itemDoubleClicked(QListWidgetItem*)
{
    EditCase();
}
//----------------------------------------------------------------------------------------------
void CaseVisualizer::on_btnNewCase_clicked()
{
    NewCase();
	Refresh();
}
//----------------------------------------------------------------------------------------------
void CaseVisualizer::on_btnDeleteCase_clicked()
{
    if(ui.lstCases->currentItem() != NULL)
    {
        int caseIdxInViewLst = ui.lstCases->currentIndex().row();
        if(caseIdxInViewLst >= 0)
        {
            CaseEx* pCase = (CaseEx*)ui.lstCases->item(caseIdxInViewLst)->data(Qt::UserRole).toULongLong();
            DeleteCase(pCase);
            delete ui.lstCases->item(caseIdxInViewLst);
        }
    }
}
//----------------------------------------------------------------------------------------------
void CaseVisualizer::on_btnDuplicateCase_clicked()
{
    if(ui.lstCases->currentItem() != NULL)
    {
        int caseIdxInViewLst = ui.lstCases->currentIndex().row(); 
        if(caseIdxInViewLst >= 0)
        {
            CaseEx* pCase = (CaseEx*)ui.lstCases->item(caseIdxInViewLst)->data(Qt::UserRole).toULongLong();
            DuplicateCase(pCase);
        }
    }
}
//----------------------------------------------------------------------------------------------
void CaseVisualizer::on_lstCases_itemSelectionChanged()
{
    int caseIdx = ui.lstCases->currentIndex().row();
    int numCases = (int)m_pCaseBase->CaseContainer.size();

    if(caseIdx >= 0 &&
        caseIdx < numCases)
    {
        QVariant casePtrData = ui.lstCases->item(caseIdx)->data(Qt::UserRole);
        CaseEx* pSelectedCase = (CaseEx*)casePtrData.toULongLong();
        m_pCaseView->View(pSelectedCase);
    }
    else if (caseIdx == numCases  && numCases > 0)
    {
        m_pCaseView->View(m_pCaseBase->CaseContainer[caseIdx -1]);
    }
    else if (caseIdx == -1 && numCases > 0)
    {
        m_pCaseView->View(m_pCaseBase->CaseContainer[0]);
    }
    else
    {
        m_pCaseView->View(nullptr);
    }
}
//----------------------------------------------------------------------------------------------
void CaseVisualizer::on_btnReloadCB_clicked()
{
    if (!m_caseBasePath.isEmpty())
    {
        OpenCaseBase(m_caseBasePath);
    }
}
//----------------------------------------------------------------------------------------------
void CaseVisualizer::VerifyHashCollisions()
{
    map<unsigned, vector<GoalEx*>> hashes;

    for (auto pCase : m_pCaseBase->CaseContainer)
    {
        auto& r = hashes.operator[](pCase->Goal()->Hash());
        
        // Collision detected!
        if (!r.empty() && !r.back()->Equals(pCase->Goal()))
        {
            QMessageBox::warning(this, tr("Goal Hash Collision"),
                QString("'%1' collides with '%2'").arg(
                QString(r.back()->ToString().c_str()), QString(pCase->Goal()->ToString().c_str())));
        }

        r.push_back(pCase->Goal());
    }
}
//----------------------------------------------------------------------------------------------
void CaseVisualizer::OpenCaseBase(QString cbFilename)
{
    if (cbFilename.isEmpty())
    {
        QDir dir = "../../IStrategizer/Build/Debug";

        if (!dir.exists())
            dir = QDir::currentPath();

        cbFilename = QFileDialog::getOpenFileName(this,
            tr("Open CaseBase"),
            dir.path(),
            CaseBaseFilter);
    }

    if(QFile::exists(cbFilename))
    {
        Toolbox::MemoryClean(m_pCaseBase);

        m_caseBasePath = cbFilename;
        m_pCaseBase = new CaseBaseEx();
        g_ObjectSerializer.Deserialize(m_pCaseBase, string(cbFilename.toLocal8Bit()));
		setWindowTitle(QString("Case Visualizer - %1").arg(m_caseBasePath));

        VerifyHashCollisions();

        Refresh();
        SelectCase(0);
    }
}
//----------------------------------------------------------------------------------------------
void CaseVisualizer::NewCaseBase()
{
    Toolbox::MemoryClean(m_pCaseBase);
    m_pCaseBase = new CaseBaseEx();
    SelectCase(0);
}
//----------------------------------------------------------------------------------------------
void CaseVisualizer::SaveCaseBaseAs()
{
    QDir dir = "../../IStrategizer/Build/Debug";

    if (!dir.exists())
        dir = QDir::currentPath();

    QString fileName = QFileDialog::getSaveFileName(
        this, 
        tr("Open CaseBase"),
        dir.path(),
        CaseBaseFilter);

    if(!fileName.isEmpty())
    {
        g_ObjectSerializer.Serialize(m_pCaseBase, string(fileName.toLocal8Bit()));
        m_caseBasePath = fileName;
		statusBar()->showMessage(QString("Case base saved @ %1").arg(m_caseBasePath), 2000);
		setWindowTitle(QString("Case Visualizer - %1").arg(m_caseBasePath));
    }
}
//----------------------------------------------------------------------------------------------
void CaseVisualizer::SaveCaseBase()
{
    if (m_caseBasePath.isEmpty())
        SaveCaseBaseAs();
    else
    {
        g_ObjectSerializer.Serialize(m_pCaseBase, string(m_caseBasePath.toLocal8Bit()));
		statusBar()->showMessage(QString("Case base saved @ %1").arg(m_caseBasePath), 2000);
    }
}
//----------------------------------------------------------------------------------------------
void CaseVisualizer::Refresh()
{
    stringstream entryStream;
    string caseGoalName;
    GoalEx* pCaseGoal;

    ui.lstCases->clear();

    int i = 0;
    for(SVector<CaseEx*>::iterator caseItr = m_pCaseBase->CaseContainer.begin();
        caseItr != m_pCaseBase->CaseContainer.end();
        ++caseItr)
    {
        entryStream.str("");

        if((*caseItr)->Goal() == NULL)
            entryStream << "UnnamedCase-" << i;
        else
        {
            pCaseGoal = const_cast<GoalEx*>((*caseItr)->Goal());
            assert(pCaseGoal);

            caseGoalName = m_idLookup.GetByFirst(pCaseGoal->StepTypeId());
            entryStream << pCaseGoal->ToString(true);
        }

        QListWidgetItem* pItem = new QListWidgetItem(entryStream.str().c_str());

        QVariant casePtrData((qulonglong)*caseItr);
        pItem->setData(Qt::UserRole, casePtrData);

        ui.lstCases->addItem(pItem);

        ++i;
    }

    ui.lstCases->sortItems(Qt::AscendingOrder);
}
//----------------------------------------------------------------------------------------------
CaseEx* CaseVisualizer::NewCase()
{
    if(m_goalDialog->exec() == QDialog::Accepted)
    {
        GoalEx* newGoal = g_GoalFactory.GetGoal((GoalType)m_goalDialog->SelectedPlanStepId(), false);
        CaseEx* newCase = new CaseEx(new OlcbpPlan, newGoal, nullptr, 1, 1);
        m_pCaseBase->CaseContainer.push_back(newCase);

		return newCase;
    }

	return nullptr;
}
//----------------------------------------------------------------------------------------------
CaseEx* CaseVisualizer::NewCase(GoalType p_caseGoal)
{
    GoalEx* newGoal = g_GoalFactory.GetGoal(p_caseGoal, false);
    CaseEx* newCase = new CaseEx(new OlcbpPlan, newGoal, nullptr, 1, 1);
    m_pCaseBase->CaseContainer.push_back(newCase);

	return newCase;
}
//----------------------------------------------------------------------------------------------
void CaseVisualizer::DeleteCase(CaseEx* pCase)
{
    auto itr = std::find(m_pCaseBase->CaseContainer.begin(), m_pCaseBase->CaseContainer.end(), pCase);
    _ASSERTE(itr != m_pCaseBase->CaseContainer.end());
    m_pCaseBase->CaseContainer.erase(itr);
    Toolbox::MemoryClean(pCase);
}
//----------------------------------------------------------------------------------------------
void CaseVisualizer::DuplicateCase(CaseEx* pCase)
{
    QTemporaryFile temp;
    if (temp.open())
    {
        g_ObjectSerializer.Serialize(pCase, temp.fileName().toStdString());
        CaseEx* pClonedCase = new CaseEx;
        g_ObjectSerializer.Deserialize(pClonedCase, temp.fileName().toStdString());
        pClonedCase->Goal()->Id(GoalEx::GenerateID());
        
        m_pCaseBase->CaseContainer.push_back(pClonedCase);

        Refresh();
    }
}
//----------------------------------------------------------------------------------------------
void CaseVisualizer::EditCase()
{
}
//----------------------------------------------------------------------------------------------
CaseVisualizer::~CaseVisualizer()
{
    Toolbox::MemoryClean(m_pCaseBase);
    Toolbox::MemoryClean(m_pCaseView);
    Toolbox::MemoryClean(m_goalDialog);
}
//----------------------------------------------------------------------------------------------
void CaseVisualizer::SelectCase(int caseIdx)
{
    if (ui.lstCases->count() > 0)
    {
        QListWidgetItem *pFirstCase = ui.lstCases->item(0);
        _ASSERTE(pFirstCase != nullptr);
        pFirstCase->setSelected(true);
    }
    else
    {
        ui.lstCases->selectAll();
    }
}
//////////////////////////////////////////////////////////////////////////
void CaseVisualizer::on_btnGenSCVPlans_clicked()
{
	GenCollectPrimaryResourceCases();
	GenCollectSecondaryResourceCases();
	GenSCVTrainForceCases();
	GenBuildRefineryCases();

	Refresh();
}
//////////////////////////////////////////////////////////////////////////
void CaseVisualizer::GenCollectPrimaryResourceCases()
{
	/*
	Collect(X) = { X == 1 -> S(TrainForce(SCV),Gather(Primary))
	{ X > 1  -> S(Collect(X-1),TrainForce(SCV),Gather(Primary))
	*/
	const unsigned MaxSCVs = 15;
	EntityClassType scvType = (EntityClassType)m_idLookup.GetBySecond("Terran_SCV");

	for (unsigned numSCVs = 1; numSCVs <= MaxSCVs; ++numSCVs)
	{
		auto pCollectResourceCase = NewCase(GOALEX_CollectResource);

		pCollectResourceCase->Goal()->Parameter(PARAM_ResourceId, RESOURCE_Primary);
		pCollectResourceCase->Goal()->Parameter(PARAM_Amount, numSCVs);

		// TrainForce(SCV,Idle,1)
		auto pSCVTrainForceG = g_GoalFactory.GetGoal(GOALEX_TrainForce, false);
		pSCVTrainForceG->Parameter(PARAM_EntityClassId, scvType);
		pSCVTrainForceG->Parameter(PARAM_ObjectStateType, OBJSTATE_Idle);
		pSCVTrainForceG->Parameter(PARAM_Amount, 1);
		pCollectResourceCase->Plan()->AddNode(pSCVTrainForceG, pSCVTrainForceG->Id());

		// GatherResource(Primary,DistanceToBase=25)
		auto pSCVGatherA = g_ActionFactory.GetAction(ACTIONEX_GatherResource, false);
		pSCVGatherA->Parameter(PARAM_ResourceId, RESOURCE_Primary);
		pSCVGatherA->Parameter(PARAM_DistanceToBase, 25);
		pCollectResourceCase->Plan()->AddNode(pSCVGatherA, pSCVGatherA->Id());

		pCollectResourceCase->Plan()->AddEdge(pSCVTrainForceG->Id(), pSCVGatherA->Id());

		if (numSCVs > 1)
		{
			// CollectResource(Primary,numSCVs-1)
			auto pCollectResource = g_GoalFactory.GetGoal(GOALEX_CollectResource, false);
			pCollectResource->Parameter(PARAM_ResourceId, RESOURCE_Primary);
			pCollectResource->Parameter(PARAM_Amount, numSCVs - 1);
			pCollectResourceCase->Plan()->AddNode(pCollectResource, pCollectResource->Id());

			pCollectResourceCase->Plan()->AddEdge(pCollectResource->Id(), pSCVTrainForceG->Id());
		}
	}
}
//////////////////////////////////////////////////////////////////////////
void CaseVisualizer::GenSCVTrainForceCases()
{
	auto pTrainForceCase = NewCase(GOALEX_TrainForce);

	EntityClassType scvType = (EntityClassType)m_idLookup.GetBySecond("Terran_SCV");

	pTrainForceCase->Goal()->Parameter(PARAM_EntityClassId, scvType);
	pTrainForceCase->Goal()->Parameter(PARAM_ObjectStateType, OBJSTATE_Idle);
	pTrainForceCase->Goal()->Parameter(PARAM_Amount, 1);

	auto pTrainSCV = g_ActionFactory.GetAction(ACTIONEX_Train, false);
	pTrainSCV->Parameter(PARAM_EntityClassId, (int)scvType);
	pTrainForceCase->Plan()->AddNode(pTrainSCV, pTrainSCV->Id());
}
//////////////////////////////////////////////////////////////////////////
void CaseVisualizer::GenCollectSecondaryResourceCases()
{
	/*
	Collect(X) = { X == 1 -> S(BuildInfra(Refinery),TrainForce(SCV),Gather(Secondary)) 
				 { X > 1  -> S(Collect(X-1),BuildInfra(Refinery),TrainForce(SCV),Gather(Secondary))
	*/
	const unsigned MaxSCVs = 3;
	EntityClassType scvType = (EntityClassType)m_idLookup.GetBySecond("Terran_SCV");
	EntityClassType refineryType = (EntityClassType)m_idLookup.GetBySecond("Terran_Refinery");

	for (unsigned numSCVs = 1; numSCVs <= MaxSCVs; ++numSCVs)
	{
		auto pCollectResourceCase = NewCase(GOALEX_CollectResource);

		pCollectResourceCase->Goal()->Parameter(PARAM_ResourceId, RESOURCE_Secondary);
		pCollectResourceCase->Goal()->Parameter(PARAM_Amount, numSCVs);

		// BuildInfrastructure(Refinery,1)
		auto pBuildRefineryG = g_GoalFactory.GetGoal(GOALEX_BuildInfrastructure, false);
		pBuildRefineryG->Parameter(PARAM_EntityClassId, refineryType);
		pBuildRefineryG->Parameter(PARAM_Amount, 1);
		pCollectResourceCase->Plan()->AddNode(pBuildRefineryG, pBuildRefineryG->Id());

		// TrainForce(SCV,Idle,1)
		auto pSCVTrainForceG = g_GoalFactory.GetGoal(GOALEX_TrainForce, false);
		pSCVTrainForceG->Parameter(PARAM_EntityClassId, scvType);
		pSCVTrainForceG->Parameter(PARAM_ObjectStateType, OBJSTATE_Idle);
		pSCVTrainForceG->Parameter(PARAM_Amount, 1);
		pCollectResourceCase->Plan()->AddNode(pSCVTrainForceG, pSCVTrainForceG->Id());

		// GatherResource(Secondary,DistanceToBase=25)
		auto pSCVGatherA = g_ActionFactory.GetAction(ACTIONEX_GatherResource, false);
		pSCVGatherA->Parameter(PARAM_ResourceId, RESOURCE_Secondary);
		pSCVGatherA->Parameter(PARAM_DistanceToBase, 25);
		pCollectResourceCase->Plan()->AddNode(pSCVGatherA, pSCVGatherA->Id());

		pCollectResourceCase->Plan()->AddEdge(pBuildRefineryG->Id(), pSCVTrainForceG->Id());
		pCollectResourceCase->Plan()->AddEdge(pSCVTrainForceG->Id(), pSCVGatherA->Id());

		if (numSCVs > 1)
		{
			// CollectResource(Secondary,numSCVs-1)
			auto pCollectResource = g_GoalFactory.GetGoal(GOALEX_CollectResource, false);
			pCollectResource->Parameter(PARAM_ResourceId, RESOURCE_Secondary);
			pCollectResource->Parameter(PARAM_Amount, numSCVs - 1);
			pCollectResourceCase->Plan()->AddNode(pCollectResource, pCollectResource->Id());

			pCollectResourceCase->Plan()->AddEdge(pCollectResource->Id(), pBuildRefineryG->Id());
		}
	}
}
//////////////////////////////////////////////////////////////////////////
void CaseVisualizer::GenBuildRefineryCases()
{
	auto pBuildRefineryCase = NewCase(GOALEX_BuildInfrastructure);

	EntityClassType refineryType = (EntityClassType)m_idLookup.GetBySecond("Terran_Refinery");

	pBuildRefineryCase->Goal()->Parameter(PARAM_EntityClassId, refineryType);
	pBuildRefineryCase->Goal()->Parameter(PARAM_Amount, 1);

	auto pBuildRefineryA = g_ActionFactory.GetAction(ACTIONEX_Build, false);
	pBuildRefineryA->Parameter(PARAM_EntityClassId, (int)refineryType);
	pBuildRefineryA->Parameter(PARAM_DistanceToBase, 25);
	pBuildRefineryCase->Plan()->AddNode(pBuildRefineryA, pBuildRefineryA->Id());
}
