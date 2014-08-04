#include "EntityClassNearArea.h"

#include "WorldMap.h"
#include "GamePlayer.h"
#include <vector>

using namespace IStrategizer;
using namespace std;

EntityClassNearArea::EntityClassNearArea(PlayerType p_player, EntityClassType p_entityType, CellFeature* cellFeature, int howFar, int p_amount) : 
            ConditionEx(p_player, CONDEX_EntityClassNearArea)
{
    m_params[PARAM_EntityClassId] = p_entityType;
    m_params[PARAM_Distance] = howFar;
    m_params[PARAM_Amount] = p_amount;
    cellFeature->To(m_params);
}
//---------------------------------------------------------------------------
bool EntityClassNearArea::Evaluate(RtsGame& game)
{
    EntityList entityIds;
    game.Self()->Entities((EntityClassType)m_params[PARAM_EntityClassId], entityIds);
    Vector2 position = Vector2(-1, -1);
    int counter = 0;
    ConditionEx::Evaluate(game);
    for (unsigned i = 0; i < entityIds.size(); ++i)
    {
        position = game.Map()->GetNearestCell(new CellFeature(m_params));

        if (position.X == -1 && position.Y == -1)
        {
            if (m_params[PARAM_Amount] == DONT_CARE)
            {
                 _isSatisfied = true;
                 break;
            }
            else
            {
                counter++;
                if (counter == m_params[PARAM_Amount])
                {
                    _isSatisfied = true;
                    break;
                }
            }
        }
    }

    return _isEvaluated && _isSatisfied;
}
