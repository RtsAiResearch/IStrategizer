#include "IMView.h"
#include "QtGui/QPainter"
#include <cassert>
#include "InfluenceMap.h"
#include "IMDrawingStrategy.h"

using namespace IStrategizer;

IMView::IMView(QWidget *parent)
: QWidget(parent), m_pIM(NULL), m_pfnIMDrawer(NULL), m_transformationCached(false)
{
	setBackgroundRole(QPalette::Dark);
	setAutoFillBackground(true);
}
//////////////////////////////////////////////////////////////////////////
IMView::~IMView()
{

}
//////////////////////////////////////////////////////////////////////////
void IMView::SetIM(const InfluenceMap *p_pIM)
{
	m_pIM = p_pIM;
	m_transformationCached = false;
	m_pfnIMDrawer = IMDrawingStrategy::GetStrategy(p_pIM->TypeId());
}
//////////////////////////////////////////////////////////////////////////
QSize IMView::minimumSizeHint() const
{
	return QSize(800, 600);
}
//////////////////////////////////////////////////////////////////////////
QSize IMView::sizeHint() const
{
	return QSize(800, 600);
}
//////////////////////////////////////////////////////////////////////////
void IMView::paintEvent(QPaintEvent *event)
{
	if (!m_pIM)
		return;

	QPainter painter(this);

	painter.save();

	FitGridToCanvas(painter);
	DrawGrid(painter);
	DrawInfluence(painter);

	painter.restore();
};
//////////////////////////////////////////////////////////////////////////
void IMView::FitGridToCanvas(QPainter &p_painter)
{
	if (!m_transformationCached)
	{
		QSize		widgetSize = QWidget::size();
		int			gridWidth = m_pIM->GridWidth() * m_pIM->CellSide();
		int			gridHeight = m_pIM->GridHeight() * m_pIM->CellSide();
		qreal		scaleX = (qreal)widgetSize.width() / (qreal)gridWidth;
		qreal		scaleY = (qreal)widgetSize.height() / (qreal)gridHeight;
		qreal		uniformScale = min(scaleX, scaleY);
		qreal		offsetX = 0.0;
		qreal		offsetY = 0.0;

		// We scale on the X-Axis, then Align on Y-Axis
		if (scaleX < scaleY)
			offsetY = (widgetSize.height() - ((qreal)gridHeight * uniformScale)) / 2.0;
		else
			offsetX = (widgetSize.width() - ((qreal)gridWidth * uniformScale)) / 2.0;

		m_cachedScaleX = uniformScale;
		m_cachedScaleY = uniformScale;
		m_cachedTranslateX = offsetX / uniformScale;
		m_cachedTranslateY = offsetY / uniformScale;

		m_transformationCached = true;
	}

	p_painter.scale(m_cachedScaleX, m_cachedScaleY);
	p_painter.translate(m_cachedTranslateX, m_cachedTranslateY);
}
//////////////////////////////////////////////////////////////////////////
void IMView::DrawGrid(QPainter &p_painter)
{
	QPen	pen(Qt::lightGray, 0);
	int		endX = m_pIM->WorldWidth();
	int		endY = m_pIM->WorldHeight();
	int		dX = m_pIM->CellSide();
	int		dY =  m_pIM->CellSide();

	if (!(endX && endX && dX && dY))
		return;

	pen.setStyle(Qt::DotLine);
	p_painter.setPen(pen);

	for (int startX = 0; startX <= endX; startX += dX)
	{
		p_painter.drawLine(startX, 0, startX, endY);
	}

	for (int startY = 0; startY <= endY; startY += dY)
	{
		p_painter.drawLine(0, startY, endX, startY);
	}
}
//////////////////////////////////////////////////////////////////////////
void IMView::DrawInfluence(QPainter &p_painter)
{
	if (m_pfnIMDrawer)
		m_pfnIMDrawer(m_pIM, p_painter);
}