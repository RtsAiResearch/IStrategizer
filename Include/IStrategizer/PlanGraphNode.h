#ifndef PLANTREENODEEX_H
#define PLANTREENODEEX_H

#include <vector>
#include <set>
#include <queue>
#include <cassert>

namespace IStrategizer
{
    enum PlanGraphNodeType
    {
        PGNTYPE_Goal,
        PGNTYPE_Action,
        PGNTYPE_NULL
    };

    class PlanStepEx;
    class GoalEx;
    class Action;
    class CaseEx;

    class PlanGraphNode
    {
    public:
        typedef std::vector<PlanGraphNode*> List;
        typedef std::set<PlanGraphNode*> Set;
        typedef std::queue<PlanGraphNode*> Queue;
        typedef unsigned NodeID;

        PlanGraphNode(PlanStepEx* p_PlanStep, PlanGraphNode* p_SubPlanGoal);

        inline PlanGraphNodeType Type(){return _type;}
        inline const List& Children(){ return _children; }
        inline const List& Parents(){ return _parents; }
        inline const List& BelongingSubPlanChildren() { return _belongingSubPlanChildren; }
        inline const PlanStepEx* PlanStep() const { return _pPlanStep; }
        inline PlanStepEx* PlanStep() { return _pPlanStep; }
        inline bool IsOpen() { return _isOpen; }
        inline bool IsReady() { return _parents.size() == _readyParents.size(); }
        inline PlanGraphNode* SubPlanGoal() { return _pSubPlanGoal; }
        inline bool IsNull() { return _type == PGNTYPE_NULL; }
        CaseEx * BelongingCase() const { return _pBelongingCase; }
        void BelongingCase(CaseEx *p_Case) { _pBelongingCase = p_Case; }

        GoalEx* GetGoal();
        Action* GetAction();
        void Open();
        void Close();
        void SetChildrenAsBelongingSubPlanChildren();
        void NotifyParentSuccess(PlanGraphNode *p_SucceededParent);
        void CrossUnlinkChildren();
        void CrossLinkChild(PlanGraphNode* p_Child);
        void CrossUnlinkChild(PlanGraphNode* p_Child);
        NodeID Id() const { return _id; }
        static PlanGraphNode* CreatePlanRoot(PlanStepEx *p_PlanStep);
        static PlanGraphNode& Null();
        static void* operator new (size_t p_size); 
        static void operator delete (void *p_Node);

    protected:
        void AddChild(PlanGraphNode *p_Child);
        void AddParent(PlanGraphNode *p_Parent);
        void DeleteChild(PlanGraphNode* p_Child);
        void DeleteParent(PlanGraphNode* p_Parent);

    private:
        PlanStepEx                *_pPlanStep;
        PlanGraphNode            *_pSubPlanGoal;
        PlanGraphNodeType        _type;
        List                    _parents;
        List                    _children;
        List                    _belongingSubPlanChildren;
        Set                        _readyParents;
        bool                    _isOpen;
        CaseEx                    *_pBelongingCase;
        NodeID                  _id;
        static NodeID           s_lastNodeID;
    };
}

#endif 
