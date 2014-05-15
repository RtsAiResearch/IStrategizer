#ifndef GAMERESEARCH_H
#define GAMERESEARCH_H

#include "EngineData.h"

namespace IStrategizer
{
    class WorldResources;

    class GameResearch
    {
    public:
        GameResearch() : m_pRequiredResources(nullptr) {}
        ResearchType Id() const { return m_id; }
        WorldResources* RequiredResources() const { return m_pRequiredResources; }
        virtual void Init() = 0;

    protected:
        ResearchType m_id;
        WorldResources* m_pRequiredResources;
    };
}

#endif // GAMERESEARCH_H
