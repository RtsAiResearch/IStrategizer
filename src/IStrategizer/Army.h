#ifndef ARMY_H
#define ARMY_H

#include "EngineObject.h"
#include "EngineData.h"

namespace IStrategizer
{
    class RtsGame;
    class WorldClock;
    class Vector2;
    class GameType;

    enum ArmyGroup
    {
        ARMGRP_MeleelAttacker,
        ARMGRP_RangedAttacker,
        ARMGRP_Supporter
    };

    class Army : public EngineObject
    {
    public:
        Army(RtsGame& game);
        ~Army();
        void NotifyMessegeSent(Message* p_msg);
        Vector2 Center() const;
        TID Leader() const;
        bool Empty() const { return m_armySize == 0; }
        const EntitySet& MeleeAttackers() const { return m_groups.at(ARMGRP_MeleelAttacker); }
        const EntitySet& RangedAttackers() const { return m_groups.at(ARMGRP_RangedAttacker); }
        const EntitySet& Supporters() const { return m_groups.at(ARMGRP_Supporter); }
        bool HasType(EntityClassType type);
        void Move(Vector2 pos, Vector2 formationFrontDir);
        void Stop();
        void UseTech(ResearchType tech);
        void Attack(TID entityId);

    private:
        ArmyGroup Classify(const GameType* pType);
        void SelectArmy(RtsGame& game);

        std::map<ArmyGroup, EntitySet> m_groups;
        int m_armySize;
    };
}

#endif // ARMY_H