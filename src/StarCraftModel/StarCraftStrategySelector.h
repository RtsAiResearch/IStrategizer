#ifndef STARCRAFTSTRATEGYSELECTOR_H
#define STARCRAFTSTRATEGYSELECTOR_H

#include "StrategySelector.h"
#include "Battle.h"
#include "TargetFSMState.h"
#include "AttackFSMState.h"
#include "FinishedFSMState.h"

namespace IStrategizer
{
    class StarCraftStrategySelector : public StrategySelector
    {
    public:
        StarCraftStrategySelector() : m_currentTrainOrder(0)
        {
            PlanStepParameters params;
            params[PARAM_AlliedUnitsTotalHP] = 160;
            params[PARAM_AlliedUnitsTotalDamage] = 196;
            m_trainOrders.push_back(params);
        }

        void SelectAttackStrategy(Army* pArmy, std::vector<FSMState<Battle*>*>& states) const
        {
            states.push_back(new TargetFSMState<Battle*>(nullptr));
            states.push_back(new AttackFSMState<Battle*>(nullptr));
            states.push_back(new FinishedFSMState<Battle*>(nullptr));
        }

        void SelectTrainOrder(std::string map, PlanStepParameters& params)
        {
            int orderInx;
            if (m_currentTrainOrder == m_trainOrders.size() - 1)
            {
                orderInx = m_currentTrainOrder;
            }
            else
            {
                orderInx = ++m_currentTrainOrder;
            }

            params = m_trainOrders[orderInx];
        }

    private:
        int m_currentTrainOrder;
        std::vector<PlanStepParameters> m_trainOrders;
    };
}

#endif // STARCRAFTSTRATEGYSELECTOR_H