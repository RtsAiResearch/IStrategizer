#ifndef GAMERACE_H
#define GAMERACE_H

#include <string>

namespace IStrategizer
{
    enum EntityClassType;
    enum ResourceType;

    class GameRace
    {
    public:
        virtual ~GameRace() {}

        virtual EntityClassType GetWorkerType() const = 0;
        virtual EntityClassType GetBaseType() const = 0;
        virtual EntityClassType GetResourceSource(ResourceType p_type) const = 0;
        virtual float GetResourceConsumbtionRatePerWorker(ResourceType p_id) const = 0;
        virtual int BaseSupplyAmount() const = 0;
        virtual int SupplyBuildingSupplyAmount() const = 0;
        virtual const std::string& ToString() const = 0;
    };
}
#endif // GAMERACE_H