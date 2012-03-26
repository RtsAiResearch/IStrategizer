#ifndef PARAMETEREDIT_H
#define PARAMETEREDIT_H

#include <QDialog>
#include "ui_ParameterEdit.h"
#include <string>
#ifndef CROSSMAP_H
#include "CrossMap.h"
#endif

using namespace DataStructure;
using namespace std;

class QCompleter;

class ParameterEdit : public QDialog
{
	Q_OBJECT

public:
	ParameterEdit(string p_paramName, string p_paramValue, CrossMap<unsigned, string>* p_idLookup, QWidget *parent = 0);

	void InitializeAutoComplete();
	~ParameterEdit();
	int exec();
	string ParamName() const { return m_paramName; }
	void ParamName(string p_paramName) { m_paramName = p_paramName; }
	string ParamValue() const { return m_paramValue; }
	void ParamValue(string p_paramValue) { m_paramValue = p_paramValue; }

private:
	Ui::ParameterEditClass ui;
	string m_paramName;
	string m_paramValue;
	CrossMap<unsigned, string>* m_idLookup;
	QCompleter* m_autoComplete;

private slots:
	void on_btnCancel_clicked();
	void on_btnOK_clicked();
};

#endif // PARAMETEREDIT_H
