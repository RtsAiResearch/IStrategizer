#ifndef CLIENTMAIN_H
#define CLIENTMAIN_H

#include <QtGui/QMainWindow>
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
class ClientMain : public QMainWindow, public BwapiClient
{
	Q_OBJECT

public:
	ClientMain(QWidget *parent = 0, Qt::WFlags flags = 0);
	~ClientMain();

protected:
	void showEvent(QShowEvent *p_pEvent);
	void closeEvent(QCloseEvent *p_pEvent);
	void OnClientUpdate();
	void OnClientLoopStart();
	void OnClientLoopEnd();
	void OnMatchStart();
	void OnMatchEnd(bool p_isWinner);
	void OnUnitCreate(BWAPI::Unit *p_pUnit);
	void OnUnitDestroy(BWAPI::Unit *p_pUnit);
	void OnUniRenegade(BWAPI::Unit *p_pUnit);
	void OnSendText(const std::string &p_text);

private:
	void InitResourceManager();
	void InitIStrategizer();
	void InitIMView();
	void UpdateIMViews();
	void UpdateStatsView();
	void FinalizeIStrategizer();

	Ui::ClientMainClass			ui;
	IStrategizerEx				*m_pIStrategizer;
	IStrategizer::RtsGame		*m_pGameModel;
	IMViewWidget				*m_pBuildingDataIMWidget;
	IMViewWidget				*m_pGrndCtrlIMWidget;
	std::vector<IMViewWidget*>	m_IMViews;
	bool						m_isLearning;
};

#endif // CLIENTMAIN_H
