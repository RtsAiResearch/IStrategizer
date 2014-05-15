#ifndef GAMETYPE_H
#define GAMETYPE_H

#ifndef METADATA_H
#include "MetaData.h"
#endif

namespace IStrategizer
{
    class WorldResources;

    class GameType
    {
    public:
        GameType() : m_requiredResources(nullptr) {}
        ~GameType();
        EntityClassType Id() const { return m_id; }
        WorldResources* RequiredResources() const { return m_requiredResources; }
        int Attr(EntityClassAttribute p_attrId) const { return m_attributes[INDEX(p_attrId, EntityClassAttribute)]; }
        virtual void Init() = 0;

    protected:
        void Attr(EntityClassAttribute p_attrId, int p_val) { m_attributes[INDEX(p_attrId, EntityClassAttribute)] = p_val; }

        EntityClassType m_id;
        WorldResources* m_requiredResources;
        int m_attributes[COUNT(EntityClassAttribute)];
    };
}

#endif // GAMETYPE_H
