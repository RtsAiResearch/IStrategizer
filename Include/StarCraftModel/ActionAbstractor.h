#ifndef __ACTIONABSTRACTOR_H_INC__
#define __ACTIONABSTRACTOR_H_INC__

#include "BWAPI.h"
#include "EngineData.h"

namespace IStrategizer
{
    class ActionAbstractor
    {
    public:
        //************************************
        // IStrategizer::ActionAbstractor::GetAbstractedParameter
        // Description:    Gets abstracted parameters for a given action
        // Parameter:     const BWAPI::Unit trainee: The trained unit
        // Parameter:     const BWAPI::Unit trainer: The trainer for the unit
        // Returns:       PlanStepParameters
        //************************************
        PlanStepParameters GetAbstractedParameter(const BWAPI::Unit trainee, const BWAPI::Unit trainer) const;

        //************************************
        // IStrategizer::ActionAbstractor::GetAbstractedParameter
        // Description:    Gets abstracted parameters for a given action
        // Parameter:     ActionType action: the action type.
        // Parameter:     BWAPI::Unit unit: The unit that made action
        // Returns:       void
        //************************************
        PlanStepParameters GetAbstractedParameter(ActionType actionType, const BWAPI::Unit unit) const;

    private:
        PlanStepParameters GetAbstractedParameterAux(PlanStepParameters actionParameters, const BWAPI::Unit trainee, const BWAPI::Unit trainer) const;
    };
}

#endif // __ACTIONABSTRACTOR_H_INC__