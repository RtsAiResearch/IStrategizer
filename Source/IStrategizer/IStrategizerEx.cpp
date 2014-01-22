#include <cassert>
#include "IStrategizerEx.h"
#include "MessagePump.h"
#include "OnlineCaseBasedPlannerEx.h"
#include "LearningFromHumanDemonstration.h"
#include "GoalFactory.h"
#include "CaseLearningHelper.h"
#include "IMSystemManager.h"
#include "DataMessage.h"
#include "IStrategizerException.h"
#include "SerializationEssentials.h"
#include "WorldClock.h"
#include "RtsGame.h"
#include "DynamicComponent.h"
#include "Toolbox.h"
#include "IMSystemManager.h"

using namespace IStrategizer;

IStrategizerEx::IStrategizerEx(const IStrategizerParam &p_param, RtsGame& p_RtsGame) 
    : _self(PLAYER_Self),
    _enemy(PLAYER_Enemy),
    _param(p_param),
    _caseLearning(nullptr),
    _planner(nullptr),
    _isFirstUpdate(true),
    m_RtsGame(p_RtsGame)
{
    m_RtsGame.Init();

    PlanStepParameters params;
    params[PARAM_PlayerId] = _self;
    params[PARAM_StrategyTypeId] = STRTYPE_EarlyTierRush;

    switch(p_param.Phase)
    {
    case PHASE_Online:
        _planner = new OnlineCaseBasedPlannerEx();
        _planner->Init(m_RtsGame, GoalFactory::Instance().GetGoal(GOALEX_WinGame, params));
        g_OnlineCaseBasedPlanner = _planner;
        break;

    case PHASE_Offline:
        _caseLearning = new LearningFromHumanDemonstration(m_RtsGame, _self, _enemy);
        MessagePump::Instance(m_RtsGame).RegisterForMessage(m_RtsGame, MSp_RtsGameEnd, this);
        break;
    }

    IMSysManagerParam imSysMgrParam;
    imSysMgrParam.BuildingDataIMCellSize = _param.BuildingDataIMCellSize;
    imSysMgrParam.GroundControlIMCellSize = _param.GrndCtrlIMCellSize;

    g_IMSysMgr.Init(m_RtsGame, imSysMgrParam);
    MessagePump::Instance(m_RtsGame).RegisterForMessage(m_RtsGame, MSG_EntityCreate, this);
    MessagePump::Instance(m_RtsGame).RegisterForMessage(m_RtsGame, MSG_EntityDestroy, this);

    DynamicComponent::RealTime(true);
    // DynamicComponent::GlobalInvalidation(true);
    // DynamicComponent::GlobalInvalidationInterval(2);
}
//---------------------------------------------------------------------------------------------
void IStrategizerEx::NotifyMessegeSent(RtsGame& m_RtsGame, Message* p_message)
{
    switch(p_message->MessageTypeID())
    {
    case MSp_RtsGameStart:
        _clock.Reset();
    case MSp_RtsGameEnd:
        if (_param.Phase == PHASE_Offline)
        {
            StartOfflineLearning();
        }
        break;
    }
}
//--------------------------------------------------------------------------------
void IStrategizerEx::Update(unsigned p_gameCycle)
{
    try
    {
        if (_isFirstUpdate)
        {
            _clock.Reset();
            _isFirstUpdate = false;
        }

        _clock.Update(p_gameCycle);
        MessagePump::Instance(m_RtsGame).Update(_clock);
        g_IMSysMgr.Update(m_RtsGame, _clock);

        if (_param.Phase == PHASE_Online)
            _planner->Update(m_RtsGame, _clock);
    }
    catch (IStrategizer::Exception &e)
    {
        e.To(cout);
    }
    catch (std::exception &e)
    {
        cout << "IStrategizer encountered unhandled std exception: " << e.what() << endl;
    }
}
//--------------------------------------------------------------------------------
void IStrategizerEx::StartOfflineLearning()
{
    _caseLearning->Learn(m_RtsGame);
}
//----------------------------------------------------------------------------------------------
IStrategizerEx::~IStrategizerEx()
{
    g_IMSysMgr.Finalize();
    Toolbox::MemoryClean(_planner);
    Toolbox::MemoryClean(_caseLearning);
}