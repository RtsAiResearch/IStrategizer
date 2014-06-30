#include "MapArea.h"
#include "OccupanceDataIM.h"

using namespace IStrategizer;

MapArea& MapArea::Null()
{
    static MapArea nullptrArea(Vector2::Null(), -1, -1);

    return nullptrArea;
}
//////////////////////////////////////////////////////////////////////////
bool MapArea::IsNull()
{
    return m_pos.IsNull() &&
        m_width == -1 &&
        m_height == -1;
}
//////////////////////////////////////////////////////////////////////////
bool MapArea::Acquire()
{
    OccupanceDataIM *pIM = (OccupanceDataIM*)g_IMSysMgr.GetIM(IM_BuildingData);
    _ASSERTE(pIM);

    return pIM->ReserveArea(m_pos, m_width, m_height);
}
//////////////////////////////////////////////////////////////////////////
bool MapArea::Release()
{
    OccupanceDataIM *pIM = (OccupanceDataIM*)g_IMSysMgr.GetIM(IM_BuildingData);
    _ASSERTE(pIM);

    return pIM->FreeArea(m_pos, m_width, m_height);
}
