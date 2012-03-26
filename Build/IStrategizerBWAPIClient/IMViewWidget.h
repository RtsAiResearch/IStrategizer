#ifndef IMVIEWWIDGET_H
#define IMVIEWWIDGET_H

#include <QWidget>
#include "ui_IMViewWidget.h"

namespace IStrategizer
{
	class InfluenceMap;
}

class IMView;
class IMViewWidget : public QWidget
{
	Q_OBJECT

public:
	IMViewWidget(QWidget *parent = 0);
	~IMViewWidget();
	void SetIM(const IStrategizer::InfluenceMap *p_pIM);
	void update();
private:
	Ui::IMViewWidgetClass				ui;
	const IStrategizer::InfluenceMap	*m_pIM;
	IMView								*m_pIMView;
};

#endif // IMVIEWWIDGET_H
