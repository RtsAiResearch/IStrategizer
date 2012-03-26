#ifndef CLIENTMAIN_H
#define CLIENTMAIN_H

#include <QtGui/QMainWindow>
#include "ui_ClientMain.h"
#include <Windows.h>
#include <vector>

class IStrategizerEx;
namespace IStrategizer
{
	class RtsGame;
}

namespace BWAPI
{
	class Unit;
}

class IMViewWidget;
class ClientMain : public QMainWindow
{
	Q_OBJECT

public:
	ClientMain(QWidget *parent = 0, Qt::WFlags flags = 0);
	~ClientMain();

	void FinalizeIStrategizer();
	static DWORD BwapiThreadStart(PVOID p_pvContext);
	void BwapiMainThread();

	void InitResourceManager();

protected:
	void showEvent(QShowEvent *p_pEvent);
	void closeEvent(QCloseEvent *p_pEvent) ;

	void OnUnitCreate(BWAPI::Unit *p_pUnit);
	void OnUnitDestroy(BWAPI::Unit *p_pUnit);
	void OnUniRenegade(BWAPI::Unit *p_pUnit);
	void OnSendText(const std::string &p_text);

private:
	void ConnectToBwapiServer();
	void InitClient();

	void InitIMView();
	void Shutdown();
	void EventLoop();

	void UpdateIMViews();
	void UpdateStatsView();
	void UpdateTerrainAnalysis();
	void InitIStrategizer();

	Ui::ClientMainClass			ui;
	HANDLE						m_hBwapiThread;
	bool						m_bClientInitialized;
	IStrategizerEx				*m_pIStrategizer;
	IStrategizer::RtsGame		*m_pGameModel;
	IMViewWidget				*m_pBuildingDataIMWidget;
	IMViewWidget				*m_pGrndCtrlIMWidget;
	std::vector<IMViewWidget*>	m_IMViews;
	int							m_engineCycles;
};

#endif // CLIENTMAIN_H
