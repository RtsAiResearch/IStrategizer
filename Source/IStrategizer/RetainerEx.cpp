#include "RetainerEx.h"
#include "CaseBaseEx.h"
#include "Toolbox.h"
#include "ObjectSerializer.h"
#include "RtsGame.h"

#include <fstream>
using namespace std;
using namespace IStrategizer;

RetainerEx::RetainerEx(string p_caseBasePath) : AbstractRetainer("Retainer"), _caseBasePath(p_caseBasePath), _caseBaseLoaded(false)
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
void RetainerEx::Retain(CaseEx* p_case)
{
    _ASSERTE(_caseBaseLoaded);
    _ASSERTE(p_case);

    LogInfo("retaining case");

    for (size_t i = 0; i < _caseBase->CaseContainer.size(); ++i)
    {
        if (_caseBase->CaseContainer[i] == p_case)
        {
            _caseBase->CaseContainer[i] = p_case;
            return;
        }
    }

    _caseBase->CaseContainer.push_back(const_cast<CaseEx*>(p_case));
}
//----------------------------------------------------------------------------------------------
void RetainerEx::ExecuteCommand(const char* p_cmd)
{
    if(!strcmp(p_cmd, "flush"))
    {
        Flush();
        g_Game->DisplayMessage("flushing case-base has be performed successfully");
    }
    else if(!strcmp(p_cmd, "info"))
    {
        char buffer[128];
        sprintf_s(buffer, "retriever has %d cases", _caseBase->CaseContainer.size());
        g_Game->DisplayMessage(buffer);
    }
}
//-------------------------------------------------------------------------------------------------------------------------------
RetainerEx::~RetainerEx()
{
    Flush();
    Toolbox::MemoryClean(_caseBase);
}
