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
            m_scoutController(nullptr),
            m_pConsultant(pConsultant),
            m_active(false),
            m_knownEnemySpawnLocIdx(-1),
            m_currSpawnLocIdxToScout(-1)
        {}

        void Init();
        void Update();
        Vector2 GetEnemySpawnLocation() const { return (IsEnemySpawnLocationKnown() ? m_otherSpawnLocations.at(m_knownEnemySpawnLocIdx).Location : Vector2::Inf()); }
        bool IsEnemySpawnLocationKnown() const;
        void NotifyMessegeSent(_In_ Message* pMsg);
        Vector2 GetSuspectedEnemySpawnLocation();
        bool IsScounting() const { return m_scoutController.IsControllingEntity(); }
        bool IsActive() const { return m_active; }
        void Activate() { m_active = true; }
        void Deactivate() { m_active = false; }

    private:
        int GetNearestSpawnLocationIdx(_In_ bool checkNotDiscovered = false, _In_ bool checkEnemyNotExist = false);
        StrategySelectorPtr m_pConsultant;
        bool m_active;
        EntityController m_scoutController;

        struct SpawnLocationData
        {
            Vector2 Location;
            int DistanceToSelf;
            bool EnemyExist;
        };
        
        std::vector<SpawnLocationData> m_otherSpawnLocations;
        int m_currSpawnLocIdxToScout;
        int m_knownEnemySpawnLocIdx;
    };
}

#endif // SCOUTMANAGER_H