///> [Serializable]
#ifndef GAMETYPE_H
#define GAMETYPE_H

#include "EngineData.h"
#include "WorldResources.h"
#include "EngineObject.h"
#include "SVector.h"
#include "RtsAiEngine.h"

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
            m_attributes(COUNT(EntityTypeProperty), 0)
        {}

        GameType(const IGameUnitType* pUnit) :
            m_id(pUnit->EngineId()),
            m_pUnitType(pUnit),
            m_attributes(COUNT(EntityTypeProperty), 0)
        {}

        EntityClassType Id() const { return m_id; }
        const WorldResources* RequiredResources() const { return &m_requiredResources; }
        int P(EntityTypeProperty attrId) const { return m_attributes[INDEX(attrId, EntityTypeProperty)]; }

        virtual void Init();
        virtual EntityClassType SourceEntity() const;
        virtual void GetRequirements(std::vector<ResearchType>& researches, std::map<EntityClassType, unsigned>& buildings) const;
        virtual EntityClassType GetBuilderType() const;
        std::string ToString(bool minimal = false) const { return m_pUnitType->ToString(); }

    protected:
        void P(EntityTypeProperty attrId, int val) { m_attributes[INDEX(attrId, EntityTypeProperty)] = val; }

        ///> type=int
        EntityClassType m_id;
        ///> type=vector(int)
        Serialization::SVector<int> m_attributes;
        WorldResources m_requiredResources;
        const IGameUnitType* m_pUnitType;
    };
}

#endif // GAMETYPE_H
