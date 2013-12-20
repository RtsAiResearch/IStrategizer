#ifndef PLANGRAPHVIEWWIDGET_H
#define PLANGRAPHVIEWWIDGET_H

#include <QWidget>
#include "ui_PlanGraphViewWidget.h"

class PlanGraphViewWidget : public QWidget
{
	Q_OBJECT

public:
	PlanGraphViewWidget(QWidget *parent = 0);
	~PlanGraphViewWidget();

private:
	Ui::PlanGraphViewWidget ui;
};

#endif // PLANGRAPHVIEWWIDGET_H
