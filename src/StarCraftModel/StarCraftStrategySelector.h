#ifndef STARCRAFTSTRATEGYSELECTOR_H
#define STARCRAFTSTRATEGYSELECTOR_H

#include "StrategySelector.h"
#include "Battle.h"
#include "TargetFSMState.h"
#include "BasicAttackFSMState.h"
#include "FinishedFSMState.h"
#include "DeployFSMState.h"
#include "TankAttackFSMState.h"
#include "BWAPI.h"

using namespace BWAPI;

namespace IStrategizer
{
    class StarCraftStrategySelector : public StrategySelector
    {
    public:
        StarCraftStrategySelector(std::string map) : m_currentTrainOrder(0)
        {
            PlanStepParameters params;

            // 4 Tanks
            params[PARAM_AlliedUnitsTotalHP] = 600;
            params[PARAM_AlliedUnitsTotalDamage] = 124;
            m_trainOrders.push_back(params);

            //// 4 Marines
            //params[PARAM_AlliedUnitsTotalHP] = 160;
            //params[PARAM_AlliedUnitsTotalDamage] = 196;
            //m_trainOrders.push_back(params);

            //// 4 Firebats
            //params[PARAM_AlliedUnitsTotalHP] = 200;
            //params[PARAM_AlliedUnitsTotalDamage] = 36;
            //m_trainOrders.push_back(params);
        }

        void SelectAttackStrategy(Army* pArmy, std::vector<FSMState<Battle*>*>& states) const
        {
            if (HasUnitType(pArmy, UnitTypes::Terran_Siege_Tank_Tank_Mode.getID()))
            {
                states.push_back(new TargetFSMState<Battle*>(nullptr, Deploy));
                states.push_back(new DeployFSMState<Battle*>(nullptr, WeaponTypes::Arclite_Shock_Cannon.maxRange(), TankAttack));
                states.push_back(new TankAttackFSMState<Battle*>(nullptr));
                states.push_back(new FinishedFSMState<Battle*>(nullptr));
            }
            else
            {
                states.push_back(new TargetFSMState<Battle*>(nullptr, BasicAttack));
                states.push_back(new BasicAttackFSMState<Battle*>(nullptr));
                states.push_back(new FinishedFSMState<Battle*>(nullptr));
            }
        }

        void SelectTrainOrder(PlanStepParameters& params)
        {
            int orderInx;
            if (m_currentTrainOrder == m_trainOrders.size() - 1)
            {
                orderInx = m_currentTrainOrder;
            }
            else
            {
                orderInx = m_currentTrainOrder++;
            }

            params = m_trainOrders[orderInx];
        }

    private:
        bool HasUnitType(Army* pArmy, int typeId) const
        {
            EntitySet entities = pArmy->Entities();

            for (TID id : entities)
            {
                if (Broodwar->getUnit(id)->getType().getID() == typeId)
                {
                    return true;
                }
            }

            return false;
        }

        int m_currentTrainOrder;
        std::vector<PlanStepParameters> m_trainOrders;
    };
}

#endif // STARCRAFTSTRATEGYSELECTOR_H