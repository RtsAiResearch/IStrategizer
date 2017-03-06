///> [Serializable]
#ifndef GAMERESEARCH_H
#define GAMERESEARCH_H

#include "EngineData.h"
#include "WorldResources.h"
#include "EngineObject.h"
#include "RtsAiEngine.h"

namespace IStrategizer
{
    class WorldResources;

    ///> class=GameResearch
    class GameResearch : public EngineObject
    {
        OBJECT_SERIALIZABLE(GameResearch, &m_id);
    public:
        GameResearch() :
            m_id(RESEARCH_END),
            m_pTech(nullptr),
            m_pUpgrade(nullptr)
        {}

        GameResearch(const IGameTechType* pTech) :
            m_id(pTech->EngineId()),
            m_pTech(pTech),
            m_pUpgrade(nullptr)
        {}

        GameResearch(const IGameUpgradeType* pUpgrade) :
            m_id(pUpgrade->EngineId()),
            m_pUpgrade(pUpgrade),
            m_pTech(nullptr)
        {}

        ResearchType Id() const { return m_id; }
        virtual ~GameResearch() {}
        const WorldResources* RequiredResources() const { return &m_pRequiredResources; }

        virtual void Init();
        virtual EntityClassType SourceEntity() const;
        virtual void GetRequirements(std::vector<ResearchType>& researches, std::map<EntityClassType, unsigned>& buildings) const;

    protected:
        ///> type=int
        ResearchType m_id;
        WorldResources m_pRequiredResources;
        const IGameTechType* m_pTech;
        const IGameUpgradeType* m_pUpgrade;
    };
}

#endif // GAMERESEARCH_H
