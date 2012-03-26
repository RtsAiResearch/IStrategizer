#include <cassert>
#include <iostream>
#include <tchar.h>
#include "Console.h"

CConsole::CConsole()
{
	// initialize members
	ZeroMemory(this, sizeof(CConsole));

	BOOL bOk;
	// try creating a new console in case none exists
	bOk = AllocConsole();

	m_bAlreadyHasConsole = !bOk;

	m_hStdIn	= GetStdHandle(STD_INPUT_HANDLE);
	m_hStdOut	= GetStdHandle(STD_OUTPUT_HANDLE);

	// I am the console owner
	// Redirect CRT (stdin, stdout) handles to me
	if (!m_bAlreadyHasConsole)
	{
		bOk = InitializeCrtHandles();
		assert(bOk);
	}
}
//////////////////////////////////////////////////////////////////////////
CConsole::~CConsole()
{
	// If I am the console owner, then I have the rights to free it
	if (!m_bAlreadyHasConsole)
	{
		BOOL bOk;
		bOk = ResetCrtHandles();
		assert(bOk);
		bOk = FreeConsole();
		assert(bOk);
	}
}
//////////////////////////////////////////////////////////////////////////
BOOL CConsole::InitializeCrtHandles()
{
	// 1. Associate Windows std handles with C file descriptors
	m_iCrtStdInDescriptor = _open_osfhandle((intptr_t)m_hStdIn, _O_TEXT);
	if (m_iCrtStdInDescriptor == CRT_ERROR)
		return FALSE;

	m_iCrtStdOutDescriptor = _open_osfhandle((intptr_t)m_hStdOut, _O_TEXT);
	if (m_iCrtStdOutDescriptor == CRT_ERROR)
		return FALSE;

	// 2. Associate a stream with the previously associated CRT files in step 1
	m_pNewStdIn = _tfdopen(m_iCrtStdInDescriptor, CRT_READ);
	if (m_pNewStdIn == NULL)
		return FALSE;

	m_pNewStdOut = _tfdopen(m_iCrtStdOutDescriptor, CRT_WRITE);
	if (m_pNewStdOut == NULL)
		return FALSE;

	// 3. Copy the current std stream address in case we want to restore the original streams later
	m_oldStdIn	= *stdin;
	m_oldStdOut	= *stdout;

	// 4. make std streams point to the previously created console streams in step 2
	*stdin	= *m_pNewStdIn;
	*stdout	= *m_pNewStdOut;

	return TRUE;
}
//////////////////////////////////////////////////////////////////////////
BOOL CConsole::ResetCrtHandles()
{
	// 1. make std streams point to the original streams
	*stdin	= m_oldStdIn;
	*stdout	= m_oldStdOut;

	BOOL bOk;
	// 2. free the previously created console streams
	bOk = !fclose(m_pNewStdIn);
	bOk = !fclose(m_pNewStdOut);

	return bOk;
}