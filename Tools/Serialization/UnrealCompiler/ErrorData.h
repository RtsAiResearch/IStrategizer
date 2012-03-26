#ifndef ERROR_H
#define ERROR_H

#include <string>
#include <vector>
using namespace std;

enum ErrorType
{
    ERROR_InvalidToken,
    ERROR_UnexpectedToken,
    ERROR_SyntaxError
};

struct ErrorDescription
{
public:
    ErrorType   Type;
    string      Format;

    ErrorDescription(ErrorType p_errorType,  const string& p_format)
    {
        Type    = p_errorType;
        Format  = p_format;
    }
};

struct ErrorData
{
    unsigned		ErrorTypeId;
    vector<string>  Parameter;
    int             Row;
    int             Column;

    ErrorData(int p_errorTypeId,  const string& p_parameter, int p_row, int p_column) :
    ErrorTypeId(p_errorTypeId), Row(p_row), Column(p_column)
    {
        Parameter.push_back(p_parameter);
    }

    ErrorData(int p_errorTypeId,  const vector<string>& p_parameters, int p_row, int p_column) :
    ErrorTypeId(p_errorTypeId), Row(p_row), Column(p_column)
    {
        Parameter.reserve(p_parameters.size());
        Parameter.insert(Parameter.begin(), p_parameters.begin(), p_parameters.end());
    }
};

const int MaxErroTypes = 128;
extern vector<ErrorDescription*> g_ErrorDescriptions;

#endif // ERROR_H