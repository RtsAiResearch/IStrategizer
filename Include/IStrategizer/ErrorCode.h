#ifndef ERRORCODE_H
#define ERRORCODE_H

enum ErrorType
{
	ERR_Success,
	ERR_InvalidParameterValue,
	ERR_NullReferenceValue,
	ERR_InvalidId,
	ERR_EntityDoesNotExist,
	ERR_PreconditionsNotSatisfied,
	ERR_PreparationFailed,
	ERR_ActionIsNotPrepared
};

/*
class ErrorHandler
{
private:
	long _lastErrorCode;

public:
	ErrorHandler() : _lastErrorCode(ERR_Success) { }
	static ErrorHandler Instance();
	long GetLastError();
	void SetLastError(long p_errorCode);
};
//---------------------------------------------------------------------------------
ErrorHandler ErrorHandler::Instance()
{
	static ErrorHandler m_instance;
	return m_instance;
}
//---------------------------------------------------------------------------------
#define g_ErrorHandler ErrorHandler::Instance()
//---------------------------------------------------------------------------------
long ErrorHandler::GetLastError()
{
	return _lastErrorCode;
}
//---------------------------------------------------------------------------------
void ErrorHandler::SetLastError(long p_errorCode)
{
	_lastErrorCode = p_errorCode;
}
//---------------------------------------------------------------------------------
*/
#endif	// ERRORCODE_H