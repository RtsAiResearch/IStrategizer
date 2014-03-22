#include "EntityClassNearArea.h"

#include "WorldMap.h"
#include "GamePlayer.h"
#include <vector>

using namespace IStrategizer;
using namespace std;

EntityClassNearArea::EntityClassNearArea(PlayerType p_player, EntityClassType p_entityType, CellFeature* cellFeature, int howFar, int p_amount) : 
            ConditionEx(p_player, CONDEX_EntityClassNearArea)
{
    _conditionParameters[PARAM_EntityClassId] = p_entityType;
    _conditionParameters[PARAM_Distance] = howFar;
    _conditionParameters[PARAM_Amount] = p_amount;
    cellFeature->To(_conditionParameters);
}
//---------------------------------------------------------------------------
bool EntityClassNearArea::Evaluate(RtsGame& pRtsGame)
{
    vector<TID> entityIds;
    pRtsGame.Self()->Entities((EntityClassType)_conditionParameters[PARAM_EntityClassId], entityIds);
    Vector2 position = Vector2::Null();
    int counter = 0;
    ConditionEx::Evaluate(pRtsGame);
    for (size_t i = 0; i < entityIds.size(); ++i)
    {
        position = pRtsGame.Map()->GetNearestCell(new CellFeature(_conditionParameters));

        if (!position.IsNull())
        {
            if (_conditionParameters[PARAM_Amount] == DONT_CARE)
            {
                 _isSatisfied = true;
                 break;
            }
            else
            {
                counter++;
                if (counter == _conditionParameters[PARAM_Amount])
                {
                    _isSatisfied = true;
                    break;
                }
            }
        }
    }

    return _isEvaluated && _isSatisfied;
}
