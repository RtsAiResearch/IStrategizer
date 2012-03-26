#ifndef PLANTREEEX_H
#define PLANTREEEX_H

#include <vector>
using namespace std;

namespace OLCBP
{
	class PlanTreeNodeEx;
	class PlanTreeEx
	{
	private:
		PlanTreeNodeEx* _root;
	public:
		PlanTreeEx();
		PlanTreeEx(PlanTreeNodeEx* p_root);
		PlanTreeNodeEx* Root(){return _root;}
	};
}


#endif
