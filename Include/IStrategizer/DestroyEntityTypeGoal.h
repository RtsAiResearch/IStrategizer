///> [Serializable]
#ifndef DESTROYENTITYTYPEGOAL_H
#define DESTROYENTITYTYPEGOAL_H

#include "GoalEx.h"
#include <map>

namespace IStrategizer
{
    ///> class=DestroyEntityTypeGoal
    ///> parent=GoalEx
    class DestroyEntityTypeGoal : public GoalEx
    {
        OBJECT_SERIALIZABLE(DestroyEntityTypeGoal);

    private:
        int m_demandTargetSize;
        std::map<EntityClassType, int> m_destroyed;
        std::vector<GoalEx*> m_succededInstances;

    public:
        DestroyEntityTypeGoal();
        DestroyEntityTypeGoal(const PlanStepParameters& p_parameters);
        void Copy(IClonable* p_dest);
        void HandleMessage(RtsGame& game, Message* p_msg, bool& p_consumed);
        std::vector<GoalEx*> GetSucceededInstances(RtsGame &game);

    protected:
        void InitializePostConditions();
        bool SuccessConditionsSatisfied(RtsGame& game);
    };
}
#endif // DESTROYENTITYTYPEGOAL_H
