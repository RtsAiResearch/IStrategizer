#include "RetainerEx.h"
#include "CaseBaseEx.h"
#include "Toolbox.h"
#include "ObjectSerializer.h"
#include "RtsGame.h"
#include "GoalEx.h"

#include <fstream>
using namespace std;
using namespace IStrategizer;

RetainerEx::RetainerEx() :
    AbstractRetainer("Retainer"), 
    m_caseBasePath(CASEBASE_FILENAME), 
    m_caseBaseLoaded(false)
{
}
//----------------------------------------------------------------------------------------------
void RetainerEx::ReadCaseBase()
{
    LogInfo("Reading case-base %s", m_caseBasePath.c_str());

    fstream file;

    file.open(m_caseBasePath.c_str(), ios::in | ios::binary);

    // Read existing case-base
    if (file.is_open())
    {
        file.close();
        g_ObjectSerializer.Deserialize(&m_casebase, m_caseBasePath);
        m_caseBaseLoaded = true;
    }
    // Create case-base if not found
    else
    {
        file.open(m_caseBasePath.c_str(), ios::out | ios::binary);
        file.close();
        m_caseBaseLoaded = true;
        Flush();
    }
}
//----------------------------------------------------------------------------------------------
void RetainerEx::Retain(_In_ CaseEx* pCase)
{
    if (find(m_casebase.CaseContainer.begin(), m_casebase.CaseContainer.end(), pCase) == m_casebase.CaseContainer.end())
        m_casebase.CaseContainer.push_back(pCase);
}
//----------------------------------------------------------------------------------------------
void RetainerEx::Flush()
{
    if (m_caseBaseLoaded)
    {
        LogInfo("Flushing case-base");
        g_ObjectSerializer.Serialize(&m_casebase, m_caseBasePath);
    }
    else
    {
        LogError("Failed to flush case-base");
    }
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
        sprintf_s(buffer, "retriever has %d cases", m_casebase.CaseContainer.size());
        g_Game->DisplayMessage(buffer);
    }
}
//-------------------------------------------------------------------------------------------------------------------------------
RetainerEx::~RetainerEx()
{
    Flush();

    while (!m_casebase.CaseContainer.empty())
    {
        auto& pCase = m_casebase.CaseContainer.back();
        SAFE_DELETE(pCase);
        m_casebase.CaseContainer.pop_back();
    }
}