#ifndef PLANTREENODEEX_H
#include "PlanGraphNode.h"
#endif
#ifndef PLANSTEPEX_H
#include "PlanStepEx.h"
#endif
#ifndef GOALEX_H
#include "GoalEx.h"
#endif
#ifndef ACTION_H
#include "Action.h"
#endif
#ifndef ENGINEDATA_H
#include "EngineData.h"
#endif
#include "Logger.h"
#include <algorithm>
#include <cassert>
#include <Windows.h>

using namespace IStrategizer;

PlanGraphNode::NodeID PlanGraphNode::s_lastNodeID = 0;

//////////////////////////////////////////////////////////////////////////
void* PlanGraphNode::operator new (size_t p_size)
{
    void* pNode = HeapAlloc(GetProcessHeap(), 0, p_size);
    return pNode;
}
//////////////////////////////////////////////////////////////////////////
void PlanGraphNode::operator delete (void *p_Node)
{
    HeapFree(GetProcessHeap(), 0, p_Node);
}
//////////////////////////////////////////////////////////////////////////
PlanGraphNode::PlanGraphNode(PlanStepEx* p_PlanStep, PlanGraphNode* p_SubPlanGoal) :
_pPlanStep(p_PlanStep), _pSubPlanGoal(p_SubPlanGoal), _isOpen(true)
{
    _id = ++s_lastNodeID;

    if (p_PlanStep)
    {
        if(BELONG(GoalType, p_PlanStep->StepTypeId()))
        {
            _type = PGNTYPE_Goal;
        }
        else if(BELONG(ActionType, p_PlanStep->StepTypeId()))
        {
            _type = PGNTYPE_Action;
        }
    }
    else
    {
        _type = PGNTYPE_NULL;
    }
}
//////////////////////////////////////////////////////////////////////////
PlanGraphNode& PlanGraphNode::Null()
{
    static PlanGraphNode sentinelNode(nullptr, nullptr);
    return sentinelNode;
}
//////////////////////////////////////////////////////////////////////////
PlanGraphNode* PlanGraphNode::CreatePlanRoot(PlanStepEx *p_PlanStep)
{
    PlanGraphNode *pPlanRoot = nullptr;

    // Initialize plan root
    pPlanRoot = new PlanGraphNode(p_PlanStep, &PlanGraphNode::Null());
    pPlanRoot->SetChildrenAsBelongingSubPlanChildren();
    pPlanRoot->AddParent(&PlanGraphNode::Null());
    pPlanRoot->NotifyParentSuccess(&PlanGraphNode::Null());

    return pPlanRoot;
}
//////////////////////////////////////////////////////////////////////////
GoalEx* PlanGraphNode::GetGoal()
{
    assert(BELONG(GoalType, _pPlanStep->StepTypeId()));
    return static_cast<GoalEx*> (_pPlanStep);
}
//////////////////////////////////////////////////////////////////////////
Action* PlanGraphNode::GetAction()
{
    assert(BELONG(ActionType, _pPlanStep->StepTypeId()));
    return static_cast<Action*> (_pPlanStep);
}
//////////////////////////////////////////////////////////////////////////
void PlanGraphNode::NotifyParentSuccess(PlanGraphNode *p_SucceededParent)
{
    _readyParents.insert(p_SucceededParent);
}
//////////////////////////////////////////////////////////////////////////
void PlanGraphNode::Open() 
{
    assert(_isOpen == false);
    _isOpen = true;
    LogInfo("'%s' is opened", _pPlanStep->ToString().c_str());
}
//////////////////////////////////////////////////////////////////////////
void PlanGraphNode::Close()
{
    assert(_isOpen == true);
    _isOpen = false; 
    LogInfo("'%s' is closed", _pPlanStep->ToString().c_str());
}
//////////////////////////////////////////////////////////////////////////
void PlanGraphNode::AddChild(PlanGraphNode *p_Child)
{
    _children.push_back(p_Child);
}
//////////////////////////////////////////////////////////////////////////
void PlanGraphNode::AddParent(PlanGraphNode *p_Parent)
{
    _parents.push_back(p_Parent);
}
//////////////////////////////////////////////////////////////////////////
void PlanGraphNode::CrossUnlinkChildren()
{
    List tempChildren(_children);

    for (List::iterator itr = tempChildren.begin();
        itr != tempChildren.end(); ++itr)
    {
        CrossUnlinkChild(*itr);
    }

    _children.clear();
}
//////////////////////////////////////////////////////////////////////////
void PlanGraphNode::DeleteChild(PlanGraphNode* p_Child)
{
    List::iterator itrWhere = find(_children.begin(), _children.end(), p_Child);
    assert(itrWhere != _children.end());

    _children.erase(itrWhere);
}
//////////////////////////////////////////////////////////////////////////
void PlanGraphNode::DeleteParent(PlanGraphNode* p_Parent)
{
    List::iterator itrWhere = find(_parents.begin(), _parents.end(), p_Parent);
    assert(itrWhere != _parents.end());

    _parents.erase(itrWhere);

    if (_readyParents.find(p_Parent) != _readyParents.end())
        _readyParents.erase(p_Parent);
}
//////////////////////////////////////////////////////////////////////////
void PlanGraphNode::CrossLinkChild(PlanGraphNode* p_Child)
{
    AddChild(p_Child);
    p_Child->AddParent(this);
}
//////////////////////////////////////////////////////////////////////////
void PlanGraphNode::CrossUnlinkChild(PlanGraphNode* p_Child)
{
    DeleteChild(p_Child);
    p_Child->DeleteParent(this);
}
//////////////////////////////////////////////////////////////////////////
void PlanGraphNode::SetChildrenAsBelongingSubPlanChildren()
{
    _belongingSubPlanChildren.clear();
    _belongingSubPlanChildren.insert(_belongingSubPlanChildren.begin(), _children.begin(), _children.end());
}
