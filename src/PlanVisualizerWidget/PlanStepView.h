#ifndef PLANSTEPVIEW_H
#define PLANSTEPVIEW_H

#pragma warning(push, 3)
#include <QWidget>
#include <QTableWidgetItem>
#include "ui_PlanStepView.h"
#pragma warning(pop)

#include <string>

#ifndef CROSSMAP_H
#include "CrossMap.h"
#endif
#ifndef ENGINEDATA_H
#include "EngineData.h"
#endif

namespace IStrategizer
{
    class PlanStepEx;
    class ParameterEdit;

    class PlanStepView : public QWidget
    {
        Q_OBJECT

    public:
        PlanStepView(CrossMap<unsigned, std::string>* p_idLookup, QWidget *parent = 0);
        void View(PlanStepEx* p_planStep);
        ~PlanStepView();

    private:
        CrossMap<unsigned, std::string>* m_idLookup;
        PlanStepEx* m_planStep;
        ParameterEdit* m_paramEditDialog;
        void ViewParameters(const PlanStepParameters* p_params);
        void ViewConditions(const PlanStepEx* p_planStep);
        void ViewPerformance(const PlanStepEx* p_planStep) const;
        Ui::PlanStepViewClass ui;

        private slots:
            void on_tblParameters_itemDoubleClicked(QTableWidgetItem* p_item);
            void EditSelectedParameter();
    };
}

#endif // PLANSTEPVIEW_H
