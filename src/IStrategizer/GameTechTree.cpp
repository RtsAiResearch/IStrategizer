#include "GameTechTree.h"
#include "MetaData.h"
#include <cassert>
#include <string>
#include <map>
#include "ObjectFactory.h"
#include "RtsGame.h"

using namespace IStrategizer;
using namespace std;

DECL_SERIALIZABLE(GameTechTree);

bool GameTechTree::ResearchAvailable(ResearchType researchId) const
{
    if (m_isOnline)
    {
        if (researchId == RESEARCH_END)
            return false;

        // Is Tech
        if (g_GameImpl->GetTechTypeByEngineId(researchId) != nullptr)
        {
            return g_GameImpl->PlayerIsResearchAvailable(m_playerId, g_GameImpl->GetTechTypeByEngineId(researchId));
        }
        // Is Upgrade
        else
        {
            return g_GameImpl->PlayerMaxUpgradeLevel(m_playerId, g_GameImpl->GetUpgradeTypeByEngineId(researchId)) > 0;
        }
    }
    else
        return m_cachedAvailResearches.count(researchId) > 0;
}
//////////////////////////////////////////////////////////////////////////
bool GameTechTree::ResearchDone(ResearchType researchId) const
{
    if (m_isOnline)
    {
        if (researchId == RESEARCH_END)
            return false;

        // Is Tech
        if (g_GameImpl->GetTechTypeByEngineId(researchId) != nullptr)
        {
            return g_GameImpl->PlayerHasResearched(m_playerId, g_GameImpl->GetTechTypeByEngineId(researchId));
        }
        // Is Upgrade
        else
        {
            // FIXME: we restrict upgrade levels to be 1 level only
            return g_GameImpl->PlayerUpgradeLevel(m_playerId, g_GameImpl->GetUpgradeTypeByEngineId(researchId)) > 0;
        }
    }
    else
        return m_cachedDoneResearches.count(researchId) > 0;
}
//////////////////////////////////////////////////////////////////////////
void GameTechTree::SetOffline(RtsGame* pBelongingGame)
{
    _ASSERTE(m_isOnline);

    for (auto researchEntry : g_Game->ResearchTypes())
    {
        if (ResearchAvailable(researchEntry.first))
            m_cachedAvailResearches.insert(researchEntry.first);

        if (ResearchDone(researchEntry.first))
            m_cachedDoneResearches.insert(researchEntry.first);
    }

    m_isOnline = false;
}
