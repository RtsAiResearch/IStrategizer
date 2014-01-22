#ifndef __ACTIONABSTRACTOR_H_INC__
#define __ACTIONABSTRACTOR_H_INC__

#include "BWAPI.h"
#include "EngineData.h"
#include "RtsGame.h"

namespace IStrategizer
{
    class ActionAbstractor
    {
    public:
        //************************************
        // IStrategizer::ActionAbstractor::GetAbstractedParameter
        // Description:    Gets abstracted parameters for a given action
        // Parameter:     RtsGame& p_RtsGame: The game instance to act upon
        // Parameter:     const BWAPI::Unit trainee: The trained unit
        // Parameter:     const BWAPI::Unit trainer: The trainer for the unit
        // Returns:       PlanStepParameters
        //************************************
        PlanStepParameters GetAbstractedParameter(RtsGame& p_RtsGame, const BWAPI::Unit trainee, const BWAPI::Unit trainer) const;

        //************************************
        // IStrategizer::ActionAbstractor::GetAbstractedParameter
        // Description:    Gets abstracted parameters for a given action
        // Parameter:     RtsGame& p_RtsGame: The game instance to act upon
        // Parameter:     ActionType action: the action type.
        // Parameter:     BWAPI::Unit unit: The unit that made action
        // Returns:       void
        //************************************
        PlanStepParameters GetAbstractedParameter(RtsGame& p_RtsGame, ActionType actionType, const BWAPI::Unit unit) const;

    private:
        PlanStepParameters GetAbstractedParameterAux(RtsGame& p_RtsGame, PlanStepParameters actionParameters, const BWAPI::Unit trainee, const BWAPI::Unit trainer) const;
    };
}

#endif // __ACTIONABSTRACTOR_H_INC__
