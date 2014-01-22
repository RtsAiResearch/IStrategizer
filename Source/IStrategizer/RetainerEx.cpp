#ifndef RETAINEREX_H
#include "RetainerEx.h"
#endif
#ifndef CASEBASEEX_H
#include "CaseBaseEx.h"
#endif
#ifndef TOOLBOX_H
#include "Toolbox.h"
#endif
#ifndef OBJECTSERIALIZER_H
#include "ObjectSerializer.h"
#endif
#ifndef RTSGAME_H
#include "RtsGame.h"
#endif

#include <fstream>
using namespace std;
using namespace IStrategizer;

RetainerEx::RetainerEx(RtsGame& p_RtsGame, string p_caseBasePath) : AbstractRetainer(p_RtsGame, "Retainer"), _caseBasePath(p_caseBasePath), _caseBaseLoaded(false)
{
}
//----------------------------------------------------------------------------------------------
void RetainerEx::ReadCaseBase()
{
    LogInfo("reading case-base");

    Toolbox::MemoryClean(_caseBase);
    _caseBase = new CaseBaseEx();

    fstream file;

    file.open(_caseBasePath.c_str(), ios::in | ios::binary);

    // Read existing case-base
    if (file.is_open())
    {
        file.close();
        g_ObjectSerializer.Deserialize(_caseBase, _caseBasePath);
        _caseBaseLoaded = true;
    }
    // Create case-base if not found
    else
    {
        file.open(_caseBasePath.c_str(), ios::out | ios::binary | ios::app);
        file.close();
        _caseBaseLoaded = true;
        Flush();
    }
}
//----------------------------------------------------------------------------------------------
void RetainerEx::Flush()
{
    if (_caseBaseLoaded && _caseBase)
    {
        LogInfo("Flushing case-base");

        g_ObjectSerializer.Serialize(_caseBase, _caseBasePath);
    }
    else
    {
        LogError("Failed to flushing case-base");
    }
}
//-------------------------------------------------------------------------------------------------------------------------------
void RetainerEx::Retain(const CaseEx* p_case)
{
    assert(_caseBaseLoaded);
    assert(p_case);

    LogInfo("retaining case");

    _caseBase->CaseContainer.push_back(const_cast<CaseEx*>(p_case));
}
//----------------------------------------------------------------------------------------------
void RetainerEx::ExecuteCommand(RtsGame& p_RtsGame, const char* p_cmd)
{
    if(!strcmp(p_cmd, "flush"))
    {
        Flush();
        p_RtsGame.DisplayMessage("flushing case-base has be performed successfully");
    }
    else if(!strcmp(p_cmd, "info"))
    {
        char buffer[128];
        sprintf_s(buffer, "retriever has %d cases", _caseBase->CaseContainer.size());
        p_RtsGame.DisplayMessage(buffer);
    }
}
//-------------------------------------------------------------------------------------------------------------------------------
RetainerEx::~RetainerEx()
{
    Flush();
    Toolbox::MemoryClean(_caseBase);
}
