#include "IMDrawingStrategy.h"
#include "InfluenceMap.h"
#include <cassert>
#include <cmath>

using namespace IStrategizer;
using namespace std;

DrawIMCallback IMDrawingStrategy::GetStrategy(IMType p_type)
{
	switch (p_type)
	{
	case IM_BuildingData:
		return DrawBuildingDataIM;
	case IM_GroundControl:
		return DrawGroundControlIM;
	}

	assert(0);
	return NULL;
}
//////////////////////////////////////////////////////////////////////////
void IMDrawingStrategy::DrawBuildingDataIM(const InfluenceMap *p_pIM, QPainter &p_painter)
{
	QPen		pen(Qt::black, 0);
	QBrush		occupanceBrush(Qt::blue);
	QBrush		reserveBrush(Qt::green);
	int			endX = p_pIM->GridWidth();
	int			endY = p_pIM->GridHeight();
	int			dX = p_pIM->CellSide();
	int			dY = p_pIM->CellSide();
	TCell		cellInf;
	const TCell *pIMData = p_pIM->Map();

	if (!(endX && endX && dX && dY))
		return;

	p_painter.save();

	pen.setStyle(Qt::SolidLine);
	p_painter.setPen(pen);

	for (int x = 0; x < endX; ++x)
	{
		for (int y = 0; y < endY; ++y)
		{
			cellInf = pIMData[y * endX + x];

			if (cellInf > 0)
			{
				p_painter.setBrush(occupanceBrush);
				p_painter.drawRect(x * dX, y * dY, dX, dY);
			}
			else if (cellInf < 0)
			{
				p_painter.setBrush(reserveBrush);
				p_painter.drawRect(x * dX, y * dY, dX, dY);
			}
		}
	}

	p_painter.restore();
}
//////////////////////////////////////////////////////////////////////////
void IMDrawingStrategy::DrawGroundControlIM(const InfluenceMap *p_pIM, QPainter &p_painter)
{
	QPen				pen(Qt::black, 0);
	QBrush				enemyBrush(QColor(0, 0, 255));
	QBrush				playerBrush(QColor(255, 0, 0));
	QBrush				emptyBrush(QColor(0, 0, 0));
	int					endX = p_pIM->GridWidth();
	int					endY = p_pIM->GridHeight();
	int					dX = p_pIM->CellSide();
	int					dY = p_pIM->CellSide();
	TCell				cellInf;
	int					intenisty;
	const TCell			*pIMData = p_pIM->Map();
	const IMStatistics	&stat = p_pIM->Statistics();

	if (!(endX && endX && dX && dY))
		return;

	p_painter.save();

	pen.setStyle(Qt::SolidLine);
	p_painter.setPen(pen);

	p_painter.setBrush(emptyBrush);
	p_painter.drawRect(0, 0, endX * dX, endY * dY);

	for (int x = 0; x < endX; ++x)
	{
		for (int y = 0; y < endY; ++y)
		{
			cellInf = pIMData[y * endX + x];
			
			if (cellInf == 0)
				continue;

			if (cellInf > 0)
			{
				intenisty = (int)fabs(((float)cellInf / (float)stat.MaxInf) * 255.0f);
				playerBrush.setColor(QColor(intenisty, 0, 0));
				p_painter.setBrush(playerBrush);
			}
			else if (cellInf < 0)
			{
				intenisty = (int)fabs(((float)cellInf / (float)stat.MinInf) * 255.0f);
				enemyBrush.setColor(QColor(0, 0, intenisty));
				p_painter.setBrush(enemyBrush);
			}

			p_painter.drawRect(x * dX, y * dY, dX, dY);
		}
	}

	p_painter.restore();
}