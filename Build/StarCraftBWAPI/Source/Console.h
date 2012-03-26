#ifndef CONSOLE_H
#define CONSOLE_H

#include <Windows.h>
#include <stdio.h>
#include <fcntl.h>
#include <io.h>

#define CRT_ERROR	-1
#define CRT_READ	_T("r")
#define CRT_WRITE	_T("w")

#ifdef _UNICODE
#define _tcout std::wcout
#else
#define _tcout std::cout
#endif

#ifdef _UNICODE
#define _tcin std::wcin
#else
#define _tcin std::wcin
#endif

class CConsole
{
public:
	CConsole();
	~CConsole();

private:
	BOOL InitializeCrtHandles();
	BOOL ResetCrtHandles();

	HANDLE	m_hStdIn;
	HANDLE	m_hStdOut;
	INT		m_iCrtStdInDescriptor;
	INT		m_iCrtStdOutDescriptor;
	FILE	m_oldStdIn;
	FILE	m_oldStdOut;
	FILE	*m_pNewStdIn;
	FILE	*m_pNewStdOut;
	BOOL	m_bAlreadyHasConsole;
};

#endif // CONSOLE_H