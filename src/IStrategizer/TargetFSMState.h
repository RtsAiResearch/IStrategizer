#ifndef DEPLOYFSMSTATE_H
#define DEPLOYFSMSTATE_H

#include "FSMState.h"
#include "EngineData.h"
#include "RtsGame.h"
#include "GamePlayer.h"
#include "GameType.h"
#include "GameEntity.h"
#include "EngineAssist.h"
#include <stack>
#include <map>

namespace IStrategizer
{
    template<class TController, class TControllerTraits = ControllerTraits<TController>>
    class TargetFSMState : public FSMState<TController, ControllerTraits<TController>>
    {
    public:
        TargetFSMState(TController controller, BattleStateType nextState) :
            m_enemyAttacked(false),
            m_targetId(DONT_CARE),
            m_targetTypeRank(DONT_CARE),
            m_enemyEntitiesCount(DONT_CARE),
            FSMState(Target, controller),
            m_nextState(nextState)
        {
            m_targetRanking.push_back(TP_IsAttacker);
            m_targetRanking.push_back(TP_IsWorker);
            m_targetRanking.push_back(TP_IsProducer);
            m_targetRanking.push_back(TP_IsBuilding);
        }

        void Enter(RtsGame& game, const WorldClock& clock)
        {
            ControllerTraits<TController>::Type battle = m_controller;
            Army* pArmy = TControllerTraits::GetArmy(battle);
            
            if (pArmy->Empty())
            {
                // Do not select target if the army is empty.
                return;
            }

            if (!ArmyUnderAttack(game))
            {
                SelectEnemyTarget(game);
            }

            if (m_targetId != DONT_CARE)
            {
                TControllerTraits::NextTarget(battle, m_targetId);
            }
        }

        void Exit(RtsGame& game, const WorldClock& clock)
        {
            m_enemyAttacked = false;
            m_targetId= DONT_CARE;
            m_targetTypeRank = DONT_CARE;
            m_enemyEntitiesCount = DONT_CARE;
        }
        
        int CheckTransitions(RtsGame& game, const WorldClock& clock)
        {
            ControllerTraits<TController>::ConstType battle = m_controller;
            Army* pArmy = TControllerTraits::GetArmy(battle);

            return (pArmy->Empty() || m_enemyEntitiesCount == 0) ? Finished : m_nextState;
        }

    private:
        bool ArmyUnderAttack(RtsGame& game)
        {
            EntityList entities;
            game.Enemy()->Entities(entities);
            int closestDistance = INT_MAX;
            ControllerTraits<TController>::ConstType battle = m_controller;
            Army* pArmy = TControllerTraits::GetArmy(battle);
            Vector2 armyCenter = pArmy->Center();

            for (TID enemyEntityId : entities)
            {
                GameEntity* pEnemyEntity = game.Enemy()->GetEntity(enemyEntityId);
                _ASSERTE(pEnemyEntity);
                ObjectStateType enemyEntityState = (ObjectStateType)pEnemyEntity->P(OP_State);
                int distance = armyCenter.Distance(pEnemyEntity->GetPosition());

                if (enemyEntityState == OBJSTATE_Attacking && distance <= closestDistance)
                {
                    m_targetId = enemyEntityId;
                    m_enemyAttacked = true;
                    m_targetTypeRank = m_targetRanking.size() + 1;
                    return true;
                }
            }

            return false;
        }

        void SelectEnemyTarget(RtsGame &game)
        {
            EntityList entities;
            game.Enemy()->Entities(entities);
            m_enemyEntitiesCount = entities.size();
            int closestDistance = INT_MAX;

            ControllerTraits<TController>::ConstType battle = m_controller;
            Army* pArmy = TControllerTraits::GetArmy(battle);
            Vector2 armyCenter = pArmy->Center();
            
            int rank = m_targetRanking.size();
            for (EntityTypeProperty attr : m_targetRanking)
            {
                for (TID entityId : entities)
                {
                    GameEntity* pGameEntity = game.Enemy()->GetEntity(entityId);
                    GameType* pEntityType = game.GetEntityType(pGameEntity->TypeId());

                    if (!g_Assist.IsEntityObjectReady(entityId, PLAYER_Enemy) && !pEntityType->P(TP_IsBuilding))
                        continue;

                    int distance = armyCenter.Distance(pGameEntity->GetPosition());

                    if (pEntityType->P(attr) && rank > m_targetTypeRank && distance <= closestDistance)
                    {
                        m_targetId = entityId;
                        m_targetTypeRank = rank;
                        closestDistance = distance;
                    }
                }
                --rank;
            }
        }

        TID m_targetId;
        int m_targetTypeRank;
        int m_enemyEntitiesCount;
        bool m_enemyAttacked;
        BattleStateType m_nextState;
        std::vector<EntityTypeProperty> m_targetRanking;
    };
}

#endif // DEPLOYFSMSTATE_H