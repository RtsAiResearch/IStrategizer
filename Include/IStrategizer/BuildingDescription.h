#ifndef BUILDINGDESCRIPTION_H
#define BUILDINGDESCRIPTION_H

namespace IStrategizer
{
    class GameEntity;

    class BuildingDescription
    {
    public:
        int m_numberOfBuildings;
        int m_numberOfCriticalBuildings;

        void AddEntity(GameEntity *p_entity);
        void RemoveEntity(GameEntity *p_entity);
        void Clear();
        float GetDistance(BuildingDescription *p_other);
    
    protected:
        void        InitializeAddressesAux();
    };
}

#endif // BUILDINGDESCRIPTION_H
