#include "BwapiGame.h"
#include "BwapiTypes.h"
#include <map>
#include <BWAPI.h>

using namespace BWAPI;
using namespace IStrategizer;
using namespace std;

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
        Broodwar->getLatencyFrames());  // frames to run
}
//////////////////////////////////////////////////////////////////////////
void BwapiGame::DebugDrawUnitBuildBox(_In_ const IGameUnitType* pUnitType, _In_ Vector2 pos) const
{
    TilePosition tilePos(pos.X / TILE_SIZE, pos.Y / TILE_SIZE);
    auto type = UnitTypes::allUnitTypes()[pUnitType->GameId()];

    // Register an event that draws the target build location
    Broodwar->registerEvent([tilePos, type](Game*) {
        Broodwar->drawBoxMap(BWAPI::Position(tilePos),
            BWAPI::Position(tilePos + type.tileSize()),
            Colors::Blue); },
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

int BwapiGame::ClientVersion() const
{
    Broodwar->getRevision();
}
//////////////////////////////////////////////////////////////////////////
void BwapiGame::LastError(_Inout_ char* pTxtBuff, _In_ int buffMax) const
{
    string error = Broodwar->getLastError().toString();

    error.copy(pTxtBuff, buffMax - 1);
    pTxtBuff[buffMax - 1] = '\0';
}
//////////////////////////////////////////////////////////////////////////
bool BwapiGame::Init() const
{
    // Init unit types
    for (UnitType::set::iterator i = UnitTypes::allUnitTypes().begin();
        i != UnitTypes::allUnitTypes().end();
        ++i)
    {
        g_BwapiUnitTypes[(*i).getID()] = new BwapiUnitType(*i);
    }

    // Init tech types
    for (TechType::set::iterator i = TechTypes::allTechTypes().begin();
        i != TechTypes::allTechTypes().end();
        ++i)
    {
        g_BwapiTechTypes[(*i).getID()] = new BwapiTechType(*i);
    }

    // Init game races
    for (Race::set::iterator i = Races::allRaces().begin();
        i != Races::allRaces().end();
        ++i)
    {
        g_BwapiRaces[(*i).getID()] = new BwapiRace(*i);
    }

    // Init game upgrade types
    for (UpgradeType::set::iterator i = UpgradeTypes::allUpgradeTypes().begin();
        i != UpgradeTypes::allUpgradeTypes().end();
        ++i)
    {
        g_BwapiUpgradeTypes[(*i).getID()] = new BwapiUpgradeType(*i);
    }

}
//////////////////////////////////////////////////////////////////////////
bool BwapiGame::IsInGame() const
{
    throw std::logic_error("The method or operation is not implemented.");
}
//////////////////////////////////////////////////////////////////////////
TID BwapiGame::SelfPlayer() const
{
    throw std::logic_error("The method or operation is not implemented.");
}
//////////////////////////////////////////////////////////////////////////
TID BwapiGame::EnemyPlayer() const
{
    throw std::logic_error("The method or operation is not implemented.");
}
//////////////////////////////////////////////////////////////////////////
TID BwapiGame::NeutralPlayer() const
{
    throw std::logic_error("The method or operation is not implemented.");
}
//////////////////////////////////////////////////////////////////////////
int BwapiGame::GameFrame() const
{
    throw std::logic_error("The method or operation is not implemented.");
}
//////////////////////////////////////////////////////////////////////////
const IGameUnitType* BwapiGame::GetUnitTypeByEngineId(_In_ EntityClassType id) const
{
    throw std::logic_error("The method or operation is not implemented.");
}
//////////////////////////////////////////////////////////////////////////
const IGameTechType* BwapiGame::GetTechTypeByEngineId(_In_ ResearchType id) const
{
    throw std::logic_error("The method or operation is not implemented.");
}
//////////////////////////////////////////////////////////////////////////
const IGameUpgradeType* BwapiGame::GetUpgradeTypeByEngineId(_In_ ResearchType id) const
{
    throw std::logic_error("The method or operation is not implemented.");
}
//////////////////////////////////////////////////////////////////////////
const IGameRace* BwapiGame::GetRace(_In_ TID raceId) const
{
    throw std::logic_error("The method or operation is not implemented.");
}
//////////////////////////////////////////////////////////////////////////
const IGameTechType* BwapiGame::GetResearch(_In_ TID researchId) const
{
    throw std::logic_error("The method or operation is not implemented.");
}
//////////////////////////////////////////////////////////////////////////
GameRaceListPtr BwapiGame::GetRaces() const
{
    throw std::logic_error("The method or operation is not implemented.");
}
//////////////////////////////////////////////////////////////////////////
GameTechTypeListPtr BwapiGame::GetTechTypes() const
{
    throw std::logic_error("The method or operation is not implemented.");
}
//////////////////////////////////////////////////////////////////////////
GameUpgradeTypeListPtr BwapiGame::GetUpgradeTypes() const
{
    throw std::logic_error("The method or operation is not implemented.");
}
//////////////////////////////////////////////////////////////////////////
GameUnitTypeListPtr BwapiGame::GetUnitTypes() const
{
    throw std::logic_error("The method or operation is not implemented.");
}
//////////////////////////////////////////////////////////////////////////
void BwapiGame::DebugDrawMapLine(_In_ Vector2 p1, _In_ Vector2 p2, _In_ GameDrawColor c) const
{
    throw std::logic_error("The method or operation is not implemented.");
}
//////////////////////////////////////////////////////////////////////////
void BwapiGame::DebugDrawMapCircle(_In_ Vector2 p, _In_ int r, _In_ GameDrawColor c, _In_ bool fill /*= false*/) const
{
    throw std::logic_error("The method or operation is not implemented.");
}
//////////////////////////////////////////////////////////////////////////
void BwapiGame::DebugDrawMapText(_In_ Vector2 p, _In_ const char* pTxt) const
{
    throw std::logic_error("The method or operation is not implemented.");
}
//////////////////////////////////////////////////////////////////////////
void BwapiGame::DebugDrawMapRectangle(_In_ Vector2 topLeft, _In_ Vector2 bottomRight, _In_ GameDrawColor c, _In_ bool fill /*= false*/) const
{
    throw std::logic_error("The method or operation is not implemented.");
}
//////////////////////////////////////////////////////////////////////////
void BwapiGame::DebugDrawScreenText(_In_ Vector2 p, _In_ const char* pTxt, _In_ GameDrawColor c) const
{
    throw std::logic_error("The method or operation is not implemented.");
}
//////////////////////////////////////////////////////////////////////////
int BwapiGame::MapWidth() const
{
    throw std::logic_error("The method or operation is not implemented.");
}
//////////////////////////////////////////////////////////////////////////
int BwapiGame::MapHeight() const
{
    throw std::logic_error("The method or operation is not implemented.");
}
//////////////////////////////////////////////////////////////////////////
SmartPtr< ArrayList<Vector2> > BwapiGame::MapSpawnLocations()
{
    throw std::logic_error("The method or operation is not implemented.");
}
//////////////////////////////////////////////////////////////////////////
Vector2 BwapiGame::MapGetClosestReachableRegionCenter(_In_ TID entityId) const
{
    throw std::logic_error("The method or operation is not implemented.");
}
//////////////////////////////////////////////////////////////////////////
bool BwapiGame::MapIsExplored(_In_ Vector2 loc) const
{
    throw std::logic_error("The method or operation is not implemented.");
}
//////////////////////////////////////////////////////////////////////////
bool BwapiGame::MapIsBuildable(_In_ Vector2 loc, _In_ bool checkCanBuild) const
{
    throw std::logic_error("The method or operation is not implemented.");
}
//////////////////////////////////////////////////////////////////////////
bool BwapiGame::MapCanBuildHere(_In_ Vector2 loc, const IGameUnitType* pUnitType)
{
    throw std::logic_error("The method or operation is not implemented.");
}
//////////////////////////////////////////////////////////////////////////
int BwapiGame::MapTileSize() const
{
    throw std::logic_error("The method or operation is not implemented.");
}
//////////////////////////////////////////////////////////////////////////
GameUnitListPtr BwapiGame::MapGasFields() const
{
    throw std::logic_error("The method or operation is not implemented.");
}
//////////////////////////////////////////////////////////////////////////
bool BwapiGame::MapHasPath(_In_ Vector2 srcPos, _In_ Vector2 dstPos) const
{
    throw std::logic_error("The method or operation is not implemented.");
}
//////////////////////////////////////////////////////////////////////////
const IGameRace* BwapiGame::PlayerRace(_In_ TID playerId) const
{
    throw std::logic_error("The method or operation is not implemented.");
}
//////////////////////////////////////////////////////////////////////////
Vector2 BwapiGame::PlayerStartLocation(_In_ TID playerId) const
{
    throw std::logic_error("The method or operation is not implemented.");
}
//////////////////////////////////////////////////////////////////////////
IStrategizer::PlayerType BwapiGame::PlayerGetType(_In_ TID playerId) const
{
    throw std::logic_error("The method or operation is not implemented.");
}
//////////////////////////////////////////////////////////////////////////
int BwapiGame::PlayerMinerals(_In_ TID playerId) const
{
    throw std::logic_error("The method or operation is not implemented.");
}
//////////////////////////////////////////////////////////////////////////
int BwapiGame::PlayerGas(_In_ TID playerId) const
{
    throw std::logic_error("The method or operation is not implemented.");
}
//////////////////////////////////////////////////////////////////////////
int BwapiGame::PlayerSupplyUsed(_In_ TID playerId) const
{
    throw std::logic_error("The method or operation is not implemented.");
}
//////////////////////////////////////////////////////////////////////////
int BwapiGame::PlayerSupplyTotal(_In_ TID playerId) const
{
    throw std::logic_error("The method or operation is not implemented.");
}
//////////////////////////////////////////////////////////////////////////
bool BwapiGame::PlayerHasResearched(_In_ TID playerId, const IGameTechType* pTechType) const
{
    throw std::logic_error("The method or operation is not implemented.");
}
//////////////////////////////////////////////////////////////////////////
int BwapiGame::PlayerUpgradeLevel(_In_ TID playerId, const IGameUpgradeType* pUpgradeType) const
{
    throw std::logic_error("The method or operation is not implemented.");
}
//////////////////////////////////////////////////////////////////////////
int BwapiGame::PlayerMaxUpgradeLevel(_In_ TID playerId, const IGameUpgradeType* pUpgradeType) const
{
    throw std::logic_error("The method or operation is not implemented.");
}
//////////////////////////////////////////////////////////////////////////
bool BwapiGame::PlayerIsResearchAvailable(_In_ TID playerId, const IGameTechType* pTechType) const
{
    throw std::logic_error("The method or operation is not implemented.");
}
//////////////////////////////////////////////////////////////////////////
bool BwapiGame::PlayerIsNeutral(_In_ TID playerId) const
{
    throw std::logic_error("The method or operation is not implemented.");
}
//////////////////////////////////////////////////////////////////////////
Vector2 BwapiGame::UnitTilePosition(_In_ TID unitId) const
{
    throw std::logic_error("The method or operation is not implemented.");
}
//////////////////////////////////////////////////////////////////////////
Vector2 BwapiGame::UnitPosition(_In_ TID unitId) const
{
    throw std::logic_error("The method or operation is not implemented.");
}
//////////////////////////////////////////////////////////////////////////
Vector2 BwapiGame::UnitTopLeft(_In_ TID unitId) const
{
    throw std::logic_error("The method or operation is not implemented.");
}
//////////////////////////////////////////////////////////////////////////
Vector2 BwapiGame::UnitBottomRight(_In_ TID unitId) const
{
    throw std::logic_error("The method or operation is not implemented.");
}
//////////////////////////////////////////////////////////////////////////
const IGameUnitType* BwapiGame::UnitGetType(_In_ TID unitId) const
{
    throw std::logic_error("The method or operation is not implemented.");
}
//////////////////////////////////////////////////////////////////////////
TID BwapiGame::UnitPlayer(_In_ TID unitId) const
{
    throw std::logic_error("The method or operation is not implemented.");
}
//////////////////////////////////////////////////////////////////////////
bool BwapiGame::UnitExists(_In_ TID unitId) const
{
    throw std::logic_error("The method or operation is not implemented.");
}
//////////////////////////////////////////////////////////////////////////
bool BwapiGame::UnitIsVisible(_In_ TID unitId) const
{
    throw std::logic_error("The method or operation is not implemented.");
}
//////////////////////////////////////////////////////////////////////////
bool BwapiGame::UnitIsDetected(_In_ TID unitId) const
{
    throw std::logic_error("The method or operation is not implemented.");
}
//////////////////////////////////////////////////////////////////////////
bool BwapiGame::UnitIsStuck(_In_ TID unitId) const
{
    throw std::logic_error("The method or operation is not implemented.");
}
//////////////////////////////////////////////////////////////////////////
bool BwapiGame::UnitIsGatheringGas(_In_ TID unitId) const
{
    throw std::logic_error("The method or operation is not implemented.");
}
//////////////////////////////////////////////////////////////////////////
bool BwapiGame::UnitIsGatheringMinerals(_In_ TID unitId) const
{
    throw std::logic_error("The method or operation is not implemented.");
}
//////////////////////////////////////////////////////////////////////////
bool BwapiGame::UnitIsBeingGathered(_In_ TID unitId) const
{
    throw std::logic_error("The method or operation is not implemented.");
}
//////////////////////////////////////////////////////////////////////////
bool BwapiGame::UnitIsMoving(_In_ TID unitId) const
{
    throw std::logic_error("The method or operation is not implemented.");
}
//////////////////////////////////////////////////////////////////////////
bool BwapiGame::UnitIsAttacking(_In_ TID unitId) const
{
    throw std::logic_error("The method or operation is not implemented.");
}
//////////////////////////////////////////////////////////////////////////
bool BwapiGame::UnitIsInAttackFrame(_In_ TID unitId) const
{
    throw std::logic_error("The method or operation is not implemented.");
}
//////////////////////////////////////////////////////////////////////////
bool BwapiGame::UnitIsUnderAttack(_In_ TID unitId) const
{
    throw std::logic_error("The method or operation is not implemented.");
}
//////////////////////////////////////////////////////////////////////////
bool BwapiGame::UnitIsRepairing(_In_ TID unitId) const
{
    throw std::logic_error("The method or operation is not implemented.");
}
//////////////////////////////////////////////////////////////////////////
bool BwapiGame::UnitIsIdle(_In_ TID unitId) const
{
    throw std::logic_error("The method or operation is not implemented.");
}
//////////////////////////////////////////////////////////////////////////
bool BwapiGame::UnitIsCompleted(_In_ TID unitId) const
{
    throw std::logic_error("The method or operation is not implemented.");
}
//////////////////////////////////////////////////////////////////////////
bool BwapiGame::UnitIsBeingConstructed(_In_ TID unitId) const
{
    throw std::logic_error("The method or operation is not implemented.");
}
//////////////////////////////////////////////////////////////////////////
bool BwapiGame::UnitIsConstructing(_In_ TID unitId) const
{
    throw std::logic_error("The method or operation is not implemented.");
}
//////////////////////////////////////////////////////////////////////////
bool BwapiGame::UnitIsTargetable(_In_ TID unitId) const
{
    throw std::logic_error("The method or operation is not implemented.");
}
//////////////////////////////////////////////////////////////////////////
bool BwapiGame::UnitIsTraining(_In_ TID unitId) const
{
    throw std::logic_error("The method or operation is not implemented.");
}
//////////////////////////////////////////////////////////////////////////
bool BwapiGame::UnitIsInterruptible(_In_ TID unitId) const
{
    throw std::logic_error("The method or operation is not implemented.");
}
//////////////////////////////////////////////////////////////////////////
bool BwapiGame::UnitCanAttackUnit(_In_ TID unitId, _In_ TID targetId) const
{
    throw std::logic_error("The method or operation is not implemented.");
}
//////////////////////////////////////////////////////////////////////////
bool BwapiGame::UnitCanBuildAddOn(_In_ TID unitId, _In_ const IGameUnitType* pUnitType) const
{
    throw std::logic_error("The method or operation is not implemented.");
}
//////////////////////////////////////////////////////////////////////////
bool BwapiGame::UnitCanBuild(_In_ TID unitId, _In_ const IGameUnitType* pUnitType) const
{
    throw std::logic_error("The method or operation is not implemented.");
}
//////////////////////////////////////////////////////////////////////////
bool BwapiGame::UnitCanRepair(_In_ TID unitId, _In_ TID targetId) const
{
    throw std::logic_error("The method or operation is not implemented.");
}
//////////////////////////////////////////////////////////////////////////
bool BwapiGame::UnitCanGather(_In_ TID unitId, _In_ TID targetId) const
{
    throw std::logic_error("The method or operation is not implemented.");
}
//////////////////////////////////////////////////////////////////////////
bool BwapiGame::UnitCanTrain(_In_ TID unitId, _In_ const IGameUnitType* pUnitType) const
{
    throw std::logic_error("The method or operation is not implemented.");
}
//////////////////////////////////////////////////////////////////////////
TID BwapiGame::UnitTarget(_In_ TID unitId) const
{
    throw std::logic_error("The method or operation is not implemented.");
}
//////////////////////////////////////////////////////////////////////////
TID BwapiGame::UnitOrderTarget(_In_ TID unitId) const
{
    throw std::logic_error("The method or operation is not implemented.");
}
//////////////////////////////////////////////////////////////////////////
Vector2 BwapiGame::UnitTargetPosition(_In_ TID unitId) const
{
    throw std::logic_error("The method or operation is not implemented.");
}
//////////////////////////////////////////////////////////////////////////
Vector2 BwapiGame::UnitOrderTargetPosition(_In_ TID unitId) const
{
    throw std::logic_error("The method or operation is not implemented.");
}
//////////////////////////////////////////////////////////////////////////
int BwapiGame::UnitLastCommandFrame(_In_ TID unitId) const
{
    throw std::logic_error("The method or operation is not implemented.");
}
//////////////////////////////////////////////////////////////////////////
int BwapiGame::UnitHitpoints(_In_ TID unitId) const
{
    throw std::logic_error("The method or operation is not implemented.");
}
//////////////////////////////////////////////////////////////////////////
TID BwapiGame::UnitBuildUnit(_In_ TID unitId) const
{
    throw std::logic_error("The method or operation is not implemented.");
}
//////////////////////////////////////////////////////////////////////////
bool BwapiGame::UnitStop(_In_ TID unitId) const
{
    throw std::logic_error("The method or operation is not implemented.");
}
//////////////////////////////////////////////////////////////////////////
bool BwapiGame::UnitCancelConstruction(_In_ TID unitId) const
{
    throw std::logic_error("The method or operation is not implemented.");
}
//////////////////////////////////////////////////////////////////////////
bool BwapiGame::UnitAttack(_In_ TID unitId, _In_ TID targetId) const
{
    throw std::logic_error("The method or operation is not implemented.");
}
//////////////////////////////////////////////////////////////////////////
bool BwapiGame::UnitRepair(_In_ TID unitId, _In_ TID targetId) const
{
    throw std::logic_error("The method or operation is not implemented.");
}
//////////////////////////////////////////////////////////////////////////
bool BwapiGame::UnitGather(_In_ TID unitId, _In_ TID targetId) const
{
    throw std::logic_error("The method or operation is not implemented.");
}
//////////////////////////////////////////////////////////////////////////
bool BwapiGame::UnitAttackMove(_In_ TID unitId, _In_ Vector2 pos) const
{
    throw std::logic_error("The method or operation is not implemented.");
}
//////////////////////////////////////////////////////////////////////////
bool BwapiGame::UnitMove(_In_ TID unitId, _In_ Vector2 pos) const
{
    throw std::logic_error("The method or operation is not implemented.");
}
//////////////////////////////////////////////////////////////////////////
bool BwapiGame::UnitBuildAddon(_In_ TID unitId, _In_ const IGameUnitType* pUnitType) const
{
    throw std::logic_error("The method or operation is not implemented.");
}
//////////////////////////////////////////////////////////////////////////
bool BwapiGame::UnitBuild(_In_ TID unitId, _In_ const IGameUnitType* pUnitType, _In_ Vector2 pos) const
{
    throw std::logic_error("The method or operation is not implemented.");
}
//////////////////////////////////////////////////////////////////////////
bool BwapiGame::UnitTrain(_In_ TID unitId, _In_ const IGameUnitType* pUnitType) const
{
    throw std::logic_error("The method or operation is not implemented.");
}
//////////////////////////////////////////////////////////////////////////
bool BwapiGame::UnitResearch(_In_ TID unitId, _In_ const IGameTechType* pUnitType) const
{
    throw std::logic_error("The method or operation is not implemented.");
}
//////////////////////////////////////////////////////////////////////////
bool BwapiGame::UnitUpgrade(_In_ TID unitId, _In_ const IGameUpgradeType* pUnitType) const
{
    throw std::logic_error("The method or operation is not implemented.");
}
