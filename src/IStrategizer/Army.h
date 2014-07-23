#ifndef ARMY_H
#define ARMY_H

#include "EngineData.h"
#include "Vector2.h"
#include "RtsGame.h"
#include <memory>

namespace IStrategizer
{
    class WorldClock;
    class GameType;

    enum ArmyGroup
    {
        ARMGRP_MeleelAttacker,
        ARMGRP_RangedAttacker,
        ARMGRP_Supporter,
        ARMGRP_Worker
    };

    class Army : public EngineObject
    {
    public:
        Army();
        ~Army();
        void NotifyMessegeSent(_In_ Message* p_msg);
        Vector2 Center() const;
        TID Leader() const;
        bool Empty() const { return m_armySize == 0; }
        const EntitySet& Group(_In_ ArmyGroup group) const { return m_groups.at(group); }
        bool HasType(_In_ EntityClassType type);
        void SelectArmy(_In_ RtsGame& game);
        void Exclude(_In_ TID entityId);
        void Release();

    private:
        ArmyGroup Classify(const GameType* pType);
        std::map<ArmyGroup, EntitySet> m_groups;
        int m_armySize;
    };

    typedef std::shared_ptr<Army> ArmyPtr;
}

#endif // ARMY_H