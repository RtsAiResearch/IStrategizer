#ifndef CLIENTMAIN_H
#define CLIENTMAIN_H

#include <QMainWindow>
#include "ui_ClientMain.h"
#include "BwapiClient.h"
#include <Windows.h>
#include <vector>

class IStrategizerEx;
namespace IStrategizer
{
	class RtsGame;
}

class IMViewWidget;
class PlannerViewWidget;

class ClientMain : public QMainWindow, public BwapiClient
{
	Q_OBJECT

public:
	ClientMain(QWidget *parent = 0, Qt::WindowFlags flags = 0);
	~ClientMain();

protected:
	void showEvent(QShowEvent *p_pEvent);
	void closeEvent(QCloseEvent *p_pEvent);
	void OnClientUpdate();
	void OnClientLoopStart();
	void OnClientLoopEnd();
	void OnMatchStart();
	void OnMatchEnd(bool p_isWinner);
	void OnUnitCreate(BWAPI::Unit p_pUnit);
	void OnUnitDestroy(BWAPI::Unit p_pUnit);
	void OnUniRenegade(BWAPI::Unit p_pUnit);
	void OnSendText(const std::string &p_text);

private:
	void InitResourceManager();
	void InitIStrategizer();
	void InitIMView();
	void UpdateViews();
	void UpdateStatsView();
	void FinalizeIStrategizer();
	void InitPlannerView();

	Ui::ClientMainClass			ui;
	IStrategizerEx				*m_pIStrategizer;
	IStrategizer::RtsGame		*m_pGameModel;
	IMViewWidget				*m_pBldngDataIMWdgt;
	IMViewWidget				*m_pGrndCtrlIMWdgt;
	std::vector<IMViewWidget*>	m_IMViews;
	PlannerViewWidget			*m_pPlannerViewWdgt;
	bool						m_isLearning;
};

#endif // CLIENTMAIN_H
