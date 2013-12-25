#include "GameTraceCollector.h"
#include "MessagePump.h"
#include "DataMessage.h"
#include "GameTrace.h"
#include "Logger.h"

using namespace BWAPI;
using namespace IStrategizer;

void GameTraceCollector::OnGameFrame()
{
	DataMessage<GameTrace> *pTraceMsg = nullptr;
	bool newOrder;

	for each (Unit unit in Broodwar->getAllUnits())
	{
		newOrder = false;

		// This unit is gathering resources, but his current order
		// is issued by the unit micromanagement for gathered resource OR
		// We didn't record an order for this unit before OR
		// The order recorded for that unit has changed OR
		// It is the same order but changed in Target or Target Position
		if (!IsAutoGatheringResources(unit) ||
			m_unitOrder.count(unit) == 0 ||
			m_unitOrder[unit] != unit->getOrder() ||
			m_unitOrderTarget[unit] != unit->getOrderTarget() ||
			m_unitOrderTargetPosition[unit] != unit->getOrderTargetPosition())
		{
			m_unitOrder[unit] = unit->getOrder();
			m_unitOrderTarget[unit] = unit->getOrderTarget();
			m_unitOrderTargetPosition[unit] = unit->getOrderTargetPosition();
			newOrder = true;
		}

		if (newOrder)
		{
			CollectGameTraceForUnitOrder(unit);
		}
	}
}
//////////////////////////////////////////////////////////////////////////
void IStrategizer::GameTraceCollector::CollectGameTraceForUnitOrder(const Unit unit)
{
	Order order = unit->getOrder();

	LogInfo("(P%d) %s: %s", unit->getPlayer()->getID(), unit->getType().getName().c_str(), order.c_str());
}
//////////////////////////////////////////////////////////////////////////
bool IStrategizer::GameTraceCollector::IsAutoGatheringResources(const Unit unit)
{
	bool isAutoGatheringMinerals = false;
	bool isAutoGatheringGas = false;

	// fast return if unit not expected to gather any resources
	if (!unit->getType().isWorker())
		return false;

	isAutoGatheringMinerals = unit->isGatheringMinerals() &&
		(unit->getOrder() == Orders::MoveToMinerals ||
		unit->getOrder() == Orders::MiningMinerals ||
		unit->getOrder() == Orders::WaitForMinerals ||
		unit->getOrder() == Orders::ReturnMinerals);

	isAutoGatheringGas = unit->isGatheringGas() &&
		(unit->getOrder() == Orders::MoveToGas ||
		unit->getOrder() == Orders::HarvestGas ||
		unit->getOrder() == Orders::WaitForGas ||
		unit->getOrder() == Orders::ReturnGas);

	return isAutoGatheringGas || isAutoGatheringMinerals;
}
