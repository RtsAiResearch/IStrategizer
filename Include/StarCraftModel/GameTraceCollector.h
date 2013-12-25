#ifndef __GAMETRACECOLLETOR_H_INC__
#define __GAMETRACECOLLETOR_H_INC__

#include "BWAPI.h"

namespace IStrategizer
{
	class GameTraceCollector
	{
	public:
		~GameTraceCollector() {}

		//************************************
		// IStrategizer::GameTraceCollector::OnGameFrame
		// Description:	Handles each game frame to collect game traces
		// Returns:   	void
		//************************************
		void OnGameFrame();
	private:
		
		//************************************
		// IStrategizer::GameTraceCollector::CollectGameTraceForUnitOrder
		// Description:	Collect a game trace based on the provided unit order
		// Parameter: 	BWAPI::Unit unit: A unit with new order
		// Returns:   	void
		//************************************
		void CollectGameTraceForUnitOrder(const BWAPI::Unit unit);


		//************************************
		// IStrategizer::GameTraceCollector::IsAutoGatheringResources
		// Description:	Check whether the unit is already gathering resources and that his
		// current order is issued by the unit specialized micro-manager
		// For example: When a worker is ordered to gather minerals from a mine, the
		// unit micro-manager auto control the unit to: 
		// Move to mines, Mine minerals, Wait mine slot to mine, Return minerals, etc..
		// Parameter: 	BWAPI::Unit unit:
		// Returns:   	true if the unit is auto controlled, false otherwise
		//************************************
		bool IsAutoGatheringResources(const BWAPI::Unit unit);

		std::map<BWAPI::Unit, BWAPI::Order> m_unitOrder;
		std::map<BWAPI::Unit, BWAPI::Unit> m_unitOrderTarget;
		std::map<BWAPI::Unit, BWAPI::Position> m_unitOrderTargetPosition;
	};
}
#endif // __GAMETRACECOLLETOR_H_INC__