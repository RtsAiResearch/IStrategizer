#include "GameResearch.h"
#include "WorldResources.h"
#include <cassert>
#include "ObjectFactory.h"

using namespace IStrategizer;
using namespace std;

DECL_SERIALIZABLE(GameResearch);

void GameResearch::Init()
{
    if (m_pTech)
    {
        m_pRequiredResources = WorldResources(0, m_pTech->GasPrice(), m_pTech->MineralsPrice());
    }
    else
    {
        m_pRequiredResources = WorldResources(0, m_pUpgrade->GasPrice(), m_pTech->MineralsPrice());
    }
}
//////////////////////////////////////////////////////////////////////////
void GameResearch::GetRequirements(vector<ResearchType>& researches, map<EntityClassType, unsigned>& buildings) const
{
    // Is Upgrade
    if (m_pUpgrade)
    {
        if (m_pUpgrade->WhatsRequired() != nullptr)
        {
            EntityClassType requiredEntity = m_pUpgrade->WhatsRequired()->EngineId();
            buildings[requiredEntity] = 1;
        }
    }
}
//////////////////////////////////////////////////////////////////////////
EntityClassType GameResearch::SourceEntity() const
{
    // Is Upgrade
    if (m_pUpgrade)
    {
        return m_pUpgrade->WhatUpgrades()->EngineId();
    }
    // Is Tech
    else
    {
        return m_pTech->WhatResearches()->EngineId();
    }
}