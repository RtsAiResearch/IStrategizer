#ifndef PLANTREENODEEX_H
#define PLANTREENODEEX_H

#include <vector>
#include <set>
#include <queue>
#include <cassert>

namespace IStrategizer
{
    enum PlanTreeNodeType
    {
        PTNTYPE_Goal,
        PTNTYPE_Action,
        PTNTYPE_nullptr
    };

    class PlanStepEx;
    class GoalEx;
    class Action;
    class CaseEx;

    class PlanTreeNodeEx
    {
    public:
        typedef std::vector<PlanTreeNodeEx*> List;
        typedef std::set<PlanTreeNodeEx*> Set;
        typedef std::queue<PlanTreeNodeEx*> Queue;

        PlanTreeNodeEx(PlanStepEx* p_pPlanStep, PlanTreeNodeEx* p_pSubPlanGoal);

        inline PlanTreeNodeType Type(){return _type;}
        inline const List& Children(){ return _children; }
        inline const List& Parents(){ return _parents; }
        inline const List& BelongingSubPlanChildren() { return _belongingSubPlanChildren; }
        inline const PlanStepEx* PlanStep() const { return _pPlanStep; }
        inline PlanStepEx* PlanStep() { return _pPlanStep; }
        inline bool IsOpen() { return _isOpen; }
        inline bool IsReady() { return _parents.size() == _readyParents.size(); }
        inline PlanTreeNodeEx* SubPlanGoal() { return _pSubPlanGoal; }
        inline bool IsNull() { return _type == PTNTYPE_nullptr; }
        CaseEx * BelongingCase() const { return _pBelongingCase; }
        void BelongingCase(CaseEx *p_pCase) { _pBelongingCase = p_pCase; }

        GoalEx* GetGoal();
        Action* GetAction();
        void Open();
        void Close();
        void SetChildrenAsBelongingSubPlanChildren();
        void NotifyParentSuccess(PlanTreeNodeEx *p_pSuccededParent);
        void CrossUnlinkChildren();
        void CrossLinkChild(PlanTreeNodeEx* p_pChild);
        void CrossUnlinkChild(PlanTreeNodeEx* p_pChild);
        static PlanTreeNodeEx* CreatePlanRoot(PlanStepEx *p_pPlanStep);
        static PlanTreeNodeEx& Null();
        static void* operator new (size_t p_size); 
        static void operator delete (void *p_pNode);

    protected:
        void AddChild(PlanTreeNodeEx *p_pChild);
        void AddParent(PlanTreeNodeEx *p_pParent);
        void DeleteChild(PlanTreeNodeEx* p_pChild);
        void DeleteParent(PlanTreeNodeEx* p_pParent);

    private:
        PlanStepEx *_pPlanStep;
        PlanTreeNodeEx *_pSubPlanGoal;
        PlanTreeNodeType _type;
        List _parents;
        List _children;
        List _belongingSubPlanChildren;
        Set _readyParents;
        bool _isOpen;
        CaseEx *_pBelongingCase;
    };
}

#endif 
