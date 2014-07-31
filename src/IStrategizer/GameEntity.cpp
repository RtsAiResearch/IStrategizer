#include "GameEntity.h"
#include "RtsGame.h"
#include "GameType.h"

using namespace IStrategizer;

unsigned GameEntity::Distance(_In_ const GameEntity *pOther) const
{
    int dX = P(OP_PosCenterX) - pOther->P(OP_PosCenterX);
    int dY = P(OP_PosCenterY) - pOther->P(OP_PosCenterY);
    return (unsigned)sqrt((float)(unsigned)((dX * dX) + (dY * dY)));
}
//////////////////////////////////////////////////////////////////////////
const GameType* GameEntity::Type() const
{
    return g_Game->GetEntityType(TypeId());
}
//////////////////////////////////////////////////////////////////////////
void GameEntity::CacheAttributes()
{
    if ((int)g_Game->GameFrame() > m_cacheFrame)
    {
        LogDebugInfo("Caching %s attributes", ToString().c_str());

        m_cacheFrame = g_Game->GameFrame();

        m_cachedAttr.clear();
        for (int attr = START(EntityObjectProperty);
            attr != END(EntityObjectProperty); ++attr)
        {
            m_cachedAttr[EntityObjectProperty(attr)] = P(EntityObjectProperty(attr));
        }
    }
}
//////////////////////////////////////////////////////////////////////////
void GameEntity::DebugDrawTarget()
{
    g_Game->DebugDrawMapLine(Position(), TargetPosition(), GCLR_Red);
}
//////////////////////////////////////////////////////////////////////////
void GameEntity::DebugDrawRange()
{
    g_Game->DebugDrawMapCircle(Position(), Type()->P(TP_GroundRange), GCLR_Yellow);
}
//////////////////////////////////////////////////////////////////////////
void GameEntity::DebugDrawLineOfSight()
{
    g_Game->DebugDrawMapCircle(Position(), Type()->P(TP_LineOfSight), GCLR_White);
}
//////////////////////////////////////////////////////////////////////////
void GameEntity::DebugDrawHealthBar()
{
    int barHeight = 1;

    Vector2 barTopLeft(P(OP_Left), P(OP_Bottom));
    Vector2 barBottomRight(P(OP_Right), barTopLeft.Y + barHeight);

    g_Game->DebugDrawMapRectangle(barTopLeft, barBottomRight, GCLR_Red, true);

    int fillHeight = 3;
    float healthPrct = (float)P(OP_Health) / (float)Type()->P(TP_MaxHp);
   
    int barWidth = barBottomRight.X - barTopLeft.X;
    Vector2 fillTopLeft(barTopLeft.X, barBottomRight.Y);
    Vector2 fillBottomRight(barTopLeft.X + (int)(healthPrct * (float)barWidth), fillTopLeft.Y + fillHeight);

    g_Game->DebugDrawMapRectangle(fillTopLeft, fillBottomRight, GCLR_Red, true);
}