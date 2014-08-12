#include "BwapiGame.h"
#include "BwapiTypes.h"
#include "DefinitionCrossMapping.h"
#include "BWAPI.h"
#include <map>

using namespace BWAPI;
using namespace IStrategizer;
using namespace std;

std::map<TID, IGameUnitType*> IStrategizer::g_BwapiUnitTypes;
std::map<TID, IGameTechType*> IStrategizer::g_BwapiTechTypes;
std::map<TID, IGameUpgradeType*> IStrategizer::g_BwapiUpgradeTypes;
std::map<TID, IGameRace*> IStrategizer::g_BwapiRaces;

BWAPI::Color BwapiColorFrom(GameDrawColor c)
{
    switch (c)
    {
    case GCLR_Red:
        return Colors::Red;
    case GCLR_Green:
        return Colors::Green;
    case GCLR_Blue:
        return Colors::Blue;
    case GCLR_Yellow:
        return Colors::Yellow;
    case GCLR_Orange:
        return Colors::Orange;
    case GCLR_Cyan:
        return Colors::Cyan;
    case GCLR_Purple:
        return Colors::Purple;
    case GCLR_White:
    default:
        return Colors::White;
    }
}
//////////////////////////////////////////////////////////////////////////
void BwapiGame::DebugDrawMapLastGameError(_In_ TID unitId) const
{
    Error lastErr = Broodwar->getLastError();
    BWAPI::Position unitPos = Broodwar->getUnit(unitId)->getPosition();

    Broodwar->registerEvent([unitPos, lastErr](Game*){ Broodwar->drawTextMap(unitPos, "%c%s", Text::Red, lastErr.c_str()); },   // action
        nullptr,    // condition
        100);  // frames to run
}
//////////////////////////////////////////////////////////////////////////
void BwapiGame::DebugDrawUnitBuildBox(_In_ const IGameUnitType* pUnitType, _In_ Vector2 pos, _In_ GameDrawColor c) const
{
    TilePosition tilePos(pos.X, pos.Y);
    auto type = ((BwapiUnitType*)pUnitType)->GetBwapiUnitType();

    // Register an event that draws the target build location
    Broodwar->registerEvent([tilePos, type, c](Game*) {
        Broodwar->drawBoxMap(BWAPI::Position(tilePos),
            BWAPI::Position(tilePos + type.tileSize()),
            BwapiColorFrom(c)); },
            nullptr,  // condition
            type.buildTime() + 100);  // frames to run
}
//////////////////////////////////////////////////////////////////////////
void BwapiGame::MapDebugDraw() const
{
    for (auto itr = Broodwar->getAllRegions().begin();
        itr != Broodwar->getAllRegions().end(); ++itr)
    {
        auto& regionR = (*itr);
        Broodwar->drawBoxMap(
            regionR->getBoundsLeft(),
            regionR->getBoundsTop(),
            regionR->getBoundsRight(),
            regionR->getBoundsBottom(),
            (regionR->isAccessible() ? BWAPI::Colors::Yellow : BWAPI::Colors::Grey),
            false);

        Broodwar->drawTextMap(regionR->getCenter(), "ID:%d RGRP-ID:%d", regionR->getID(), regionR->getRegionGroupID());

        for (auto nItr = regionR->getNeighbors().begin();
            nItr != regionR->getNeighbors().end(); ++itr)
        {
            auto& nRegionR = (*nItr);
            if (nRegionR->isAccessible())
                Broodwar->drawLineMap(regionR->getCenter(), nRegionR->getCenter(), BWAPI::Colors::Green);
        }
    }
}
//////////////////////////////////////////////////////////////////////////
int BwapiGame::ClientVersion() const
{
    return Broodwar->getRevision();
}
//////////////////////////////////////////////////////////////////////////
void BwapiGame::LastError(_Inout_ char* pTxtBuff, _In_ int buffMax) const
{
    string error = Broodwar->getLastError().toString();
    strcpy_s(pTxtBuff, buffMax, error.c_str());
}
//////////////////////////////////////////////////////////////////////////
bool BwapiGame::Init() const
{
    // Init unit types
    for (UnitType::set::iterator i = UnitTypes::allUnitTypes().begin();
        i != UnitTypes::allUnitTypes().end();
        ++i)
    {
        auto pType = new BwapiUnitType(*i);
        g_BwapiUnitTypes[(*i).getID()] = pType;
        m_pEngineIdsName[pType->EngineId()] = _strdup(pType->ToString());
    }

    // Init tech types
    for (TechType::set::iterator i = TechTypes::allTechTypes().begin();
        i != TechTypes::allTechTypes().end();
        ++i)
    {
        auto pType = new BwapiTechType(*i);
        g_BwapiTechTypes[(*i).getID()] = pType;
        m_pEngineIdsName[pType->EngineId()] = _strdup(pType->ToString());
    }

    // Init game races
    for (Race::set::iterator i = Races::allRaces().begin();
        i != Races::allRaces().end();
        ++i)
    {
        auto pType = new BwapiRace(*i);
        g_BwapiRaces[(*i).getID()] = pType;
    }

    // Init game upgrade types
    for (UpgradeType::set::iterator i = UpgradeTypes::allUpgradeTypes().begin();
        i != UpgradeTypes::allUpgradeTypes().end();
        ++i)
    {
        auto pType = new BwapiUpgradeType(*i);
        g_BwapiUpgradeTypes[(*i).getID()] = pType;
        m_pEngineIdsName[pType->EngineId()] = _strdup(pType->ToString());
    }

    return true;

}
//////////////////////////////////////////////////////////////////////////
bool BwapiGame::IsInGame() const
{
    return Broodwar->isInGame();
}
//////////////////////////////////////////////////////////////////////////
TID BwapiGame::SelfPlayer() const
{
    return Broodwar->self()->getID();
}
//////////////////////////////////////////////////////////////////////////
TID BwapiGame::EnemyPlayer() const
{
    return Broodwar->enemy()->getID();
}
//////////////////////////////////////////////////////////////////////////
TID BwapiGame::NeutralPlayer() const
{
    return Broodwar->neutral()->getID();
}
//////////////////////////////////////////////////////////////////////////
int BwapiGame::GameFrame() const
{
    return Broodwar->getFrameCount();
}
//////////////////////////////////////////////////////////////////////////
const IGameUnitType* BwapiGame::GetUnitTypeByEngineId(_In_ EntityClassType id) const
{
    return g_BwapiUnitTypes[BwapiUnitType::GetGameId(id)];
}
//////////////////////////////////////////////////////////////////////////
const IGameTechType* BwapiGame::GetTechTypeByEngineId(_In_ ResearchType id) const
{
    return g_BwapiTechTypes[BwapiTechType::GetGameId(id)];
}
//////////////////////////////////////////////////////////////////////////
const IGameUpgradeType* BwapiGame::GetUpgradeTypeByEngineId(_In_ ResearchType id) const
{
    return g_BwapiUpgradeTypes[BwapiUpgradeType::GetGameId(id)];
}
//////////////////////////////////////////////////////////////////////////
const IGameRace* BwapiGame::GetRace(_In_ TID raceId) const
{
    return g_BwapiRaces[raceId];
}
//////////////////////////////////////////////////////////////////////////
const IGameTechType* BwapiGame::GetResearch(_In_ TID researchId) const
{
    return g_BwapiTechTypes[researchId];
}
//////////////////////////////////////////////////////////////////////////
GameRaceListPtr BwapiGame::GetRaces() const
{
    auto list = GameRaceListPtr(GameRaceList::New(g_BwapiRaces.size()));
    int count = 0;

    for each(auto item in g_BwapiRaces)
    {
        list->At(count++) = item.second;
        ;
    }

    return list;
}
//////////////////////////////////////////////////////////////////////////
GameTechTypeListPtr BwapiGame::GetTechTypes() const
{
    auto list = GameTechTypeListPtr(GameTechTypeList::New(g_BwapiTechTypes.size()));
    int count = 0;

    for each(auto item in g_BwapiTechTypes)
    {
        list->At(count++) = item.second;
    }

    return list;
}
//////////////////////////////////////////////////////////////////////////
GameUpgradeTypeListPtr BwapiGame::GetUpgradeTypes() const
{
    auto list = GameUpgradeTypeListPtr(GameUpgradeTypeList::New(g_BwapiUpgradeTypes.size()));
    int count = 0;

    for each(auto item in g_BwapiUpgradeTypes)
    {
        list->At(count++) = item.second;
    }

    return list;
}
//////////////////////////////////////////////////////////////////////////
GameUnitTypeListPtr BwapiGame::GetUnitTypes() const
{
    auto list = GameUnitTypeListPtr(GameUnitTypeList::New(g_BwapiUnitTypes.size()));
    int count = 0;

    for each(auto item in g_BwapiUnitTypes)
    {
        list->At(count++) = item.second;
    }

    return list;
}
//////////////////////////////////////////////////////////////////////////
void BwapiGame::DebugDrawMapLine(_In_ Vector2 p1, _In_ Vector2 p2, _In_ GameDrawColor c) const
{
    if (p1.IsInf() || p2.IsInf())
        return;

    Broodwar->drawLineMap(p1.X, p1.Y, p2.X, p2.Y, BwapiColorFrom(c));
}
//////////////////////////////////////////////////////////////////////////
void BwapiGame::DebugDrawMapCircle(_In_ Vector2 p, _In_ int r, _In_ GameDrawColor c, _In_ bool fill /*= false*/) const
{
    if (p.IsInf() || r <= 0)
        return;

    Broodwar->drawCircleMap(p.X, p.Y, r, BwapiColorFrom(c), fill);
}
//////////////////////////////////////////////////////////////////////////
void BwapiGame::DebugDrawMapText(_In_ Vector2 p, _In_ const char* pTxt) const
{
    if (p.IsInf())
        return;

    Broodwar->drawTextMap(p.X, p.Y, pTxt);
}
//////////////////////////////////////////////////////////////////////////
void BwapiGame::DebugDrawMapRectangle(_In_ Vector2 topLeft, _In_ Vector2 bottomRight, _In_ GameDrawColor c, _In_ bool fill /*= false*/) const
{
    if (topLeft.IsInf() || bottomRight.IsInf())
        return;

    Broodwar->drawBoxMap(topLeft.X, topLeft.Y, bottomRight.X, bottomRight.Y, BwapiColorFrom(c), fill);
}
//////////////////////////////////////////////////////////////////////////
void BwapiGame::DebugDrawScreenText(_In_ Vector2 p, _In_ const char* pTxt, _In_ GameDrawColor c) const
{
    if (p.IsInf())
        return;

    Broodwar->drawTextScreen(p.X, p.Y, pTxt);
}
//////////////////////////////////////////////////////////////////////////
int BwapiGame::MapWidth() const
{
    return Broodwar->mapWidth();
}
//////////////////////////////////////////////////////////////////////////
int BwapiGame::MapHeight() const
{
    return Broodwar->mapHeight();
}
//////////////////////////////////////////////////////////////////////////
SmartPtr< ArrayList<Vector2> > BwapiGame::GetStartLocations() const
{
    auto& bwapiLocations = Broodwar->getStartLocations();
    auto locations = SmartPtr< ArrayList<Vector2> >(ArrayList<Vector2>::New(bwapiLocations.size()));
    int count = 0;

    for each (auto location in bwapiLocations)
    {
        locations->At(count).X = location.x;
        locations->At(count).Y = location.y;
        ++count;
    }

    return locations;
}
//////////////////////////////////////////////////////////////////////////
SmartPtr< ArrayList<TID> > BwapiGame::MapUnitsOnTile(_In_ Vector2 loc) const
{
    auto unitsOnTile = Broodwar->getUnitsOnTile(loc.X, loc.Y);
    auto pUnits = SmartPtr< ArrayList<TID> >(ArrayList<TID>::New(unitsOnTile.size()));
    int count = 0;

    for each (auto pUnit in unitsOnTile)
    {
        pUnits->At(count) = pUnit->getID();
        ++count;
    }

    return pUnits;
}
//////////////////////////////////////////////////////////////////////////
SmartPtr< ArrayList<TID> > BwapiGame::MapUnitsInRegion(_In_ Vector2 loc) const
{
    auto unitsInRegion = Broodwar->getRegionAt(loc.X, loc.Y)->getUnits();
    auto pUnits = SmartPtr< ArrayList<TID> >(ArrayList<TID>::New(unitsInRegion.size()));
    int count = 0;

    for each (auto pUnit in unitsInRegion)
    {
        pUnits->At(count) = pUnit->getID();
        ++count;
    }

    return pUnits;
}
//////////////////////////////////////////////////////////////////////////
Vector2 BwapiGame::MapGetClosestReachableRegionCenter(_In_ TID entityId) const
{
    auto pUnit = Broodwar->getUnit(entityId);
    if (pUnit == nullptr)
        return Vector2::Inf();
    auto regionCenter = pUnit->getRegion()->getCenter();
    return Vector2(regionCenter.x, regionCenter.y);
}
//////////////////////////////////////////////////////////////////////////
bool BwapiGame::MapIsExplored(_In_ Vector2 loc) const
{
    return Broodwar->isExplored(loc.X, loc.Y);
}
//////////////////////////////////////////////////////////////////////////
bool BwapiGame::MapIsBuildable(_In_ Vector2 loc, _In_ bool checkCanBuild) const
{
    return Broodwar->isBuildable(loc.X, loc.Y, checkCanBuild) &&
        Broodwar->getUnitsOnTile(loc.X, loc.Y).size() == 0;
}
//////////////////////////////////////////////////////////////////////////
bool BwapiGame::MapCanBuildHere(_In_ Vector2 loc, const IGameUnitType* pUnitType)
{
    TilePosition position;
    UnitType type = ((BwapiUnitType*)pUnitType)->GetBwapiUnitType();

    position.x = loc.X;
    position.y = loc.Y;

    return Broodwar->canBuildHere(position, type);
}
//////////////////////////////////////////////////////////////////////////
int BwapiGame::MapTileSize() const
{
    return TILE_SIZE;
}
//////////////////////////////////////////////////////////////////////////
GameUnitListPtr BwapiGame::MapGasFields() const
{
    auto& geysers = Broodwar->getGeysers();
    GameUnitListPtr geysersList(GameUnitList::New(geysers.size()));
    int count = 0;

    for each(auto geyser in geysers)
    {
        geysersList->At(count++) = geyser->getID();
    }

    return geysersList;
}
//////////////////////////////////////////////////////////////////////////
bool BwapiGame::MapHasPath(_In_ Vector2 srcPos, _In_ Vector2 dstPos) const
{
    Position p1(srcPos.X, srcPos.Y);
    Position p2(dstPos.X, dstPos.Y);

    return Broodwar->hasPath(p1, p2);
}
//////////////////////////////////////////////////////////////////////////
const IGameRace* BwapiGame::PlayerRace(_In_ TID playerId) const
{
    return g_BwapiRaces[Broodwar->getPlayer(playerId)->getRace().getID()];
}
//////////////////////////////////////////////////////////////////////////
Vector2 BwapiGame::PlayerStartLocation(_In_ TID playerId) const
{
    auto startPos = Broodwar->getPlayer(playerId)->getStartLocation();
    return Vector2(startPos.x, startPos.y);
}
//////////////////////////////////////////////////////////////////////////
IStrategizer::PlayerType BwapiGame::PlayerGetType(_In_ TID playerId) const
{
    auto pPlayer = Broodwar->getPlayer(playerId);

    if (Broodwar->self()->isEnemy(pPlayer))
        return PLAYER_Enemy;
    else if (Broodwar->self()->getID() == playerId)
        return PLAYER_Self;
    else
        return PLAYER_Neutral;
}
//////////////////////////////////////////////////////////////////////////
int BwapiGame::PlayerMinerals(_In_ TID playerId) const
{
    return Broodwar->getPlayer(playerId)->minerals();
}
//////////////////////////////////////////////////////////////////////////
int BwapiGame::PlayerGas(_In_ TID playerId) const
{
    return Broodwar->getPlayer(playerId)->gas();
}
//////////////////////////////////////////////////////////////////////////
int BwapiGame::PlayerSupplyUsed(_In_ TID playerId) const
{
    return Broodwar->getPlayer(playerId)->supplyUsed();
}
//////////////////////////////////////////////////////////////////////////
int BwapiGame::PlayerSupplyTotal(_In_ TID playerId) const
{
    return Broodwar->getPlayer(playerId)->supplyTotal();
}
//////////////////////////////////////////////////////////////////////////
bool BwapiGame::PlayerHasResearched(_In_ TID playerId, const IGameTechType* pTechType) const
{
    return Broodwar->getPlayer(playerId)->hasResearched(((BwapiTechType*)pTechType)->GetBwapiTechType());
}
//////////////////////////////////////////////////////////////////////////
int BwapiGame::PlayerUpgradeLevel(_In_ TID playerId, const IGameUpgradeType* pUpgradeType) const
{
    return Broodwar->getPlayer(playerId)->getUpgradeLevel(((BwapiUpgradeType*)pUpgradeType)->GetBwapiUpgradeType());
}
//////////////////////////////////////////////////////////////////////////
int BwapiGame::PlayerMaxUpgradeLevel(_In_ TID playerId, const IGameUpgradeType* pUpgradeType) const
{
    return Broodwar->getPlayer(playerId)->getMaxUpgradeLevel(((BwapiUpgradeType*)pUpgradeType)->GetBwapiUpgradeType());
}
//////////////////////////////////////////////////////////////////////////
bool BwapiGame::PlayerIsResearchAvailable(_In_ TID playerId, const IGameTechType* pTechType) const
{
    return Broodwar->getPlayer(playerId)->isResearchAvailable(((BwapiTechType*)pTechType)->GetBwapiTechType());
}
//////////////////////////////////////////////////////////////////////////
bool BwapiGame::PlayerIsNeutral(_In_ TID playerId) const
{
    return Broodwar->getPlayer(playerId)->isNeutral();
}
//////////////////////////////////////////////////////////////////////////
Vector2 BwapiGame::UnitTilePosition(_In_ TID unitId) const
{
    auto pos = Broodwar->getUnit(unitId)->getTilePosition();
    return Vector2(pos.x, pos.y);
}
//////////////////////////////////////////////////////////////////////////
Vector2 BwapiGame::UnitPosition(_In_ TID unitId) const
{
    auto pos = Broodwar->getUnit(unitId)->getPosition();
    return Vector2(pos.x, pos.y);
}
//////////////////////////////////////////////////////////////////////////
Vector2 BwapiGame::UnitTopLeft(_In_ TID unitId) const
{
    auto unit = Broodwar->getUnit(unitId);
    return Vector2(unit->getLeft(), unit->getTop());
}
//////////////////////////////////////////////////////////////////////////
Vector2 BwapiGame::UnitBottomRight(_In_ TID unitId) const
{
    auto unit = Broodwar->getUnit(unitId);
    return Vector2(unit->getRight(), unit->getBottom());
}
//////////////////////////////////////////////////////////////////////////
const IGameUnitType* BwapiGame::UnitGetType(_In_ TID unitId) const
{
    return g_BwapiUnitTypes[Broodwar->getUnit(unitId)->getType().getID()];
}
//////////////////////////////////////////////////////////////////////////
TID BwapiGame::UnitPlayer(_In_ TID unitId) const
{
    return Broodwar->getUnit(unitId)->getPlayer()->getID();
}
//////////////////////////////////////////////////////////////////////////
bool BwapiGame::UnitExists(_In_ TID unitId) const
{
    return Broodwar->getUnit(unitId)->exists();
}
//////////////////////////////////////////////////////////////////////////
bool BwapiGame::UnitIsVisible(_In_ TID unitId) const
{
    return Broodwar->getUnit(unitId)->isVisible();
}
//////////////////////////////////////////////////////////////////////////
bool BwapiGame::UnitIsDetected(_In_ TID unitId) const
{
    return Broodwar->getUnit(unitId)->isDetected();
}
//////////////////////////////////////////////////////////////////////////
bool BwapiGame::UnitIsStuck(_In_ TID unitId) const
{
    return Broodwar->getUnit(unitId)->isStuck();
}
//////////////////////////////////////////////////////////////////////////
bool BwapiGame::UnitIsGatheringGas(_In_ TID unitId) const
{
    return Broodwar->getUnit(unitId)->isGatheringGas();
}
//////////////////////////////////////////////////////////////////////////
bool BwapiGame::UnitIsGatheringMinerals(_In_ TID unitId) const
{
    return Broodwar->getUnit(unitId)->isGatheringMinerals();
}
//////////////////////////////////////////////////////////////////////////
bool BwapiGame::UnitIsBeingGathered(_In_ TID unitId) const
{
    return Broodwar->getUnit(unitId)->isBeingGathered();
}
//////////////////////////////////////////////////////////////////////////
bool BwapiGame::UnitIsMoving(_In_ TID unitId) const
{
    return Broodwar->getUnit(unitId)->isMoving();
}
//////////////////////////////////////////////////////////////////////////
bool BwapiGame::UnitIsAttacking(_In_ TID unitId) const
{
    return Broodwar->getUnit(unitId)->isAttacking();
}
//////////////////////////////////////////////////////////////////////////
bool BwapiGame::UnitIsInAttackFrame(_In_ TID unitId) const
{
    return Broodwar->getUnit(unitId)->isAttackFrame();
}
//////////////////////////////////////////////////////////////////////////
bool BwapiGame::UnitIsUnderAttack(_In_ TID unitId) const
{
    return Broodwar->getUnit(unitId)->isUnderAttack();
}
//////////////////////////////////////////////////////////////////////////
bool BwapiGame::UnitIsRepairing(_In_ TID unitId) const
{
    return Broodwar->getUnit(unitId)->isRepairing();
}
//////////////////////////////////////////////////////////////////////////
bool BwapiGame::UnitIsIdle(_In_ TID unitId) const
{
    return Broodwar->getUnit(unitId)->isIdle();
}
//////////////////////////////////////////////////////////////////////////
bool BwapiGame::UnitIsCompleted(_In_ TID unitId) const
{
    return Broodwar->getUnit(unitId)->isCompleted();
}
//////////////////////////////////////////////////////////////////////////
bool BwapiGame::UnitIsBeingConstructed(_In_ TID unitId) const
{
    return Broodwar->getUnit(unitId)->isBeingConstructed();
}
//////////////////////////////////////////////////////////////////////////
bool BwapiGame::UnitIsConstructing(_In_ TID unitId) const
{
    return Broodwar->getUnit(unitId)->isConstructing();
}
//////////////////////////////////////////////////////////////////////////
bool BwapiGame::UnitIsTargetable(_In_ TID unitId) const
{
    return Broodwar->getUnit(unitId)->isTargetable();
}
//////////////////////////////////////////////////////////////////////////
bool BwapiGame::UnitIsTraining(_In_ TID unitId) const
{
    return Broodwar->getUnit(unitId)->isTraining();
}
//////////////////////////////////////////////////////////////////////////
bool BwapiGame::UnitIsInterruptible(_In_ TID unitId) const
{
    return Broodwar->getUnit(unitId)->isInterruptible();
}
//////////////////////////////////////////////////////////////////////////
bool BwapiGame::UnitCanAttackUnit(_In_ TID unitId, _In_ TID targetId) const
{
    return Broodwar->getUnit(unitId)->canAttackUnit(Broodwar->getUnit(targetId));
}
//////////////////////////////////////////////////////////////////////////
bool BwapiGame::UnitCanBuildAddOn(_In_ TID unitId, _In_ const IGameUnitType* pUnitType) const
{
    return Broodwar->getUnit(unitId)->canBuildAddon(((BwapiUnitType*)pUnitType)->GetBwapiUnitType());
}
//////////////////////////////////////////////////////////////////////////
bool BwapiGame::UnitCanBuild(_In_ TID unitId, _In_ const IGameUnitType* pUnitType) const
{
    return Broodwar->getUnit(unitId)->canBuild(((BwapiUnitType*)pUnitType)->GetBwapiUnitType());
}
//////////////////////////////////////////////////////////////////////////
bool BwapiGame::UnitCanRepair(_In_ TID unitId, _In_ TID targetId) const
{
    return Broodwar->getUnit(unitId)->canRepair(Broodwar->getUnit(targetId));
}
//////////////////////////////////////////////////////////////////////////
bool BwapiGame::UnitCanGather(_In_ TID unitId, _In_ TID targetId) const
{
    return Broodwar->getUnit(unitId)->canGather(Broodwar->getUnit(targetId));
}
//////////////////////////////////////////////////////////////////////////
bool BwapiGame::UnitCanTrain(_In_ TID unitId, _In_ const IGameUnitType* pUnitType) const
{
    return Broodwar->getUnit(unitId)->canTrain(((BwapiUnitType*)pUnitType)->GetBwapiUnitType());
}
//////////////////////////////////////////////////////////////////////////
TID BwapiGame::UnitTarget(_In_ TID unitId) const
{
    auto pTarget = Broodwar->getUnit(unitId)->getTarget();
    if (pTarget)
        return pTarget->getID();
    else
        return INVALID_TID;
}
//////////////////////////////////////////////////////////////////////////
TID BwapiGame::UnitOrderTarget(_In_ TID unitId) const
{
    auto pTarget = Broodwar->getUnit(unitId)->getOrderTarget();
    if (pTarget)
        return pTarget->getID();
    else
        return INVALID_TID;
}
//////////////////////////////////////////////////////////////////////////
Vector2 BwapiGame::UnitTargetPosition(_In_ TID unitId) const
{
    auto pos = Broodwar->getUnit(unitId)->getTargetPosition();
    return Vector2(pos.x, pos.y);
}
//////////////////////////////////////////////////////////////////////////
Vector2 BwapiGame::UnitOrderTargetPosition(_In_ TID unitId) const
{
    auto pos = Broodwar->getUnit(unitId)->getOrderTargetPosition();
    return Vector2(pos.x, pos.y);
}
//////////////////////////////////////////////////////////////////////////
int BwapiGame::UnitLastCommandFrame(_In_ TID unitId) const
{
    return Broodwar->getUnit(unitId)->getLastCommandFrame();
}
//////////////////////////////////////////////////////////////////////////
int BwapiGame::UnitHitpoints(_In_ TID unitId) const
{
    return Broodwar->getUnit(unitId)->getHitPoints();
}
//////////////////////////////////////////////////////////////////////////
TID BwapiGame::UnitBuildUnit(_In_ TID unitId) const
{
    auto pUnit = Broodwar->getUnit(unitId)->getBuildUnit();
    if (pUnit)
        return pUnit->getID();
    else
        return INVALID_TID;
}
//////////////////////////////////////////////////////////////////////////
bool BwapiGame::UnitStop(_In_ TID unitId) const
{
    return Broodwar->getUnit(unitId)->stop();
}
//////////////////////////////////////////////////////////////////////////
bool BwapiGame::UnitCancelConstruction(_In_ TID unitId) const
{
    return Broodwar->getUnit(unitId)->cancelConstruction();
}
//////////////////////////////////////////////////////////////////////////
bool BwapiGame::UnitAttack(_In_ TID unitId, _In_ TID targetId) const
{
    return Broodwar->getUnit(unitId)->attack(Broodwar->getUnit(targetId));
}
//////////////////////////////////////////////////////////////////////////
bool BwapiGame::UnitRepair(_In_ TID unitId, _In_ TID targetId) const
{
    return Broodwar->getUnit(unitId)->repair(Broodwar->getUnit(targetId));
}
//////////////////////////////////////////////////////////////////////////
bool BwapiGame::UnitGather(_In_ TID unitId, _In_ TID targetId) const
{
    return Broodwar->getUnit(unitId)->gather(Broodwar->getUnit(targetId));
}
//////////////////////////////////////////////////////////////////////////
bool BwapiGame::UnitAttackMove(_In_ TID unitId, _In_ Vector2 pos) const
{
    return Broodwar->getUnit(unitId)->attack(Position(pos.X, pos.Y));
}
//////////////////////////////////////////////////////////////////////////
bool BwapiGame::UnitMove(_In_ TID unitId, _In_ Vector2 pos) const
{
    return Broodwar->getUnit(unitId)->move(Position(pos.X, pos.Y));
}
//////////////////////////////////////////////////////////////////////////
bool BwapiGame::UnitBuildAddon(_In_ TID unitId, _In_ const IGameUnitType* pUnitType) const
{
    return Broodwar->getUnit(unitId)->buildAddon(((BwapiUnitType*)pUnitType)->GetBwapiUnitType());
}
//////////////////////////////////////////////////////////////////////////
bool BwapiGame::UnitBuild(_In_ TID unitId, _In_ const IGameUnitType* pUnitType, _In_ Vector2 pos) const
{
    return Broodwar->getUnit(unitId)->build(((BwapiUnitType*)pUnitType)->GetBwapiUnitType(), TilePosition(pos.X, pos.Y));
}
//////////////////////////////////////////////////////////////////////////
bool BwapiGame::UnitTrain(_In_ TID unitId, _In_ const IGameUnitType* pUnitType) const
{
    return Broodwar->getUnit(unitId)->train(((BwapiUnitType*)pUnitType)->GetBwapiUnitType());
}
//////////////////////////////////////////////////////////////////////////
bool BwapiGame::UnitResearch(_In_ TID unitId, _In_ const IGameTechType* pTechType) const
{
    return Broodwar->getUnit(unitId)->research(((BwapiTechType*)pTechType)->GetBwapiTechType());
}
//////////////////////////////////////////////////////////////////////////
bool BwapiGame::UnitUpgrade(_In_ TID unitId, _In_ const IGameUpgradeType* pUpgradeType) const
{
    return Broodwar->getUnit(unitId)->upgrade(((BwapiUpgradeType*)pUpgradeType)->GetBwapiUpgradeType());
}
//////////////////////////////////////////////////////////////////////////
const IGameUnitType* BwapiGame::GetUnitTypeByName(_In_ const char* pName) const
{
    TID typeId = g_Database.EntityIdentMapping.GetBySecond(pName);
    return g_BwapiUnitTypes.at(typeId);
}
//////////////////////////////////////////////////////////////////////////
const IGameTechType* BwapiGame::GetTechTypeByName(_In_ const char* pName) const
{
    TID typeId = g_Database.TechIdentMapping.GetBySecond(pName);
    return g_BwapiTechTypes.at(typeId);
}
//////////////////////////////////////////////////////////////////////////
const IGameUpgradeType* BwapiGame::GetUpgradeTypeByName(_In_ const char* pName) const
{
    TID typeId = g_Database.UpgradeIdentMapping.GetBySecond(pName);
    return g_BwapiUpgradeTypes.at(typeId);
}
//////////////////////////////////////////////////////////////////////////
int BwapiGame::PlayerCompletedUnitCount(_In_ TID playerId, const IGameUnitType* pUnitType) const
{
    return Broodwar->getPlayer(playerId)->completedUnitCount(((BwapiUnitType*)pUnitType)->GetBwapiUnitType());
}
//////////////////////////////////////////////////////////////////////////
bool BwapiGame::UnitTargetInWeaponRage(_In_ TID unitId, _In_ TID targetId) const 
{
    return Broodwar->getUnit(unitId)->isInWeaponRange(Broodwar->getUnit(targetId));
}
//////////////////////////////////////////////////////////////////////////
bool BwapiGame::UnitCanUseTechPosition(_In_ TID unitId, _In_ const IGameTechType* pTechType, _In_ Vector2 pos) const
{
    return Broodwar->getUnit(unitId)->canUseTechPosition(((BwapiTechType*)pTechType)->GetBwapiTechType(), Position(pos.X, pos.Y));
}
//////////////////////////////////////////////////////////////////////////
bool BwapiGame::UnitUseTechPosition(_In_ TID unitId, _In_ const IGameTechType* pTechType, _In_ Vector2 pos) const
{
    return Broodwar->getUnit(unitId)->useTech(((BwapiTechType*)pTechType)->GetBwapiTechType(), Position(pos.X, pos.Y));
}