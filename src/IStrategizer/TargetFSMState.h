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
        TargetFSMState(TController controller) : 
            m_enemyAttacked(false),
            m_targetId(DONT_CARE),
            m_targetTypeRank(DONT_CARE),
            m_enemyEntitiesCount(DONT_CARE),
            FSMState(Target, controller)
        {
            m_targetRanking.push_back(ECATTR_IsAttacker);
            m_targetRanking.push_back(ECATTR_IsCowrad);
            m_targetRanking.push_back(ECATTR_IsCritical);
            m_targetRanking.push_back(ECATTR_IsBuilding);
        }

        void Enter(RtsGame& game, const WorldClock& clock)
        {
            SelectEnemyTarget(game);
            ControllerTraits<TController>::Type battle = m_controller;
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
            EntitySet army = TControllerTraits::Army(battle);

            return (army.empty() || m_enemyEntitiesCount == 0) ? Finished : Attack ;
        }

    private:
        bool ArmyUnderAttack(RtsGame& game)
        {
            EntityList entities;
            game.Enemy()->Entities(entities);
            for (TID enemyEntityId : entities)
            {
                GameEntity* pEnemyEntity = game.Enemy()->GetEntity(enemyEntityId);
                _ASSERTE(pEnemyEntity);
                ObjectStateType enemyEntityState = (ObjectStateType)pEnemyEntity->Attr(EOATTR_State);

                if (enemyEntityState == OBJSTATE_Attacking)
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
            std::map<EntityClassAttribute, std::stack<TID>> targets;
            game.Enemy()->Entities(entities);
            m_enemyEntitiesCount = entities.size();
            
            int rank = m_targetRanking.size();
            for (EntityClassAttribute attr : m_targetRanking)
            {
                for (TID entityId : entities)
                {
                    GameType* pEntityType = game.GetEntityType(game.Enemy()->GetEntity(entityId)->Type());

                    if (!g_Assist.IsEntityObjectReady(entityId, PLAYER_Enemy) && !pEntityType->Attr(ECATTR_IsBuilding))
                        continue;

                    if (pEntityType->Attr(attr) && rank > m_targetTypeRank)
                    {
                        m_targetId = entityId;
                        m_targetTypeRank = rank;
                        break;
                    }
                }
                --rank;
            }
        }

        TID m_targetId;
        int m_targetTypeRank;
        int m_enemyEntitiesCount;
        bool m_enemyAttacked;
        std::vector<EntityClassAttribute> m_targetRanking;
    };
}

#endif // DEPLOYFSMSTATE_H