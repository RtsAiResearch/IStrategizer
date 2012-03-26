#ifndef GAMERESEARCH_H
#define GAMERESEARCH_H

#ifndef STATICCOMPONENT_H
#include "StaticComponent.h"
#endif

#include <string>
#include <vector>
using namespace std;

namespace MetaData
{
    enum ResearchType;
}

namespace IStrategizer
{
    class WorldResources;
    class GameDependency;

    using namespace MetaData;
    class GameResearch : public StaticComponent
    {
    protected:
        ResearchType    m_id;
        WorldResources* m_requiredResources;

    public:
		GameResearch() : m_requiredResources(NULL) {}
        int             Id() const { return m_id; }
        WorldResources* RequiredResources() const { return m_requiredResources; }
    };
}

#endif // GAMERESEARCH_H