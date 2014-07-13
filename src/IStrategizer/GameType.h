///> [Serializable]
#ifndef GAMETYPE_H
#define GAMETYPE_H

#include <map>
#include "EngineData.h"
#include "WorldResources.h"
#include "EngineObject.h"
#include "SVector.h"

namespace IStrategizer
{
    class WorldResources;

    ///> class=GameType
    class GameType : public EngineObject
    {
        OBJECT_SERIALIZABLE(GameType, &m_id, &m_attributes);
    public:
        GameType() :
            m_id(ECLASS_END),
            m_attributes(COUNT(EntityClassAttribute), 0)
        {}

        EntityClassType Id() const { return m_id; }
        const WorldResources* RequiredResources() const { return &m_requiredResources; }
        int Attr(EntityClassAttribute attrId) const { return m_attributes[INDEX(attrId, EntityClassAttribute)]; }

        virtual void Init() = 0;
        virtual EntityClassType SourceEntity() const = 0;
        virtual void GetRequirements(std::vector<ResearchType>& researches, std::map<EntityClassType, unsigned>& buildings) const = 0;
        virtual EntityClassType GetBuilderType() const = 0;

    protected:
        void Attr(EntityClassAttribute attrId, int val) { m_attributes[INDEX(attrId, EntityClassAttribute)] = val; }

        ///> type=int
        EntityClassType m_id;
        ///> type=vector(int)
        Serialization::SVector<int> m_attributes;
        WorldResources m_requiredResources;
    };
}

#endif // GAMETYPE_H
