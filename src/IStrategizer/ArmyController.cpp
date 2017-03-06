#include "ArmyController.h"
#include "MessagePump.h"
#include "DataMessage.h"
#include "RtsGame.h"
#include "GamePlayer.h"
#include "GameType.h"
#include "GameEntity.h"
#include "EngineAssist.h"
#include "MathHelper.h"
#include "EntityFSM.h"
#include "ArmyFSM.h"

using namespace IStrategizer;
using namespace std;

std::unordered_map<int, ArmyGroupFormation::Data> ArmyController::sm_cachedArmySizeToFormationDataMap;

ArmyController::ArmyController(const char* pName) :
m_targetEntityId(INVALID_TID),
m_singleTargetPos(Vector2::Inf()),
m_center(Vector2::Inf()),
m_totalDiedEntities(0),
m_totalGroundAttack(0),
m_totalMaxHP(0),
m_boundingCircleRadius(0),
m_pName(pName),
m_controlBroken(false),
m_controlWorkers(false),
m_closestEntityToCenter(nullptr),
m_farthestEntityToCenter(nullptr)
{
    g_MessagePump->RegisterForMessage(MSG_EntityDestroy, this);
    g_MessagePump->RegisterForMessage(MSG_EntityRenegade, this);

    ResetLogicParams();
    m_pLogic = StackFSMPtr(new IdleArmyFSM(this));
}
//////////////////////////////////////////////////////////////////////////
void ArmyController::SetControlType(_In_ bool controlBroken, _In_ bool controlWorkers)
{
    m_controlBroken = controlBroken;
    m_controlWorkers = controlWorkers;
}
//////////////////////////////////////////////////////////////////////////
void ArmyController::TryControlArmy(_In_ bool fromCleanSlate)
{
    // For now, army controls only current free healthy attackers
    for (auto& entityR : g_Game->Self()->Entities())
    {
        (void)TryControlEntity(entityR.first);
    }
}
//////////////////////////////////////////////////////////////////////////
void ArmyController::ReleaseArmy()
{
    while (!m_entities.empty())
    {
        ReleaseEntity(m_entities.begin()->first);
    }
}
//////////////////////////////////////////////////////////////////////////
void ArmyController::NotifyMessegeSent(_In_ Message* pMsg)
{
    if (pMsg->TypeId() == MSG_EntityDestroy)
    {
        EntityDestroyMessage* pDestroyMsg = static_cast<EntityDestroyMessage*>(pMsg);
        _ASSERTE(pDestroyMsg && pDestroyMsg->Data());

        if (pDestroyMsg->Data()->OwnerId == PLAYER_Self)
        {
            OnEntityLost(pDestroyMsg->Data()->EntityId);
        }
    }
    else if (pMsg->TypeId() == MSG_EntityRenegade)
    {
        EntityDestroyMessage* pDestroyMsg = static_cast<EntityDestroyMessage*>(pMsg);
        _ASSERTE(pDestroyMsg && pDestroyMsg->Data());

        // One of my units changed control to another player
        if (pDestroyMsg->Data()->OwnerId != PLAYER_Self &&
            m_entities.count(pDestroyMsg->Data()->EntityId) > 0)
        {
            OnEntityLost(pDestroyMsg->Data()->EntityId);
        }
    }
}
//////////////////////////////////////////////////////////////////////////
ArmyGroup ArmyController::Classify(const GameType* pType)
{
    if (pType->P(TP_IsWorker))
        return ARMGRP_Worker;
    if (!pType->P(TP_IsAttacker))
        return ARMGRP_Supporter;
    else if (pType->P(TP_IsMelee))
        return ARMGRP_MeleelAttacker;
    else
        return ARMGRP_RangedAttacker;
}
//////////////////////////////////////////////////////////////////////////
bool ArmyController::HasType(EntityClassType type)
{
    for (auto& entityR : m_entities)
    {
        if (entityR.second->Entity()->TypeId() == type)
            return true;
    }
    return false;
}
//////////////////////////////////////////////////////////////////////////
void ArmyController::ReleaseEntity(_In_ TID entityId)
{
    if (m_entities.count(entityId) > 0)
    {
        auto pCtrlr = m_entities[entityId];
        auto pEntityType = g_Game->GetEntityType(pCtrlr->TypeId());

        m_totalGroundAttack -= pEntityType->P(TP_GroundAttack);
        _ASSERTE(m_totalGroundAttack >= 0);
        m_totalMaxHP -= pEntityType->P(TP_MaxHp);
        _ASSERTE(m_totalMaxHP >= 0);

        CalcGroupFormationData();

        auto pEntity = g_Game->Self()->GetEntity(entityId);
        if (pEntity)
            LogInfo("Release %s from %s", pEntity->ToString().c_str(), ToString().c_str());
        else
            LogInfo("Released non-existing entity %d from %s, to control %d entities", entityId, ToString().c_str(), m_entities.size());

        pCtrlr->ReleaseEntity();
        m_entities.erase(entityId);

        if (m_entities.empty())
        {
            ResetCache();
        }
    }
}
//////////////////////////////////////////////////////////////////////////
void ArmyController::Update()
{
    if (!IsControllingArmy())
        return;

    CalcCetner();
    CalcBoundries();

    // Workers need the auto repair data, check for them
    if (m_controlWorkers)
        CalcDamagedRepairablesNearby();

    // Only aggressive logic needs enemy data, otherwise
    // peaceful ones don't target enemies intentionally
    // eg. workers army or broken army
    if (m_pLogic->TypeId() != StandArmyFSM::TypeID &&
        m_pLogic->TypeId() != IdleArmyFSM::TypeID)
        CalcEnemyData();

    m_pLogic->Update();

    for (auto& entityR : m_entities)
    {
        auto pEntityCtrlr = entityR.second;
        pEntityCtrlr->Update();
    }

    for (auto fleeingId : m_currFramefleeingEntities)
    {
        ReleaseEntity(fleeingId);
    }

    m_currFramefleeingEntities.clear();
}
//////////////////////////////////////////////////////////////////////////
void ArmyController::Defend(_In_ Vector2 pos)
{
    _ASSERTE(!pos.IsInf());
    LogInfo("%s -> Defend(%s)", ToString().c_str(), pos.ToString().c_str());

    ResetLogicParams();

    // Set the logic input position to defend
    TargetPosition(pos);

    m_pLogic = StackFSMPtr(new GuardArmyFSM(this));
}
//////////////////////////////////////////////////////////////////////////
void ArmyController::Attack(_In_ Vector2 pos)
{
    _ASSERTE(!pos.IsInf());
    LogInfo("%s -> Attack(%s)", ToString().c_str(), pos.ToString().c_str());

    ResetLogicParams();

    // Set the logic input position to attack at
    TargetPosition(pos);

    m_pLogic = StackFSMPtr(new AttackMoveArmyFSM(this));
}
//////////////////////////////////////////////////////////////////////////
void ArmyController::Stand(_In_ Vector2 pos)
{
    _ASSERTE(!pos.IsInf());
    LogInfo("%s -> Stand(%s)", ToString().c_str(), pos.ToString().c_str());

    ResetLogicParams();

    // Set the logic input position to stand at
    TargetPosition(pos);

    m_pLogic = StackFSMPtr(new StandArmyFSM(this));
}
//////////////////////////////////////////////////////////////////////////
void ArmyController::CalcCetner()
{
    if (m_entities.empty())
    {
        m_center = Vector2::Inf();
        return;
    }

    m_center = Vector2::Zero();

    for (auto& entityR : m_entities)
    {
        m_center += entityR.second->Entity()->Position();
    }

    m_center /= (int)m_entities.size();
}
//////////////////////////////////////////////////////////////////////////
void ArmyController::CalcBoundries()
{
    m_boundingCircleRadius = 0;
    m_closestEntityToCenter = nullptr;

    if (m_entities.empty())
        return;

    int maxDist = INT_MIN;
    int minDist = INT_MAX;

    for (auto& entityR : m_entities)
    {
        int dist = m_center.Distance(entityR.second->Entity()->Position());

        if (dist > maxDist)
        {
            maxDist = dist;
            m_farthestEntityToCenter = entityR.second->Entity();
        }

        if (dist < minDist)
        {
            minDist = dist;
            m_closestEntityToCenter = entityR.second->Entity();
        }
    }

    m_boundingCircleRadius = maxDist;
}
//////////////////////////////////////////////////////////////////////////
void ArmyController::CalcEnemyData()
{
    Vector2 selfPos = Center();
    Vector2 otherPos = Vector2::Inf();

    m_closestEnemy.clear();
    m_enemyData.clear();

    for (auto& entityR : g_Game->Enemy()->Entities())
    {
        if (entityR.second->Exists() &&
            entityR.second->P(OP_IsTargetable))
        {
            otherPos = entityR.second->Position();
            int dist = selfPos.Distance(otherPos);
            m_closestEnemy.insert(make_pair(dist, entityR.first));

            auto& dat = m_enemyData[entityR.first];
            dat.E = entityR.second;
            dat.DistanceToCenter = dist;
            dat.TargetEntityId = entityR.second->TargetId();

            if (m_entities.count(dat.TargetEntityId))
                dat.IsAttackingArmy = true;

            for (auto& selfEntityR : m_entities)
            {
                if (selfEntityR.second->Entity()->IsTargetInWeaponRage(entityR.first))
                {
                    dat.IsInApproxRange = true;
                    break;
                }
            }
        }
    }

    // No enemy close to me
    if (m_closestEnemy.empty())
    {
        m_targetEntityId = INVALID_TID;
    }
    // Apply a smart target selection algorithm
    else
    {
        ChooseArmyTarget();
    }
}
//////////////////////////////////////////////////////////////////////////
void ArmyController::ChooseArmyTarget()
{
    _ASSERTE(!m_enemyData.empty());

    multimap<int, GameEntity*> penalitiesMap;

    for (auto& enemyR : m_enemyData)
    {
        int penalty = AssignPenaltyToEnemyEntity(enemyR.second);
        enemyR.second.SelectionPenalty = penalty;
        penalitiesMap.insert(make_pair(penalty, enemyR.second.E));
    }

    if (m_targetEntityId != penalitiesMap.begin()->second->Id())
    {
        m_targetEntityId = penalitiesMap.begin()->second->Id();
        LogInfo("%s chosen a new enemy target %s", ToString().c_str(), g_Game->Enemy()->GetEntity(m_targetEntityId)->ToString(true).c_str());
    }
}
//////////////////////////////////////////////////////////////////////////
int ArmyController::AssignPenaltyToEnemyEntity(_In_ const ArmyEnemyData& dat)
{
    // Hard coded decision tree
    // TODO: Convert the rules to an ID3 or C3 or similar ones

    if (dat.IsInApproxRange)
    {
        if (dat.E->Type()->P(TP_IsSupporter))
        {
            float hpPrcnt = dat.E->HitpointsPercentage();

            if (hpPrcnt < EntityController::CriticalHpPercent)
                return 1;
            else if (hpPrcnt < EntityController::HealthyHpPercent)
                return 2;
            else
                return 3;
        }
        else
        {
            if (dat.IsAttackingArmy)
            {
                if (dat.E->Type()->P(TP_IsBuilding))
                {
                    float hpPrcnt = dat.E->HitpointsPercentage();

                    if (hpPrcnt < EntityController::CriticalHpPercent)
                        return 4;
                    else if (hpPrcnt < EntityController::HealthyHpPercent)
                        return 5;
                    else
                        return 6;
                }
                else
                {
                    float hpPrcnt = dat.E->HitpointsPercentage();

                    if (hpPrcnt < EntityController::CriticalHpPercent)
                        return 7;
                    else if (hpPrcnt < EntityController::HealthyHpPercent)
                        return 8;
                    else
                        return 9;
                }
            }
            else
            {
                if (dat.E->Type()->P(TP_IsAttacker))
                {
                    float hpPrcnt = dat.E->HitpointsPercentage();

                    if (hpPrcnt < EntityController::CriticalHpPercent)
                        return 10;
                    else if (hpPrcnt < EntityController::HealthyHpPercent)
                        return 11;
                    else
                        return 12;
                }
                else
                {
                    if (dat.E->Type()->P(TP_IsWorker))
                    {
                        float hpPrcnt = dat.E->HitpointsPercentage();

                        if (hpPrcnt < EntityController::CriticalHpPercent)
                            return 13;
                        else if (hpPrcnt < EntityController::HealthyHpPercent)
                            return 14;
                        else
                            return 15;
                    }
                    else
                    {
                        float hpPrcnt = dat.E->HitpointsPercentage();

                        if (hpPrcnt < EntityController::CriticalHpPercent)
                            return 16;
                        else if (hpPrcnt < EntityController::HealthyHpPercent)
                            return 17;
                        else
                            return 18;
                    }
                }
            }
        }
    }
    else
    {
        return 19;
    }
}
//////////////////////////////////////////////////////////////////////////
bool ArmyController::IsInOrder(_In_ Vector2 pos, _In_ float orderPrcnt)
{
    if (m_entities.empty())
    {
        return false;
    }

    _ASSERTE(orderPrcnt <= 1.0f && orderPrcnt > 0.0f);

    ArmyGroupFormation::Data data = CalcGroupFormationData((int)m_entities.size());
    Circle2 orderArea(pos, data.CircleRadius);
    int numInOrder = 0;
    for (auto& entityR : m_entities)
    {
        // Fast return as soon as we see astray units out of focus area
        if (orderArea.IsInside(entityR.second->Entity()->Position()))
        {
            ++numInOrder;
        }
    }

    return ((float)numInOrder / (float)m_entities.size()) >= orderPrcnt;
}
//////////////////////////////////////////////////////////////////////////
bool ArmyController::CanControl(_In_ const GameEntity* pEntity)
{
    _ASSERTE(pEntity != nullptr);

    auto pEntityType = pEntity->Type();

    bool isBroken = !EntityController::IsHpAboveThreshold(pEntity, EntityController::CriticalHpPercent);

    return m_entities.count(pEntity->Id()) == 0 &&
        pEntity->Exists() &&
        !pEntityType->P(TP_IsBuilding) &&
        pEntity->P(OP_State) != OBJSTATE_BeingConstructed &&
        !pEntity->IsLocked() &&
        ((m_controlWorkers && pEntityType->P(TP_IsWorker)) || (!m_controlWorkers && !pEntityType->P(TP_IsWorker))) &&
        ((m_controlBroken && isBroken) || (!m_controlBroken && !isBroken)) &&
        pEntityType->P(TP_CanMove);
}
//////////////////////////////////////////////////////////////////////////
bool ArmyController::TryControlEntity(_In_ TID entityId)
{
    auto pEntity = g_Game->Self()->GetEntity(entityId);

    if (CanControl(pEntity))
    {
        auto pController = EntityControllerPtr(new EntityController(this));

        // We can't reach here if we already control the entity
        // All controlled entities are locked by us
        _ASSERTE(m_entities.count(entityId) == 0);

        m_entities[entityId] = pController;
        pController->ControlEntity(entityId);

        m_totalGroundAttack += pEntity->Type()->P(TP_GroundAttack);
        m_totalMaxHP += pEntity->Type()->P(TP_MaxHp);

        CalcGroupFormationData();

        LogInfo("%s Control->(%s). Controlling %d entities", ToString().c_str(), pEntity->ToString().c_str(), m_entities.size());

        return true;
    }

    return false;
}
//////////////////////////////////////////////////////////////////////////
void ArmyController::ReleaseHealthyEntities()
{
    if (!IsControllingArmy())
        return;

    for (auto entityItr = m_entities.begin(); entityItr != m_entities.end();)
    {
        if (entityItr->second->IsOnHealthyHP())
        {
            auto oldItr = entityItr;
            ++entityItr;
            ReleaseEntity(oldItr->first);
        }
        else
            ++entityItr;
    }
}
//////////////////////////////////////////////////////////////////////////
void ArmyController::OnEntityLost(_In_ TID entityId)
{
    ReleaseEntity(entityId);
    ++m_totalDiedEntities;
}
//////////////////////////////////////////////////////////////////////////
void ArmyController::OnEntityFleeing(_In_ TID entityId)
{
    m_currFramefleeingEntities.insert(entityId);
}
//////////////////////////////////////////////////////////////////////////
void ArmyController::CalcGroupFormationData()
{
    m_formationData = CalcGroupFormationData((int)m_entities.size());
}
//////////////////////////////////////////////////////////////////////////
ArmyGroupFormation::Data ArmyController::CalcGroupFormationData(_In_ int groupSize)
{
    if (sm_cachedArmySizeToFormationDataMap.count(groupSize) > 0)
        return sm_cachedArmySizeToFormationDataMap.at(groupSize);

    static const float sqrt2 = sqrtf(2.0);

    ArmyGroupFormation::Data data;

    if (groupSize == 0)
    {
        data.CircleRadius = 0;
        data.SquareSide = 0;
    }
    else
    {
        // minimum nxn square formation side with 0 spacing
        data.MinSquareSide = (int)ceilf(sqrtf((float)groupSize));

        data.SquareSide = int(float(data.MinSquareSide) * float(FormationSpacing));
        // +FormationSpacing to add extra padding around the Square so that units placed
        // on Square edge have less chance of being outside the closing circle when
        // the game fail to place the unit exactly on the requested tile
        data.CircleRadius = (int)ceilf(sqrt2 * ((float(data.SquareSide) * 0.5f) + float(FormationSpacing)));
    }

    sm_cachedArmySizeToFormationDataMap[groupSize] = data;

    return data;
}
//////////////////////////////////////////////////////////////////////////
void ArmyController::CalcGroupFormation(_Inout_ ArmyGroupFormation& formation)
{
    // PLACEMENT
    Vector2 topLeft = formation.Center;
    topLeft -= (m_formationData.SquareSide / 2);

    auto entityItr = m_entities.begin();

    // Fill the formation shape with units until we are out of them
    for (int y = 0; y < m_formationData.MinSquareSide && entityItr != m_entities.end(); ++y)
    {
        for (int x = 0; x < m_formationData.MinSquareSide && entityItr != m_entities.end(); ++x)
        {
            Vector2& pos = formation.Placement[entityItr->first];
            pos.X = topLeft.X + (x * FormationSpacing);
            pos.Y = topLeft.Y + (y * FormationSpacing);
            ++entityItr;
        }
    }
}
//////////////////////////////////////////////////////////////////////////
bool ArmyController::IsAnyEnemyInFormationAreaSight(_In_ Vector2 areaPos) const
{
    auto sightArea = Circle2(areaPos, m_formationData.CircleRadius + SightAreaRadius);

    if (m_closestEnemy.empty())
        return false;
    else
        return m_closestEnemy.begin()->first < m_formationData.CircleRadius + SightAreaRadius;
}
//////////////////////////////////////////////////////////////////////////
void ArmyController::CalcDamagedRepairablesNearby()
{
    m_damagedRepairablesNearby.clear();

    auto sightArea = SightArea();

    for (auto& entityR : g_Game->Self()->Entities())
    {
        if (sightArea.IsInside(entityR.second->Position()) &&
            EntityController::IsDamaged(entityR.second))
            m_damagedRepairablesNearby.insert(entityR.first);
    }
}
//////////////////////////////////////////////////////////////////////////
TID ArmyController::ChooseRepairTarget(_In_ const GameEntity* pEntity)
{
    unordered_map<TID, TID> damagedToRepairerMap;

    for (auto& entityR : m_entities)
    {
        auto pEntity = entityR.second->Entity();

        if (pEntity->P(OP_IsRepairing))
        {
            damagedToRepairerMap[pEntity->TargetId()] = pEntity->Id();
        }
    }

    for (auto entityId : DamagedRepairablesNearby())
    {
        if (damagedToRepairerMap.count(entityId) == 0 &&
            pEntity->CanRepair(entityId))
        {
            auto pTarget = g_Game->Self()->GetEntity(entityId);

            if (!pTarget->P(OP_IsRepairing) && !pTarget->P(OP_IsMoving))
                return entityId;
        }
    }

    return INVALID_TID;
}
//////////////////////////////////////////////////////////////////////////
void ArmyController::ResetLogicParams()
{
    // Parameters
    m_targetEntityId = INVALID_TID;
    m_singleTargetPos = Vector2::Inf();
}
//////////////////////////////////////////////////////////////////////////
void ArmyController::ResetCache()
{
    m_enemyData.clear();
    m_closestEnemy.clear();
    m_formationData = ArmyGroupFormation::Data();
    m_damagedRepairablesNearby.clear();
    m_boundingCircleRadius = 0;
    m_center = Vector2::Inf();
    m_closestEntityToCenter = nullptr;
    m_farthestEntityToCenter = nullptr;
}