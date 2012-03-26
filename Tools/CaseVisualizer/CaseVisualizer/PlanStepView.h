#ifndef PLANSTEPVIEW_H
#define PLANSTEPVIEW_H

#include <QWidget>
#include "ui_PlanStepView.h"
#include <string>
using namespace std;
using namespace stdext;

#ifndef CROSSMAP_H
#include "CrossMap.h"
#endif
#ifndef ENGINEDATA_H
#include "EngineData.h"
#endif

class PlanStepEx;
class ParameterEdit;

using namespace DataStructure;

class PlanStepView : public QWidget
{
	Q_OBJECT

public:
	PlanStepView(CrossMap<unsigned, string>* p_idLookup, QWidget *parent = 0);
	void View(PlanStepEx* p_planStep);
	~PlanStepView();

private:
	CrossMap<unsigned, string>*	m_idLookup;
	PlanStepEx* m_planStep;
	ParameterEdit* m_paramEditDialog;
	void ViewParameters( const PlanStepParameters* p_params );
	void ViewConditions(const PlanStepEx* p_planStep);
	Ui::PlanStepViewClass ui;

private slots:
	void on_tblParameters_itemDoubleClicked(QTableWidgetItem* p_item);
	void EditSelectedParameter();
};

#endif // PLANSTEPVIEW_H
