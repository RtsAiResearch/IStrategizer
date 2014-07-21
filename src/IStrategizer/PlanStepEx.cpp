#include "PlanStepEx.h"
#include <cstdio>
#include <algorithm>
#include <Rpc.h>
#include "MathHelper.h"
#include "Logger.h"
#include "ObjectFactory.h"
#include "EngineDefs.h"
#pragma comment(lib, "Rpcrt4.lib")

using namespace std;
using namespace IStrategizer;

unsigned PlanStepEx::s_lastPlanstepID = 0;

unsigned PlanStepEx::GenerateID()
{
    UUID newId;
    UuidCreate(&newId);
    return newId.Data1;
}
//////////////////////////////////////////////////////////////////////////
PlanStepEx::PlanStepEx(int p_stepTypeId, ExecutionStateType p_state) 
    : _stepTypeId(p_stepTypeId),
    _state(p_state), 
    _postCondition(nullptr), 
    _firstUpdate(true),
    _id(GenerateID()),
	m_sleepStartGameFrame(0),
	m_sleepEndGameFrame(0),
	m_sleepsCount(0)
{
    
}
//////////////////////////////////////////////////////////////////////////
PlanStepEx::PlanStepEx(int p_stepTypeId, ExecutionStateType p_state, const PlanStepParameters& p_parameters) 
    : _stepTypeId(p_stepTypeId),
    _state(p_state),
    _params(p_parameters),
    _postCondition(nullptr),
    _firstUpdate(true),
    _id(GenerateID()),
	m_sleepStartGameFrame(0),
	m_sleepEndGameFrame(0),
	m_sleepsCount(0)
{
}
//////////////////////////////////////////////////////////////////////////
PlanStepEx::~PlanStepEx()
{
	SAFE_DELETE(_postCondition);
}
//////////////////////////////////////////////////////////////////////////
void PlanStepEx::Parameters(const PlanStepParameters& p_val)
{
	for (auto& r : p_val)
		_params[r.first] = p_val.at(r.first);
}
//////////////////////////////////////////////////////////////////////////
void PlanStepEx::InitializeConditions()
{
    InitializePostConditions();
}
//////////////////////////////////////////////////////////////////////////
IClonable* PlanStepEx::Clone()
{
    PlanStepEx* clone = static_cast<PlanStepEx*>(g_ObjectFactory.Create(GetObjectLayout().TypeName()));
    Copy(clone);

    return clone;
}
//////////////////////////////////////////////////////////////////////////
void PlanStepEx::Copy(IClonable* p_dest)
{
    PlanStepEx* m_dest = static_cast<PlanStepEx*>(p_dest);

    m_dest->_stepTypeId = _stepTypeId;
    m_dest->_state = _state;
    m_dest->_params = _params;
    m_dest->_postCondition = _postCondition ? static_cast<CompositeExpression*>(_postCondition->Clone()) : nullptr;
}
//////////////////////////////////////////////////////////////////////////
void PlanStepEx::SetState(ExecutionStateType p_state, RtsGame& game, const WorldClock& p_clock)
{
    _ASSERTE(p_state != _state);

    string stepName = ToString();
    const char* oldStateName = Enums[_state];
    const char* newStateName = Enums[p_state];

    LogInfo("%s: '%s'->'%s'", stepName.c_str(), oldStateName, newStateName);

    _state = p_state;
}
//////////////////////////////////////////////////////////////////////////
std::string PlanStepEx::ToString(bool minimal) const
{
    string stepDescription;
    // INT_MAX is 10 digits, reserve array of 16 for performance
    char paramRealVal[16];
    char strID[32];

    const char* stepName = Enums[_stepTypeId];
    unsigned    paramIdx = 0;

    sprintf_s(strID, "%x", _id);

    stepDescription += stepName;
    stepDescription += "[";
    stepDescription += strID;
    stepDescription += "]";
#ifdef LOG_DEBUG_INFO
    sprintf_s(strID, "@0x%x", (void*)this);
    stepDescription += strID;
#endif

    if (!minimal)
    {
        stepDescription += '(';

        for (PlanStepParameters::const_iterator itr = _params.begin();
            itr != _params.end(); ++itr)
        {
            // Parameters format is:
            // <key>=<value>, ...
            ParameterType paramKey = itr->first;

            if (ISREALVAL(ParameterType, paramKey) && 
                itr->second == DONT_CARE)
                continue;

            _ASSERTE(Enums[(int)paramKey]);

            string paramDesc = Enums[(int)paramKey];
            paramDesc += '=';

            // Parameter value is not an engine defined ID,
            // convert the int type to string
            if (ISREALVAL(ParameterType, paramKey))
            {
                _itoa_s(itr->second, paramRealVal, 10);
                paramDesc += paramRealVal;
            }
            // Parameter value is an engine defined ID
            // Use the Enums lookup table to translate it to string
            else
            {
                _ASSERTE(Enums[itr->second] != nullptr);
                paramDesc += Enums[itr->second];
            }

            stepDescription += paramDesc;

            if (paramIdx < _params.size() - 1)
            {
                stepDescription += ',';
                stepDescription += ' ';
            }

            ++paramIdx;
        }

        stepDescription += ')';
    }

    return stepDescription;
}
//----------------------------------------------------------------------------------------------
unsigned PlanStepEx::Hash(bool quantified) const
{
    auto& params = Parameters();
    // + 1 to include the StepTypeId since it is used as well in the hashing
    size_t numWords = (params.size() + 1);
    vector<int> str(numWords);

    str.push_back(StepTypeId());
    for (auto& param : Parameters())
    {
        if (!quantified && param.first == PARAM_Amount)
            continue;

        str.push_back(param.second);
    }

    unsigned h = MathHelper::SuperFastHash((const char*)&*str.cbegin(), str.size() * sizeof(int));
    return h;
}
//----------------------------------------------------------------------------------------------
void PlanStepEx::Sleep(const WorldClock& clock, unsigned numGameFrames)
{
	_ASSERTE(!IsSleeping(clock));

	m_sleepStartGameFrame = clock.ElapsedGameCycles();
	m_sleepEndGameFrame = m_sleepStartGameFrame + numGameFrames;
	++m_sleepsCount;
	LogInfo("%s is sent for sleep in the GameFrame range=[%d,%d], slept %d times so far", ToString().c_str(),
		m_sleepStartGameFrame, m_sleepEndGameFrame, m_sleepsCount);
}
//----------------------------------------------------------------------------------------------