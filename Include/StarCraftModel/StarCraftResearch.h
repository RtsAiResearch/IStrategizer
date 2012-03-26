#ifndef STARCRAFTRESEARCH_H
#define STARCRAFTRESEARCH_H

#ifndef GAMERESEARCH_H
#include "GameResearch.h"
#endif

#include "bwapi/UpgradeType.h"
#include "bwapi/TechType.h"

namespace StarCraftModel
{
    using namespace IStrategizer;
	using namespace BWAPI;
    class StarCraftResearch : public GameResearch
    {
	public:
		StarCraftResearch(UpgradeType p_research) : m_upgrade(p_research), m_isTech(false) {}
		StarCraftResearch(TechType p_research) : m_tech(p_research), m_isTech(true) {}

	protected:
		void    InitializeAux();

	private:
		bool		m_isTech;
        UpgradeType m_upgrade;
		TechType	m_tech;
    };
}

#endif // STARCRAFTRESEARCH_H