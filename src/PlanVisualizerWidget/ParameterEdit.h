#ifndef PARAMETEREDIT_H
#define PARAMETEREDIT_H

#pragma warning(push, 3)
#include <QDialog>
#include "ui_ParameterEdit.h"
#pragma warning(pop)

#include <string>
#ifndef CROSSMAP_H
#include "CrossMap.h"
#endif

class QCompleter;

namespace IStrategizer
{
    class ParameterEdit : public QDialog
    {
        Q_OBJECT

    public:
        ParameterEdit(std::string p_paramName, std::string p_paramValue, CrossMap<unsigned, std::string>* p_idLookup, QWidget *parent = 0);

        void InitializeAutoComplete();
        int exec();
        std::string ParamName() const { return m_paramName; }
        void ParamName(const std::string& p_paramName) { m_paramName = p_paramName; }
        std::string ParamValue() const { return m_paramValue; }
        void ParamValue(const std::string& p_paramValue) { m_paramValue = p_paramValue; }

    private:
        Ui::ParameterEditClass ui;
        std::string m_paramName;
        std::string m_paramValue;
        CrossMap<unsigned, std::string>* m_idLookup;
        QCompleter* m_autoComplete;
        bool m_initialized;

    private slots:
        void on_btnCancel_clicked();
        void on_btnOK_clicked();
    };
}

#endif // PARAMETEREDIT_H
