#include "EntityClassNearArea.h"

#include "WorldMap.h"
#include "GamePlayer.h"
#include <vector>

using namespace IStrategizer;
using namespace std;

EntityClassNearArea::EntityClassNearArea(PlayerType p_player, EntityClassType p_entityType, CellFeature* cellFeature, int howFar) : 
            ConditionEx(p_player, CONDEX_EntityClassNearArea)
{
    _conditionParameters[PARAM_EntityClassId] = p_entityType;
    _conditionParameters[PARAM_Distance] = howFar;
    cellFeature->To(_conditionParameters);
}
//---------------------------------------------------------------------------
bool EntityClassNearArea::Evaluate(RtsGame& p_RtsGame)
{
    vector<TID> entityIds;
    p_RtsGame.Self()->Entities((EntityClassType)_conditionParameters[PARAM_EntityClassId], entityIds);
    Vector2 position = Vector2::Null();
    
    ConditionEx::Evaluate(p_RtsGame);
    for (size_t i = 0; i < entityIds.size(); ++i)
    {
        position = p_RtsGame.Map()->GetNearestCell(new CellFeature(_conditionParameters));

        if (!position.IsNull())
        {
            _isSatisfied = true;
            break;
        }
    }

    return _isEvaluated && _isSatisfied;
}
