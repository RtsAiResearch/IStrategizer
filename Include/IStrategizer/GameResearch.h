#ifndef GAMERESEARCH_H
#define GAMERESEARCH_H

#ifndef STATICCOMPONENT_H
#include "StaticComponent.h"
#endif

namespace IStrategizer
{
	enum ResearchType;
    class WorldResources;
    class GameDependency;

    class GameResearch : public StaticComponent
    {
    protected:
        ResearchType    m_id;
        WorldResources* m_requiredResources;

    public:
		GameResearch() : m_requiredResources(nullptr) {}
        int             Id() const { return m_id; }
        WorldResources* RequiredResources() const { return m_requiredResources; }
    };
}

#endif // GAMERESEARCH_H