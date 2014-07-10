#include "GameTraceCollector.h"
#include "MessagePump.h"
#include "DataMessage.h"
#include "GameTrace.h"
#include "Logger.h"
#include "DefinitionCrossMapping.h"
#include "RtsGame.h"

#include <set>
#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

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
    _ASSERTE(playerToObserve);

    for (Unit unit : playerToObserve->getUnits())
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
        // handling an addon is special as well.
        else if (unit->getType().isAddon() && unit->isBeingConstructed() &&
            m_constructedAddons.count(unit->getID()) == 0)
        {
            m_constructedAddons.insert(unit->getID());
            CollectGameTraceForBuildAddon(unit);
        }
    }
}
//////////////////////////////////////////////////////////////////////////
bool GameTraceCollector::StartedGathering(const Unit unit)
{
    return StartedGatheringMinerals(unit) || StartedGatheringGas(unit);
}
//////////////////////////////////////////////////////////////////////////
bool GameTraceCollector::StartedGatheringGas(const Unit unit)
{
    if (m_gasGatherers.count(unit->getID()) == 0 && IsAutoGatheringGas(unit))
    {
        m_gasGatherers.insert(unit->getID());
        return true;
    }

    return false;
}
//////////////////////////////////////////////////////////////////////////
bool GameTraceCollector::StartedGatheringMinerals(const Unit unit)
{
    if (m_mineralsGatherers.count(unit->getID()) == 0 && IsAutoGatheringMinerals(unit))
    {
        m_mineralsGatherers.insert(unit->getID());
        return true;
    }

    return false;
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
    // Do not handle building addons by the order, it's special handling.
    //m_playerIssuedOrderIDs.insert(Orders::BuildAddon.getID());
    //m_playerIssuedOrderIDs.insert(Orders::PlaceAddon.getID());
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
    // if the unit had an attack trace before or
    // had gather action and now its still gathering,
    // don't include this order.
    if (m_attackers.count(unit->getID()) != 0 || 
        (m_mineralsGatherers.count(unit->getID()) != 0 && IsAutoGatheringMinerals(unit)) ||
        (m_gasGatherers.count(unit->getID()) != 0 && IsAutoGatheringGas(unit)))
    {
        _ASSERTE(m_attackers.count(unit->getID()) == 1 || 
                m_mineralsGatherers.count(unit->getID()) == 1 ||
                m_gasGatherers.count(unit->getID()) == 1);
        return false;
    }

    return StartedGathering(unit) || 
           StartedAttacking(unit) || 
           (IsPlayerIssuedOrder(unit->getOrder()) && IsOrderChanged(unit));
}
//////////////////////////////////////////////////////////////////////////
bool GameTraceCollector::IsUnitBeingTrained(const Unit unit) const
{
    return unit->isIdle() && !unit->isCompleted();
}
//////////////////////////////////////////////////////////////////////////
Unit GameTraceCollector::ReasonTrainerUnitForTrainee(const Unit trainee)
{
    Player playerToObserve = Broodwar->getPlayer(m_playerToObserveID);
    _ASSERTE(playerToObserve);
    Unit suspectedTrainer = nullptr;

    for (Unit trainer : playerToObserve->getUnits())
    {
        if (trainer->isTraining())
        {
            _ASSERTE(!trainer->getTrainingQueue().empty());
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
                    for (Unit candidateTrainee : traineesForTrainer)
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
    _ASSERTE(unit->getOrder() != Orders::Train);
    _ASSERTE(unit->getOrder() != Orders::PlaceAddon);
    ActionType action;
    
    LogInfo("(P%d,%s) %s[%d]: %s",
        unit->getPlayer()->getID(), unit->getPlayer()->getName().c_str(),
        unit->getType().getName().c_str(), unit->getID(), order.c_str());

    // We send traces only for actions recognized by the engine
    if (!g_Database.ActionMapping.TryGetByFirst(order.getID(), action))
    {
        LogInfo("Order %s will not be collected, it is not supported order", order.c_str());
        return;
    }
    else
    {
        LogInfo("Order %s will be collected", order.c_str());
    }
    
    GameTrace *pTrace = nullptr;
    PlanStepParameters actionParams = m_abstractor.GetAbstractedParameter(action, unit);

    pTrace = new GameTrace(Broodwar->getFrameCount(), action, actionParams, g_Game->Snapshot(), m_playerToObserve);

    if (action == ACTIONEX_AttackEntity || action == ACTIONEX_AttackGround)
    {
        LogInfo("test");
    }

    SendGameTrace(pTrace);
}
//////////////////////////////////////////////////////////////////////////
void GameTraceCollector::CollectGameTraceForTrainedUnit(const BWAPI::Unit trainee, const BWAPI::Unit trainer)
{
    if (!trainer)
    {
        LogInfo("Unable to find trainee %s trainer, will use normal trace collector", trainee->getPlayer()->getName().c_str());
        CollectGameTraceForUnitOrder(trainee);
        return;
    }

    ActionType action;

    UNREFERENCED_PARAMETER(trainee);

    LogInfo("(P%d,%s) %s[%d]: %s",
        trainer->getPlayer()->getID(), trainer->getPlayer()->getName().c_str(),
        trainer->getType().c_str(), trainer->getID(), "Train");

    _ASSERTE(g_Database.ActionMapping.ContainsFirst(Orders::Train.getID()));
    action = g_Database.ActionMapping.GetByFirst(Orders::Train.getID());

    GameTrace *pTrace = nullptr;
    PlanStepParameters actionParams = m_abstractor.GetAbstractedParameter(trainee, trainer);

    pTrace = new GameTrace(Broodwar->getFrameCount(), action, actionParams, g_Game->Snapshot(), m_playerToObserve);

    SendGameTrace(pTrace);
}
//////////////////////////////////////////////////////////////////////////
void GameTraceCollector::SendGameTrace(GameTrace* pTrace) const
{
    DataMessage<GameTrace> *pTraceMsg = nullptr;

    _ASSERTE(pTrace != nullptr);
    pTraceMsg = new DataMessage<GameTrace>(Broodwar->getFrameCount(), MSG_GameActionLog, pTrace);

    g_MessagePump->Send(pTraceMsg, true);
}
//////////////////////////////////////////////////////////////////////////
bool GameTraceCollector::IsAutoGatheringGas(const Unit unit)
{
    bool isAutoGatheringGas = false;

    // fast return if unit not expected to gather any resources
    if (!unit->getType().isWorker())
        return false;

    isAutoGatheringGas = (unit->getOrder() == Orders::MoveToGas ||
        unit->getOrder() == Orders::HarvestGas ||
        unit->getOrder() == Orders::WaitForGas ||
        unit->getOrder() == Orders::ReturnGas);

    return isAutoGatheringGas;
}
//////////////////////////////////////////////////////////////////////////
bool GameTraceCollector::IsAutoGathering(const Unit unit)
{
    return IsAutoGatheringMinerals(unit) || IsAutoGatheringGas(unit);
}
//////////////////////////////////////////////////////////////////////////
bool GameTraceCollector::IsAutoGatheringMinerals(const Unit unit)
{
    bool isAutoGatheringMinerals = false;

    // fast return if unit not expected to gather any resources
    if (!unit->getType().isWorker())
        return false;

    isAutoGatheringMinerals = (unit->getOrder() == Orders::MoveToMinerals ||
        unit->getOrder() == Orders::MiningMinerals ||
        unit->getOrder() == Orders::WaitForMinerals ||
        unit->getOrder() == Orders::ReturnMinerals);

    return isAutoGatheringMinerals;
}
//////////////////////////////////////////////////////////////////////////
void GameTraceCollector::CollectGameTraceForBuildAddon(const Unit unit)
{
    Order order = unit->getOrder();
    ActionType action;

    // PlaceAddon order has a special handling
    _ASSERTE(unit->isBeingConstructed());

    LogInfo("(P%d,%s) %s[%d]: %s",
        unit->getPlayer()->getID(), unit->getPlayer()->getName().c_str(),
        unit->getType().getName().c_str(), unit->getID(), order.c_str());
    LogInfo("Order PlaceAddon for %s will be collected", unit->getType().getName().c_str());
    
    _ASSERTE(g_Database.ActionMapping.ContainsFirst(Orders::PlaceAddon.getID()));
    action = g_Database.ActionMapping.GetByFirst(Orders::PlaceAddon.getID());

    PlanStepParameters actionParams = m_abstractor.GetAbstractedParameter(action, unit);
    GameTrace *pTrace = new GameTrace(Broodwar->getFrameCount(), action, actionParams, g_Game->Snapshot(), m_playerToObserve);

    SendGameTrace(pTrace);
}
//////////////////////////////////////////////////////////////////////////
bool GameTraceCollector::StartedAttacking(const Unit unit)
{
    if (m_attackers.count(unit->getID()) == 0 &&
        (unit->getOrder() == Orders::AttackUnit || unit->getOrder() == Orders::AttackTile || unit->getOrder() == Orders::AttackMove))
    {
        m_attackers.insert(unit->getID());
        return true;
    }

    return false;
}
