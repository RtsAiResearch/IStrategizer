#ifndef CASEVISUALIZER_H
#define CASEVISUALIZER_H

#pragma warning(push, 3)
#include <QMainWindow>
#include "ui_CaseVisualizer.h"
#pragma warning(pop)

#include <string>

#ifndef CROSSMAP_H
#include "CrossMap.h"
#endif

class QToolBar;
class QButtonGroup;

#define CaseBaseFilter tr("Case-base (*.cb)")

namespace IStrategizer
{
    enum GoalType;
    class CaseBaseEx;
    class CaseView;
    class ChoosePlanStepDialog;
    class CaseEx;

    class CaseVisualizer : public QMainWindow
    {
        Q_OBJECT

    public:
        CaseVisualizer(QWidget *parent = 0, Qt::WindowFlags flags = 0);
        ~CaseVisualizer();

        void OpenCaseBase(QString cbFilename);
        void NewCaseBase();
        void SaveCaseBaseAs();
        void SaveCaseBase();
        void Refresh();
        void NewCase();
        void NewCase(GoalType p_caseGoal);
        void DeleteCase(CaseEx* pCase);
        void EditCase();
        void SelectCase(int caseIdx);
        void DuplicateCase(CaseEx* pCase);

    private:
        Ui::CaseVisualizerClass ui;
        CaseBaseEx* m_pCaseBase;
        CaseView* m_pCaseView;
        QToolBar* m_pointerToolbar;
        QButtonGroup* m_pointerTypeGroup;
        QString m_caseBasePath;
        ChoosePlanStepDialog* m_goalDialog;
        CrossMap<unsigned, std::string> m_idLookup;
        bool InitIdLookup();
        void CreateToolBox();

        private slots:
            void on_actionSaveAs_triggered();
            void on_actionShow_triggered();
            void on_lstCases_itemSelectionChanged();
            void on_btnDeleteCase_clicked();
            void on_btnDuplicateCase_clicked();
            void on_btnNewCase_clicked();
            void on_lstCases_itemDoubleClicked(QListWidgetItem*);
            void on_actionNew_triggered();
            void on_actionSave_triggered();
            void on_actionOpen_triggered();
            void PointerGroupClicked(int);
            void on_btnReloadCB_clicked();
    };
}

#endif // CASEVISUALIZER_H
