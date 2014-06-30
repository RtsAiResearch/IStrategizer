#ifndef CHOOSEPLANSTEPDIALOG_H
#define CHOOSEPLANSTEPDIALOG_H

#pragma warning(push, 3)
#include "ui_ChoosePlanStepDialog.h"
#include <QDialog>
#pragma warning(pop)

#include <string>

#ifndef CROSSMAP_H
#include "CrossMap.h"
#endif

class QCompleter;

namespace IStrategizer
{
    class ChoosePlanStepDialog : public QDialog
    {
        Q_OBJECT

    public:
        ChoosePlanStepDialog(CrossMap<unsigned, std::string>* p_idLookup, bool p_goals = true, bool p_actions = true, QWidget *parent = 0);
        unsigned SelectedPlanStepId() const { return m_selectedPlanStepId; }
        int exec();

    private:
        void InitializePlanStepList(bool p_goals, bool p_actions);
        void ItemChanged();

        Ui::ChoosePlanStepDialogClass ui;
        CrossMap<unsigned, std::string>* m_idLookup;
        unsigned m_selectedPlanStepId;
        QCompleter* m_autoComplete;
        bool m_chooseGoals;
        bool m_chooseActions;
        bool m_initialized;

        private slots:
            void on_comboBox_currentIndexChanged(QString p_item);
            void on_btnCancel_clicked();
            void on_btnOK_clicked();
    };
}

#endif // CHOOSEGOALDIALOG_H
