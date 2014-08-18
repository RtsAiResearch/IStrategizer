#include "Action.h"
#include "And.h"
#include "Logger.h"
#include "CompositeExpression.h"
#include "RtsGame.h"
#include "MathHelper.h"
#include "CellFeature.h"
#include "EngineDefs.h"

using namespace std;
using namespace IStrategizer;

Action::Action(ActionType p_actionType, unsigned p_maxPrepTime, unsigned p_maxExecTime) :
PlanStepEx(p_actionType, ESTATE_END),
_preCondition(nullptr)
{
    memset(_stateStartTime, 0, sizeof(_stateStartTime));
    memset(_stateTimeout, 0, sizeof(_stateTimeout));

    _stateTimeout[INDEX(ESTATE_NotPrepared, ExecutionStateType)] = p_maxPrepTime;
    _stateTimeout[INDEX(ESTATE_Executing, ExecutionStateType)] = p_maxExecTime;
}
//////////////////////////////////////////////////////////////////////////
Action::Action(ActionType p_actionType, const PlanStepParameters& p_parameters, unsigned p_maxPrepTime, unsigned p_maxExecTime) :
PlanStepEx(p_actionType, ESTATE_END, p_parameters),
_preCondition(nullptr)
{
    memset(_stateStartTime, 0, sizeof(_stateStartTime));
    memset(_stateTimeout, 0, sizeof(_stateTimeout));

    _stateTimeout[INDEX(ESTATE_NotPrepared, ExecutionStateType)] = p_maxPrepTime;
    _stateTimeout[INDEX(ESTATE_Executing, ExecutionStateType)] = p_maxExecTime;
}
//////////////////////////////////////////////////////////////////////////
Action::~Action()
{
    SAFE_DELETE(_preCondition);
}
//////////////////////////////////////////////////////////////////////////
void Action::SetState(ExecutionStateType p_state)
{
    PlanStepEx::SetState(p_state);

    _stateStartTime[INDEX(p_state, ExecutionStateType)] = g_Game->GameFrame();

    switch (p_state)
    {
    case ESTATE_Succeeded:
        OnSucccess();
        break;
    case ESTATE_Failed:
        OnFailure();
        break;
    }
}
//////////////////////////////////////////////////////////////////////////
bool Action::PreconditionsSatisfied()
{
    if (_preCondition == nullptr) { InitializeConditions(); }
    bool satisfied = _preCondition->Evaluate(*g_Game);

    return satisfied;
}
//////////////////////////////////////////////////////////////////////////
void Action::InitializeConditions()
{
    PlanStepEx::InitializeConditions();
    InitializePreConditions();
}
//////////////////////////////////////////////////////////////////////////
void Action::Update()
{
    if (_firstUpdate)
    {
        Reset();
        _firstUpdate = false;
    }

    if (IsSleeping())
        return;
    else if (IsCurrentStateTimeout())
    {
        LogInfo(
            "%s state %s timed-out after %dms",
            ToString().c_str(),
            Enums[(int)GetState()],
            _stateTimeout[INDEX(GetState(), ExecutionStateType)]);
        SetState(ESTATE_Failed);
        return;
    }

    ExecutionStateType state = PlanStepEx::GetState();

    switch (state)
    {
    case ESTATE_NotPrepared:
        if (PreconditionsSatisfied())
        {
            LogInfo("Preconditions satisfied, trying to execute action %s", ToString().c_str());

            LogInfo("Trying to execute action %s", ToString().c_str());

            if (Execute())
            {
                SetState(ESTATE_Executing);
            }
            else
            {
                SetState(ESTATE_Failed);
            }
        }
        break;

    case ESTATE_Executing:

        if (SuccessConditionsSatisfied(*g_Game))
        {
            SetState(ESTATE_Succeeded);
            m_history.Add(ESTATE_Succeeded);
        }
        else if (!AliveConditionsSatisfied())
        {
            LogInfo("%s alive conditions not satisfied", ToString().c_str());
            SetState(ESTATE_Failed);
        }
        break;
    }
}
//////////////////////////////////////////////////////////////////////////
void Action::Copy(IClonable* p_dest)
{
    PlanStepEx::Copy(p_dest);

    Action* m_dest = static_cast<Action*>(p_dest);
    m_dest->_preCondition = _preCondition ? static_cast<CompositeExpression*>(_preCondition->Clone()) : nullptr;
}
//////////////////////////////////////////////////////////////////////////
unsigned Action::Hash(bool quantified) const
{
    auto& params = Parameters();
    // + 1 to include the StepTypeId since it is used as well in the hashing
    size_t numWords = (params.size() + 1);
    vector<int> str(numWords);

    PlanStepParameters dummyParams;
    CellFeature::Null().To(dummyParams);

    str.push_back(StepTypeId());
    for (auto& param : Parameters())
    {
        if (!quantified && param.first == PARAM_Amount)
            continue;

        // FIXME: for now we ignore cell features for simplicity
        if (dummyParams.count(param.first) > 0)
            continue;

        str.push_back(param.second);
    }

    unsigned h = MathHelper::SuperFastHash((const char*)&*str.cbegin(), str.size() * sizeof(int));
    return h;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Action::IsCurrentStateTimeout()
{
    unsigned timeout = _stateTimeout[INDEX(_state, ExecutionStateType)];
    unsigned startTime = _stateStartTime[INDEX(_state, ExecutionStateType)];

    // 0 means no timeout and thus meaning infinite timeout
    if (timeout == 0)
        return false;
    else
        return ((g_Game->GameFrame() - startTime) > timeout);
}

