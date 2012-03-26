#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#ifndef EVERYTHING_H
	#include "Everything.h"
#endif

#include <vector>
#include <stack>
using namespace std;

class FileManager
{
    stack<TCHAR*> dirStack;
    TCHAR workingDirectory[MAX_PATH_LONG+1];
    TCHAR currPath[MAX_PATH_LONG+1];
    BOOL TraverseDirectory(LPTSTR, LPTSTR, DWORD, LPBOOL, CHAR* filter, vector<string>& filePathes);
    DWORD FileType(LPWIN32_FIND_DATA);
    int GetFilesAux(int argc, LPTSTR argv[], CHAR* filter, vector<string>& filePathes);
    BOOL CompareExtension(CHAR *filename, CHAR *extension);
public:
    FileManager();
    void PushDir(const TCHAR* p_currentDir);
    void PopDir();
    const TCHAR* GetCurrentDir();
    vector<string> GetFiles(const TCHAR* p_dirPath, char* p_filter);
	static FileManager& Instance() { static FileManager instance; return instance; }
	#define g_FileManager FileManager::Instance()
};

#endif // FILEMANAGER_H