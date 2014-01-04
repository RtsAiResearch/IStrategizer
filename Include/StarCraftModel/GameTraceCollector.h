#ifndef __GAMETRACECOLLETOR_H_INC__
#define __GAMETRACECOLLETOR_H_INC__

#include "BWAPI.h"
#include "EngineData.h"
#include "ActionAbstractor.h"
#include <set>

namespace IStrategizer
{
    class GameTrace;

    class GameTraceCollector
    {
    public:
        GameTraceCollector(TID playerToObserveID)
            : m_playerToObserveID(playerToObserveID),
            m_isFirstUpdate(true)
        {}

        ~GameTraceCollector() {}

        //************************************
        // IStrategizer::GameTraceCollector::OnGameFrame
        // Description: Handles each game frame to collect game traces
        // Returns:    void
        //************************************
        void OnGameFrame();
    private:

        //************************************
        // IStrategizer::GameTraceCollector::CollectGameTraceForUnitOrder
        // Description: Collect a game trace based on the provided unit order
        // Parameter:  BWAPI::Unit unit: A unit with new order
        // Returns:    void
        //************************************
        void CollectGameTraceForUnitOrder(const BWAPI::Unit unit);

        //************************************
        // IStrategizer::GameTraceCollector::IsAutoGatheringResources
        // Description: Check whether the unit is already gathering resources and that his
        // current order is issued by the unit specialized micro-manager
        // For example: When a worker is ordered to gather minerals from a mine, the
        // unit micro-manager auto control the unit to: 
        // Move to mines, Mine minerals, Wait mine slot to mine, Return minerals, etc..
        // Parameter:  BWAPI::Unit unit:
        // Returns:    true if the unit is auto controlled, false otherwise
        //************************************
        bool IsAutoGatheringResources(const BWAPI::Unit unit);

        //************************************
        // IStrategizer::GameTraceCollector::IsOrderChanged
        // Description: Check whether the unit should be considered to received a new
        // order from the last order recorded for it
        // Parameter:  BWAPI::Unit unit:
        // Returns:    true if got a new order, false otherwise
        //************************************
        bool IsOrderChanged(BWAPI::Unit unit);

        //************************************
        // IStrategizer::GameTraceCollector::IsPlayerIssuedOrder
        // Description: Check whether a certain order can be issued only from player
        // and not the unit micro-manager
        // Parameter:  BWAPI:Order order: the order to check for
        // Returns:    true if player issued order, false otherwise
        //************************************
        bool IsPlayerIssuedOrder(const BWAPI::Order order);

        //************************************
        // IStrategizer::GameTraceCollector::InitPlayerIssuedOrderTable
        // Description: Fill a table IDs of all orders that can be issued only a player
        // not by any unit micro-manager
        // Returns:    void
        //************************************
        void InitPlayerIssuedOrderTable();

        //************************************
        // IStrategizer::GameTraceCollector::HasNewPlayerOrder
        // Description: We collect only orders issued by the player not the micro-manager and
        //  in only we consider an order to be new if it changed from last known order
        // for that particular unit
        // Parameter:  BWAPI::Unit unit:
        // Returns:    true the current unit order is new player order, false otherwise
        //************************************
        bool HasNewPlayerOrder(const BWAPI::Unit unit);

        //************************************
        // IStrategizer::GameTraceCollector::IsUnitBeingTrained
        // Description: 
        // Parameter:  const BWAPI::Unit unit:
        // Returns:    bool
        //************************************
        bool IsUnitBeingTrained(const BWAPI::Unit unit);

        //************************************
        // IStrategizer::GameTraceCollector::CollectGameTraceForTrainedUnit
        // Description: 
        // Parameter:  const BWAPI::Unit trainee:
        // Parameter:  const BWAPI::Unit trainer:
        // Returns:    void
        //************************************
        void CollectGameTraceForTrainedUnit(const BWAPI::Unit trainee, const BWAPI::Unit trainer);

        //************************************
        // IStrategizer::GameTraceCollector::ReasonTrainerUnitForTrainee
        // Description: Figure out which unit is training that particular unit in the
        // the current game frame and place that relation in the trainee to trainer map
        // which maps trainee unit to its trainer unit
        // Parameter:  BWAPI::Unit unit: the unit to reason about
        // Returns:    BWAPI::Unit the suspected trainer for trainee provided
        //************************************
        BWAPI::Unit ReasonTrainerUnitForTrainee(const BWAPI::Unit trainee);

        //************************************
        // IStrategizer::GameTraceCollector::SendGameTrace
        // Description: Send collected game trace to whoever is interested in collecting
        // game traces (i.e observing GameActionLog message)
        // Parameter:  GameTrace* pTrace: the collected game trace
        // Returns:    void
        //************************************
        void SendGameTrace(GameTrace* pTrace);

        std::map<BWAPI::Unit, BWAPI::Order> m_unitOrder;
        std::map<BWAPI::Unit, BWAPI::Unit> m_unitOrderTarget;
        std::map<BWAPI::Unit, BWAPI::Position> m_unitOrderTargetPosition;
        std::map<BWAPI::Unit, std::set<BWAPI::Unit> > m_trainerToTraineesMap;
        std::set<int> m_trainedUnits;
        std::set<int> m_playerIssuedOrderIDs;

        TID m_playerToObserveID;
        PlayerType m_playerToObserve;
        bool m_isFirstUpdate;
        ActionAbstractor m_abstractor;
    };
}
#endif // __GAMETRACECOLLETOR_H_INC__
