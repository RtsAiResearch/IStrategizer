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
            m_enemySpawnLocIdx(-1)
        {}

        void Init();
        void Update();
        bool IsScouting() const { return m_scoutController.EntityId() != INVALID_TID; }
        Vector2 GetEnemyStartLocation() const;
        bool IsEnemySpawnLocationKnown() const;
        int GetNearestSpawnLocation(_In_ bool checkNotDiscovered = false, _In_ bool checkEnemyNotExist = false);
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
        int m_enemySpawnLocIdx;
    };
}

#endif // SCOUTMANAGER_H