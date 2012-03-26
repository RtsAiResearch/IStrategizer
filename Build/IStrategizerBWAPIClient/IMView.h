#ifndef IMVIEW_H
#define IMVIEW_H

#include <QWidget>
#include "IMDrawingStrategy.h"

namespace IStrategizer
{
	class InfluenceMap;
}

class IMView : public QWidget
{
	Q_OBJECT

public:
	IMView(QWidget *p_pParent = NULL);
	~IMView();
	QSize minimumSizeHint() const;
	QSize sizeHint() const;
	void SetIM(const IStrategizer::InfluenceMap *p_pIM);
protected:
	void paintEvent(QPaintEvent *event);
	void FitGridToCanvas(QPainter &p_painter);
	void DrawGrid(QPainter &p_painter);
	void DrawInfluence(QPainter &p_painter);

	const IStrategizer::InfluenceMap *m_pIM;

private:
	qreal			m_cachedScaleX;
	qreal			m_cachedScaleY;
	qreal			m_cachedTranslateX;
	qreal			m_cachedTranslateY;
	bool			m_transformationCached;
	DrawIMCallback	m_pfnIMDrawer;
};
#endif // IMVIEW_H
