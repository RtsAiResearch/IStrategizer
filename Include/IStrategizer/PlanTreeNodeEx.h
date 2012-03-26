#ifndef PLANTREENODEEX_H
#define PLANTREENODEEX_H

#include <vector>
#include <set>
#include <cassert>

class PlanStepEx;
class GoalEx;
class Action;

namespace OLCBP
{
	enum PlanTreeNodeType
	{
		PTNTYPE_Goal,
		PTNTYPE_Action,
		PTNTYPE_Null
	};

	class PlanTreeNodeEx
	{
	public:
		typedef std::vector<PlanTreeNodeEx*> NodeList;
		typedef std::set<PlanTreeNodeEx*> NodeSet;
		PlanTreeNodeEx(PlanStepEx* p_pPlanStep, PlanTreeNodeEx* p_pSubPlanGoal);

		inline PlanTreeNodeType Type(){return _type;}
		inline const NodeList& Children(){ return _children; }
		inline const NodeList& Parents(){ return _parents; }
		inline const NodeList& BelongingSubPlanChildren() { return _belongingSubPlanChildren; }
		inline PlanStepEx* PlanStep(){ return _pPlanStep; }
		inline bool IsOpen() { return _isOpen; }
		inline bool IsReady() { return _parents.size() == _readyParents.size(); }
		inline PlanTreeNodeEx* SubPlanGoal() { return _pSubPlanGoal; }
		inline bool IsNull() { return _type == PTNTYPE_Null; }
	
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
		PlanStepEx*				_pPlanStep;
		PlanTreeNodeEx*			_pSubPlanGoal;
		PlanTreeNodeType		_type;
		NodeList				_parents;
		NodeList				_children;
		NodeList				_belongingSubPlanChildren;
		NodeSet					_readyParents;
		bool					_isOpen;
	};
}

#endif 
