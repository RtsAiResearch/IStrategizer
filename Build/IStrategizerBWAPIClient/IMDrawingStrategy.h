#ifndef IMDRAWINGSTRATEGY_H
#define IMDRAWINGSTRATEGY_H

#include "IMSystemManager.h"
#include <QtGui/QPainter>

class IMDrawingStrategy;

typedef void (*DrawIMCallback)(const IStrategizer::InfluenceMap *p_pIM, QPainter &p_painter);

class IMDrawingStrategy
{
public:
	static DrawIMCallback GetStrategy(IStrategizer::IMType p_type);
private:
	static void DrawBuildingDataIM(const IStrategizer::InfluenceMap *p_pIM, QPainter &p_painter);
	static void DrawGroundControlIM(const IStrategizer::InfluenceMap *p_pIM, QPainter &p_painter);
};
#endif IMDRAWINGSTRATEGY_H