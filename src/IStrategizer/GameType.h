#ifndef GAMETYPE_H
#define GAMETYPE_H

#include "EngineData.h"
#include "WorldResources.h"
#include <map>

namespace IStrategizer
{
    class WorldResources;

    class GameType
    {
    public:
        virtual ~GameType() {}
        EntityClassType Id() const { return m_id; }
        const WorldResources* RequiredResources() const { return &m_requiredResources; }
        int Attr(EntityClassAttribute attrId) const { return m_attributes[INDEX(attrId, EntityClassAttribute)]; }

        virtual void Init() = 0;
        virtual EntityClassType SourceEntity() const = 0;
        virtual void GetRequirements(std::vector<ResearchType>& researches, std::map<EntityClassType, unsigned>& buildings) const = 0;
        virtual EntityClassType GetBuilderType() const = 0;

    protected:
        void Attr(EntityClassAttribute attrId, int val) { m_attributes[INDEX(attrId, EntityClassAttribute)] = val; }

        EntityClassType m_id;
        WorldResources m_requiredResources;
        int m_attributes[COUNT(EntityClassAttribute)];
    };
}

#endif // GAMETYPE_H
