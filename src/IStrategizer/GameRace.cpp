#include "GameRace.h"
#include "EngineData.h"
#include "RtsGame.h"

using namespace IStrategizer;
using namespace std;

EntityClassType GameRace::GetWorkerType() const
{
    return m_pRace->WorkerType()->EngineId();
}
//////////////////////////////////////////////////////////////////////////
EntityClassType GameRace::GetBaseType() const
{
    return m_pRace->BaseType()->EngineId();
}
//////////////////////////////////////////////////////////////////////////
int GameRace::BaseSupplyAmount() const
{
    return 10;
}
//////////////////////////////////////////////////////////////////////////
int GameRace::SupplyBuildingSupplyAmount() const
{
    return 8;
}
//////////////////////////////////////////////////////////////////////////
EntityClassType GameRace::GetResourceSource(ResourceType p_type) const
{
    // Only terran is currently supported

    switch (p_type)
    {
    case RESOURCE_Primary:
        return m_pRace->MineralsProvider()->EngineId();
    case RESOURCE_Secondary:
        return m_pRace->GasProvider()->EngineId();
    case RESOURCE_Supply:
        return m_pRace->SupplyProvider()->EngineId();
    default:
        DEBUG_THROW(InvalidParameterException(XcptHere));
    }
}
//////////////////////////////////////////////////////////////////////////
unsigned GameRace::OptimalGatherersPerSource(ResourceType resource) const
{
    // Only terran is currently supported
    switch (resource)
    {
    case RESOURCE_Primary:
        return 3;

    case RESOURCE_Secondary:
        return 3;

    case RESOURCE_Supply:
        return 0;

    default:
        DEBUG_THROW(InvalidParameterException(XcptHere));
    }
}