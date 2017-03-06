#include "ErrorFactory.h"
#include <cassert>
#include <sstream>
using namespace std;

vector<ErrorDescription*> g_ErrorDescriptions(MaxErroTypes, NULL);

//////////////////////////////////////////////////////////////////////////
void ErrorFactory::Initialize()
{
    g_ErrorDescriptions[0] = new ErrorDescription(ERROR_InvalidToken, "'%s' is an invalid token");
    g_ErrorDescriptions[1] = new ErrorDescription(ERROR_UnexpectedToken, "expected '%s', found '%s'");
    g_ErrorDescriptions[2] = new ErrorDescription(ERROR_SyntaxError, "syntax error: '%s'");
}
//////////////////////////////////////////////////////////////////////////
void ErrorFactory::GetFormattedError(const ErrorData* p_errorData, int p_errorIdx, string& p_formattedError)
{
    ErrorDescription*   errorDesc;
    stringstream        errorStream;
    const unsigned bufferSize = 256;
    char buffer[bufferSize];

    assert(g_ErrorDescriptions.size() > 0 && p_errorData->ErrorTypeId < g_ErrorDescriptions.size());

    errorDesc = g_ErrorDescriptions[p_errorData->ErrorTypeId];
    if(p_errorIdx > 0)
    {
        errorStream << '[' << p_errorIdx << ']' << ' ';
    }
    errorStream << "Error " << p_errorData->ErrorTypeId << ": Line " << p_errorData->Row << ": " << errorDesc->Format.c_str();
    
    int numParams = p_errorData->Parameter.size();
    assert(numParams <= MaxParams);

    switch(numParams)
    {
    case 0:
        sprintf_s(buffer, bufferSize, errorStream.str().c_str());
        break;
    case 1:
        sprintf_s(buffer, bufferSize, errorStream.str().c_str(), p_errorData->Parameter[0].c_str());
        break;
    case 2:
        sprintf_s(buffer, bufferSize, errorStream.str().c_str(), p_errorData->Parameter[0].c_str(), p_errorData->Parameter[1].c_str());
        break;
    case 3:
        sprintf_s(buffer, bufferSize, errorStream.str().c_str(), p_errorData->Parameter[0].c_str(), p_errorData->Parameter[1].c_str(), p_errorData->Parameter[2].c_str());
        break;
    }

    p_formattedError = buffer;
}
