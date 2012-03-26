#ifndef CASEVISUALIZER_H
#define CASEVISUALIZER_H

#include <QtGui/QMainWindow>
#include "ui_CaseVisualizer.h"
#include <string>

#ifndef CROSSMAP_H
#include "CrossMap.h"
#endif

class CaseBaseEx;
class CaseView;
class ChoosePlanStepDialog;
class QToolBar;
class QButtonGroup;


namespace MetaData
{
	enum GoalType;
}

using namespace std;
using namespace DataStructure;
using namespace MetaData;

class CaseVisualizer : public QMainWindow
{
    Q_OBJECT

public:
    CaseVisualizer(QWidget *parent = 0, Qt::WFlags flags = 0);
    ~CaseVisualizer();

    void OpenCaseBase();
    void NewCaseBase();
    void SaveCaseBase();
    void Refresh();
    void NewCase();
	void NewCase(GoalType p_caseGoal);
    void DeleteCase(int p_caseIdx);
    void EditCase();

private:
    Ui::CaseVisualizerClass ui;
    CaseBaseEx* m_caseBase;
    CaseView*   m_caseView;
	QToolBar*	m_pointerToolbar;
	QButtonGroup* m_pointerTypeGroup;
	ChoosePlanStepDialog* m_goalDialog;
	CrossMap<unsigned, string> m_idLookup;
	bool InitIdLookup();
	void CreateToolBox();


private slots:
	void on_actionShow_triggered();
    void on_lstCases_itemSelectionChanged();
    void on_btnDeleteCase_clicked();
    void on_btnNewCase_clicked();
    void on_lstCases_itemDoubleClicked(QListWidgetItem*);
    void on_actionNew_triggered();
    void on_actionSave_triggered();
    void on_actionOpen_triggered();
	void PointerGroupClicked(int);
};

#endif // CASEVISUALIZER_H
