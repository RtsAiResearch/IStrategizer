#include "GameTraceCollector.h"
#include "MessagePump.h"
#include "DataMessage.h"
#include "GameTrace.h"
#include "Logger.h"
#include "DefinitionCrossMapping.h"

#include <Windows.h>
#include <set>

using namespace BWAPI;
using namespace IStrategizer;
using namespace std;

void GameTraceCollector::OnGameFrame()
{
    if (m_isFirstUpdate)
    {
        InitPlayerIssuedOrderTable();
        m_playerToObserve = g_Database.PlayerMapping.GetByFirst(m_playerToObserveID);

        m_isFirstUpdate = false;
    }

    Player playerToObserve = Broodwar->getPlayer(m_playerToObserveID);
    assert(playerToObserve);

    for each (Unit unit in playerToObserve->getUnits())
    {
        if (HasNewPlayerOrder(unit))
        {
            m_unitOrder[unit] = unit->getOrder();
            m_unitOrderTarget[unit] = unit->getOrderTarget();
            m_unitOrderTargetPosition[unit] = unit->getOrderTargetPosition();

            CollectGameTraceForUnitOrder(unit);
        }
        // The unit is being trained and we didn't record its training yet
        // then it seems that a train order has been issues to some unit
        // and we need to reason which unit trained that trainee
        else if (IsUnitBeingTrained(unit) &&
            m_trainedUnits.count(unit->getID()) == 0)
        {
            Unit trainer = ReasonTrainerUnitForTrainee(unit);
            CollectGameTraceForTrainedUnit(unit, trainer);
        }
    }
}
//////////////////////////////////////////////////////////////////////////
bool GameTraceCollector::IsAutoGatheringResources(const Unit unit)
{
    bool isAutoGatheringMinerals = false;
    bool isAutoGatheringGas = false;

    // fast return if unit not expected to gather any resources
    if (!unit->getType().isWorker())
        return false;

    isAutoGatheringMinerals = unit->isGatheringMinerals() &&
        (unit->getOrder() == Orders::MoveToMinerals ||
       /* unit->getOrder() == Orders::MiningMinerals ||*/
        unit->getOrder() == Orders::WaitForMinerals ||
        unit->getOrder() == Orders::ReturnMinerals);

    isAutoGatheringGas = unit->isGatheringGas() &&
        (unit->getOrder() == Orders::MoveToGas ||
        /*unit->getOrder() == Orders::HarvestGas ||*/
        unit->getOrder() == Orders::WaitForGas ||
        unit->getOrder() == Orders::ReturnGas);

    return isAutoGatheringGas || isAutoGatheringMinerals;
}
//////////////////////////////////////////////////////////////////////////
bool GameTraceCollector::IsOrderChanged(const Unit unit)
{
    // There is no order recorded for that unit, and it has a new order OR
    // There is already an order recorded for that unit and its parameters changed
    bool orderChanged = (m_unitOrder.count(unit) == 0 && unit->getOrder() != Orders::Nothing) ||
        (m_unitOrder.count(unit) != 0 &&
        (m_unitOrder[unit] != unit->getOrder() ||
        m_unitOrderTarget[unit] != unit->getOrderTarget() ||
        m_unitOrderTargetPosition[unit] != unit->getOrderTargetPosition()));

    return orderChanged;
}
//////////////////////////////////////////////////////////////////////////
bool GameTraceCollector::IsPlayerIssuedOrder(BWAPI::Order order)
{
    return m_playerIssuedOrderIDs.count(order.getID()) > 0;
}
//////////////////////////////////////////////////////////////////////////
void GameTraceCollector::InitPlayerIssuedOrderTable()
{
    m_playerIssuedOrderIDs.insert(Orders::Train.getID());
    m_playerIssuedOrderIDs.insert(Orders::BuildingLiftOff.getID());
    m_playerIssuedOrderIDs.insert(Orders::RallyPointTile.getID());
    m_playerIssuedOrderIDs.insert(Orders::RallyPointUnit.getID());

    m_playerIssuedOrderIDs.insert(Orders::MoveToMinerals.getID());
    m_playerIssuedOrderIDs.insert(Orders::MoveToGas.getID());
    m_playerIssuedOrderIDs.insert(Orders::PlaceBuilding.getID());
    m_playerIssuedOrderIDs.insert(Orders::Repair.getID());

    m_playerIssuedOrderIDs.insert(Orders::Upgrade.getID());
    m_playerIssuedOrderIDs.insert(Orders::ResearchTech.getID());
    m_playerIssuedOrderIDs.insert(Orders::BuildAddon.getID());
    m_playerIssuedOrderIDs.insert(Orders::Move.getID());
    m_playerIssuedOrderIDs.insert(Orders::Stop.getID());
    m_playerIssuedOrderIDs.insert(Orders::AttackMove.getID());
    m_playerIssuedOrderIDs.insert(Orders::AttackUnit.getID());
    m_playerIssuedOrderIDs.insert(Orders::Patrol.getID());
    m_playerIssuedOrderIDs.insert(Orders::HoldPosition.getID());

    m_playerIssuedOrderIDs.insert(Orders::HarvestGas.getID());
    m_playerIssuedOrderIDs.insert(Orders::MiningMinerals.getID());
}
//////////////////////////////////////////////////////////////////////////
bool GameTraceCollector::HasNewPlayerOrder(const Unit unit)
{
    return IsPlayerIssuedOrder(unit->getOrder()) &&
        /*!IsAutoGatheringResources(unit) &&*/
        IsOrderChanged(unit);
}
//////////////////////////////////////////////////////////////////////////
bool GameTraceCollector::IsUnitBeingTrained(const Unit unit)
{
    bool isBeingTrained = unit->isIdle() && !unit->isCompleted();

    return isBeingTrained;
}
//////////////////////////////////////////////////////////////////////////
Unit GameTraceCollector::ReasonTrainerUnitForTrainee(const Unit trainee)
{
    Player playerToObserve = Broodwar->getPlayer(m_playerToObserveID);
    assert(playerToObserve);
    Unit suspectedTrainer = nullptr;

    for each (Unit trainer in playerToObserve->getUnits())
    {
        if (trainer->isTraining())
        {
            assert(!trainer->getTrainingQueue().empty());
            // The first unit-type in the training queue is the same type of the unit
            // we are reasoning for
            if (*(trainer->getTrainingQueue().begin()) == trainee->getType())
            {
                set<Unit>& traineesForTrainer = m_trainerToTraineesMap[trainer];

                if (traineesForTrainer.empty())
                {
                    m_trainedUnits.insert(trainee->getID());
                    traineesForTrainer.insert(trainee);
                    suspectedTrainer = trainer;
                    break;
                }
                else
                {
                    bool foundAnotherUnitUnderTraining = false;

                    // Search for previously units recorded for being trained by that trainer
                    // and see if any of them is still under training or not
                    for each (Unit candidateTrainee in traineesForTrainer)
                    {
                        // There is a trainee recorded that does not exist anymore
                        // TODO: Clean the trainedUnits and trainerToTraineesMap of non-existing
                        // units to save processing cycles in the next reasoning
                        if (!playerToObserve->getUnits().exists(candidateTrainee))
                            continue;

                        if (IsUnitBeingTrained(candidateTrainee))
                        {
                            foundAnotherUnitUnderTraining = true;
                            break;
                        }
                    }

                    // Current trainer does not have another unit under training, so
                    // it must be me being trained
                    if (!foundAnotherUnitUnderTraining)
                    {
                        m_trainedUnits.insert(trainee->getID());
                        traineesForTrainer.insert(trainee);
                        suspectedTrainer = trainer;
                        break;
                    }
                }
            }
        }
    }

    return suspectedTrainer;
}
//////////////////////////////////////////////////////////////////////////
void GameTraceCollector::CollectGameTraceForUnitOrder(const Unit unit)
{
    Order order = unit->getOrder();

    // Train order has a special handling
    assert(unit->getOrder() != Orders::Train);
    ActionType action;
    
    LogInfo("(P%d,%s) %s[%d]: %s",
        unit->getPlayer()->getID(), unit->getPlayer()->getName().c_str(),
        unit->getType().getName().c_str(), unit->getID(), order.c_str());

    // We send traces only for actions recognized by the engine
    if (!g_Database.ActionMapping.TryGetByFirst(order.getID(), action))
    {
        LogWarning("Order %s will not be collected, it is not supported order", order.c_str());
        return;
    }
    
    GameTrace *pTrace = nullptr;
    PlanStepParameters actionParams = m_abstractor.GetAbstractedParameter(action, unit);

    pTrace = new GameTrace(Broodwar->getFrameCount(), action, actionParams, g_Game, m_playerToObserve);

    SendGameTrace(pTrace);
}
//////////////////////////////////////////////////////////////////////////
void GameTraceCollector::CollectGameTraceForTrainedUnit(const BWAPI::Unit trainee, const BWAPI::Unit trainer)
{
    ActionType action;

    UNREFERENCED_PARAMETER(trainee);

    LogInfo("(P%d,%s) %s[%d]: %s",
        trainer->getPlayer()->getID(), trainer->getPlayer()->getName().c_str(),
        trainer->getType().c_str(), trainer->getID(), "Train");

    assert(g_Database.ActionMapping.ContainsFirst(Orders::Train.getID()));
    action = g_Database.ActionMapping.GetByFirst(Orders::Train.getID());

    GameTrace *pTrace = nullptr;
    PlanStepParameters actionParams = m_abstractor.GetAbstractedParameter(trainee, trainer);

    pTrace = new GameTrace(Broodwar->getFrameCount(), action, actionParams, g_Game, m_playerToObserve);

    SendGameTrace(pTrace);
}
//////////////////////////////////////////////////////////////////////////
void GameTraceCollector::SendGameTrace(GameTrace* pTrace)
{
    DataMessage<GameTrace> *pTraceMsg = nullptr;

    assert(pTrace != nullptr);
    pTraceMsg = new DataMessage<GameTrace>(Broodwar->getFrameCount(), MSG_GameActionLog, pTrace);

    g_MessagePump.Send(pTraceMsg, true);
}