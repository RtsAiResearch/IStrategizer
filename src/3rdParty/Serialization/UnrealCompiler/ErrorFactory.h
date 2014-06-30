#ifndef ERRORFACTORY_H
#define ERRORFACTORY_H

#include "ErrorData.h"

#define MaxParams 3

class ErrorFactory
{
    void Initialize();
protected:
    ErrorFactory();
public:
    static ErrorFactory& Instance();
    void GetFormattedError(const ErrorData* p_errorData, int p_errorIdx, string& p_formattedError);
};

//////////////////////////////////////////////////////////////////////////
inline ErrorFactory::ErrorFactory()
{
    Initialize();
}
//////////////////////////////////////////////////////////////////////////
inline ErrorFactory& ErrorFactory::Instance()
{
    static ErrorFactory instance;
    return instance;
}

#define g_ErrorFactory ErrorFactory::Instance()

#endif // ERRORFACTORY_H
