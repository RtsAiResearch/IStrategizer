#ifndef SCOUTMANAGER_H
#define SCOUTMANAGER_H

#include "Vector2.h"
#include "EngineObject.h"
#include "StrategySelector.h"
#include "EntityController.h"

namespace IStrategizer
{
    class RtsGame;
    class Message;

    class ScoutManager : public EngineObject
    {
    public:
        ScoutManager(StrategySelectorPtr pConsultant) :
            m_pConsultant(pConsultant),
            m_active(false),
            m_targetSpawnLocationId(0)
        {}

        void Init();
        void Update(_In_ RtsGame& game);
        void StartScouting() { m_active = true; }
        void StopScouting() { m_active = false; }
        bool IsScouting() const { return m_scoutController.EntityId() != INVALID_TID; }
        bool IsEnemySpawnLocationKnown() const;
        void NotifyMessegeSent(_In_ Message* pMsg);
        
    private:
        StrategySelectorPtr m_pConsultant;
        bool m_active;
        EntityController m_scoutController;

        struct SpawnLocationData
        {
            Vector2 Location;
            int DistanceToSelf;
            bool Discovered;
            bool EnemyExist;
        };
        
        std::vector<SpawnLocationData> m_otherSpawnLocations;
        size_t m_targetSpawnLocationId;
    };
}

#endif // SCOUTMANAGER_H