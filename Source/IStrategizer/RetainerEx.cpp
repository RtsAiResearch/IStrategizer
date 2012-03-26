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

using namespace OLCBP;

RetainerEx::RetainerEx(string p_caseBasePath) : AbstractRetainer("retainer"), _caseBasePath(p_caseBasePath), _caseBaseLoaded(false)
{
}
//----------------------------------------------------------------------------------------------
void RetainerEx::ReadCaseBase()
{
    Log(LOG_Information, "reading case-base");

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
		Log(LOG_Information, "flushing case-base");

		g_ObjectSerializer.Serialize(_caseBase, _caseBasePath);
	}
    else
	{
		Log(LOG_Error, "failed to flushing case-base");
	}
}
//-------------------------------------------------------------------------------------------------------------------------------
void RetainerEx::Retain(const CaseEx* p_case)
{
    assert(_caseBaseLoaded);
	assert(p_case);

    Log(LOG_Information, "retaining case");

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
    Toolbox::MemoryClean(_caseBase);
}