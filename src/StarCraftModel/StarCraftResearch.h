#ifndef STARCRAFTRESEARCH_H
#define STARCRAFTRESEARCH_H

#ifndef GAMERESEARCH_H
#include "GameResearch.h"
#endif

#include "BWAPI.h"

namespace IStrategizer
{
    class StarCraftResearch : public GameResearch
    {
    public:
        StarCraftResearch(BWAPI::UpgradeType p_research) : m_upgrade(p_research), m_isTech(false) {}
        StarCraftResearch(BWAPI::TechType p_research) : m_tech(p_research), m_isTech(true) {}
        void Init();
        EntityClassType SourceEntity() const;
        void GetRequirements(std::vector<ResearchType>& researches, std::map<EntityClassType, unsigned>& buildings) const;

    private:
        bool m_isTech;
        BWAPI::UpgradeType m_upgrade;
        BWAPI::TechType m_tech;
    };
}

#endif // STARCRAFTRESEARCH_H
