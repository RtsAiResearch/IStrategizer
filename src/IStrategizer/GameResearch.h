#ifndef GAMERESEARCH_H
#define GAMERESEARCH_H

#include "EngineData.h"
#include "WorldResources.h"

namespace IStrategizer
{
    class WorldResources;

    class GameResearch
    {
    public:
        ResearchType Id() const { return m_id; }
        virtual ~GameResearch() {}
        const WorldResources* RequiredResources() const { return &m_pRequiredResources; }

        virtual void Init() = 0;
        virtual EntityClassType SourceEntity() const = 0;
        virtual void GetRequirements(std::vector<ResearchType>& researches, std::map<EntityClassType, unsigned>& buildings) const = 0;

    protected:
        ResearchType m_id;
        WorldResources m_pRequiredResources;
    };
}

#endif // GAMERESEARCH_H
