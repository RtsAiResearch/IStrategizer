#ifndef BATTLE_H
#define BATTLE_H

#include "StackFSM.h"
#include "StrategySelector.h"
#include "EntityController.h"
#include <unordered_map>

namespace IStrategizer
{
    enum ArmyGroup
    {
        ARMGRP_MeleelAttacker,
        ARMGRP_RangedAttacker,
        ARMGRP_Supporter,
        ARMGRP_Worker
    };

    class ArmyController : public EngineObject
    {
    public:
        ArmyController(StrategySelectorPtr pConsultant);

        void Update();
        TID TargetEntity() const { return m_currentTarget; }
        void AttackArea(_In_ Vector2 pos) {}
        void DefendArea(_In_ Vector2 pos);
        void NotifyMessegeSent(_In_ Message* p_msg);
        Vector2 Center() const;
        bool IsControllingArmy() const { return m_armySize == 0; }
        bool HasType(_In_ EntityClassType type);
        void ReleaseEntity(_In_ TID entityId);
        void ControlArmy();
        void ReleaseArmy();

    private:
        ArmyGroup Classify(const GameType* pType);

        std::map<ArmyGroup, std::unordered_map<TID, EntityControllerPtr>> m_groups;

        int m_armySize;
        StackFSMPtr m_pLogic;
        TID m_currentTarget;
        StrategySelectorPtr m_pConsultant;
    };
}

#endif // BATTLE_H