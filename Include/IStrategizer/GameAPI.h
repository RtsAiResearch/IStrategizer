//#ifndef GAMEAPI_H
//#define GAMEAPI_H
//
//#include "EngineData.h"
//#include "GameEngineCrossMapping.h"
//#include <vector>
//using namespace std;
//
//class CUnit;
//
//class GameAPI
//{
//public:
//    CheckReturn virtual int GetTileType(IN int p_xCoord, int p_yCoord, OUT TileFlagType& p_tileType) = 0;
//	CheckReturn virtual int GetTireBaseBuildingId(IN BaseType p_baseTypeId, OUT TEntity& p_id) = 0;
//    CheckReturn virtual int GetMapSize(OUT int& p_width, OUT int& p_height) = 0;
//    CheckReturn virtual int GetEntities(IN PlayerType p_playerType, IN vector<EntityClassType>& p_entityTypes, OUT vector<CUnit*>& p_entityObjects) = 0;
//    CheckReturn virtual int GetEntities(IN PlayerType p_playerType,OUT vector<CUnit*>& p_entityObjects) = 0;
//	CheckReturn virtual int GetRequiredBuildings(IN TEntity p_entityClassId, OUT vector<TEntity>& p_requiredBuildings) = 0;
//	CheckReturn virtual int GetRequiredResourcesForUpgrade(IN TUpgrade p_upgradeId, IN TCost p_costType, OUT int& p_requiredAmount) = 0;
//	CheckReturn virtual int GetRequiredResourcesForEntity(IN TEntity p_entityId, IN TCost p_costType, OUT int& p_requiredAmount) = 0;
//	CheckReturn virtual int GetRequiredResearchesForUpgrade(IN TUpgrade p_upgradeId, OUT vector<TUpgrade>& p_requiredResearches) = 0;
//	CheckReturn virtual int GetRequiredResearchesForEntity(IN TEntity p_entityId, OUT vector<TUpgrade>& p_requiredResearches) = 0;
//	CheckReturn virtual int CountEntityClassObjects(IN TPlayer p_player, IN TEntity p_entityClassId, OUT int& p_count) = 0;
//	CheckReturn virtual int GetEntityObjectAttribute(IN TPlayer p_player, IN TIdent p_objectId, IN EntityObjectAttribute p_attributeId, OUT int& p_value) = 0;
//	CheckReturn virtual int GetEntityObjectFromEntityClass(IN TPlayer p_player, IN TEntity p_classId, IN const vector<TIdent>& p_execludedIds, OUT TIdent& p_objectId) = 0;
//	CheckReturn virtual int GetEntityObject(IN TPlayer p_player, IN TIdent p_entityObjectId, OUT int& p_exists) = 0;
//	CheckReturn virtual int ResearchExist(IN TPlayer p_player, IN TUpgrade p_upgradeId, OUT int& p_exists) = 0;
//	CheckReturn virtual int GetResourceAmount(IN TPlayer p_player, IN TCost p_resourceId, OUT int& p_availableAmount) = 0;
//	CheckReturn virtual int GetCurrentPlayers(IN TPlayer p_player, OUT vector<TPlayer>& p_players) = 0;
//    CheckReturn virtual int GetPlayerBuildingsPosition(IN TPlayer p_player, OUT vector< pair<int, int> >& p_entitiesPosition) = 0;
//    CheckReturn virtual int GetPlayerUnitsPosition(IN TPlayer p_player, OUT vector< pair<int, int> >& p_entitiesPosition) = 0;
//    CheckReturn virtual int GetPlayerEntitiesCount(IN TPlayer p_player, OUT int& p_count) = 0;
//};
//
//extern GameAPI* g_GameAPI;
//
//#endif	// GAMEAPI_H