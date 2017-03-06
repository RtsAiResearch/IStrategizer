#ifndef GAMERACE_H
#define GAMERACE_H

#include <string>

namespace IStrategizer
{
    enum EntityClassType;
    enum ResourceType;
    class IGameRace;

    class GameRace
    {
    public:
        GameRace() :
            m_pRace(nullptr)
        {}

        GameRace(const IGameRace* pRace) :
            m_pRace(pRace)
        {}
        virtual ~GameRace() {}

        virtual EntityClassType GetWorkerType() const;
        virtual EntityClassType GetBaseType() const ;
        virtual EntityClassType GetResourceSource(ResourceType p_type) const;
        virtual int BaseSupplyAmount() const;
        virtual int SupplyBuildingSupplyAmount() const;
		virtual unsigned OptimalGatherersPerSource(ResourceType resource) const;

    protected:
        const IGameRace* m_pRace;
    };
}
#endif // GAMERACE_H