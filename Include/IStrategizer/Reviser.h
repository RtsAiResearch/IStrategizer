#ifndef REVISER_H
#define REVISER_H

#ifndef ABSTRACTREVISER_H
#include "AbstractReviser.h"
#endif

namespace OLCBP
{
	class Reviser : public AbstractReviser
	{
	public:
		void Revise(CaseEx* p_case, bool p_succeeded);
	};
}


#endif // REVISER_H