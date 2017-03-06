#include "RetainerEx.h"
#include "CaseBaseEx.h"
#include "Toolbox.h"
#include "ObjectSerializer.h"
#include "IStrategizerEx.h"
#include "RtsGame.h"
#include "GoalEx.h"

#include <fstream>

using namespace std;
using namespace IStrategizer;

RetainerEx::RetainerEx() :
m_caseBaseLoaded(false)
{
}
//----------------------------------------------------------------------------------------------
void RetainerEx::ReadCaseBase()
{
    LogInfo("Reading case-base %s", CASEBASE_IO_READ_PATH.c_str());

    fstream file;

    file.open(CASEBASE_IO_READ_PATH, ios::in | ios::binary);

    // Read existing case-base
    if (file.is_open())
    {
        file.close();
        g_ObjectSerializer.Deserialize(&m_casebase, CASEBASE_IO_READ_PATH);
        m_caseBaseLoaded = true;
    }
}
//----------------------------------------------------------------------------------------------
void RetainerEx::Retain(_In_ CaseEx* pCase)
{
    if (find(m_casebase.CaseContainer.begin(), m_casebase.CaseContainer.end(), pCase) == m_casebase.CaseContainer.end())
        m_casebase.CaseContainer.insert(pCase);
}
//----------------------------------------------------------------------------------------------
void RetainerEx::Flush()
{
    if (m_caseBaseLoaded)
    {
        LogInfo("Flushing case-base");
        g_ObjectSerializer.Serialize(&m_casebase, CASEBASE_IO_WRITE_PATH);
    }
    else
    {
        LogError("Failed to flush case-base");
    }
}
//----------------------------------------------------------------------------------------------
void RetainerEx::ExecuteCommand(const char* p_cmd)
{
    if (!strcmp(p_cmd, "flush"))
    {
        Flush();
        LogInfo("flushing case-base has be performed successfully");
    }
    else if (!strcmp(p_cmd, "info"))
    {
        LogInfo("retriever has %d cases", m_casebase.CaseContainer.size());
    }
}
//-------------------------------------------------------------------------------------------------------------------------------
RetainerEx::~RetainerEx()
{
    Flush();

    m_casebase.DeleteAll();
}