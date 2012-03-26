#ifndef CHOOSEPLANSTEPDIALOG_H
#define CHOOSEPLANSTEPDIALOG_H

#include "ui_ChoosePlanStepDialog.h"

#include <string>
#include <QDialog>

#ifndef CROSSMAP_H
#include "CrossMap.h"
#endif

using namespace DataStructure;
using namespace std;

class QCompleter;
class ChoosePlanStepDialog : public QDialog
{
	Q_OBJECT

public:
	ChoosePlanStepDialog(CrossMap<unsigned, string>* p_idLookup, bool p_goals = true, bool p_actions = true, QWidget *parent = 0);
	~ChoosePlanStepDialog();
	unsigned SelectedPlanStepId() const { return m_selectedPlanStepId; }
	void InitializePlanStepList(bool p_goals, bool p_actions);

private:
	Ui::ChoosePlanStepDialogClass ui;
	CrossMap<unsigned, string>* m_idLookup;
	unsigned m_selectedPlanStepId;
	QCompleter* m_autoComplete;
	void ItemChanged();

private slots:
	void on_comboBox_currentIndexChanged(QString p_item);
	void on_btnCancel_clicked();
	void on_btnOK_clicked();
};

#endif // CHOOSEGOALDIALOG_H