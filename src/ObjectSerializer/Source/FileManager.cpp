#ifndef FILEMANAGER_H
    #include "FileManager.h"
#endif

#include <iostream>
#include <cassert>
//----------------------------------------------------------------------------------------------
/* Chapter 3. Windows lsW file list command. */
/* lsW[options][files]
    List the attributes of one or more files.
    Options:
        -R recursive
        -l longList listing(size and time of modification)
            Depending on the ProcessItem function, this will
            also list the owner and permissions(See Chapter 5 - security). */

/* This program illustrates:
        1. Search handles and directory traversal
        2. File attributes, including time
        3. Using generic string functions to output file information */
/* THIS PROGRAM IS NOT A FAITHFUL IMPLEMENATION OF THE POSIX ls COMMAND - IT IS INTENDED TO ILLUSRATE THE WINDOWS API */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
/* BEGIN BOILERPLATE CODE */

FileManager::FileManager()
{
    int length = GetCurrentDirectory(MAX_PATH_LONG, workingDirectory);
    _ASSERTE(length > 0);
    PushDir(workingDirectory);
}
//----------------------------------------------------------------------------------------------
const TCHAR* FileManager::GetCurrentDir()
{
    return dirStack.top();
}
//----------------------------------------------------------------------------------------------
void FileManager::PushDir(const TCHAR* p_currentDir)
{
    dirStack.push(const_cast<TCHAR*>(p_currentDir));
    _tcscpy(currPath, dirStack.top());
    SetCurrentDirectory(currPath);
}
//----------------------------------------------------------------------------------------------
void FileManager::PopDir()
{
    _ASSERTE(dirStack.size() > 1);
    dirStack.pop();
    _tcscpy(currPath, dirStack.top());
    SetCurrentDirectory(currPath);
}
//----------------------------------------------------------------------------------------------
vector<string> FileManager::GetFiles(const TCHAR* p_dirPath, char* p_filter)
{
    vector<string> fileNames;

    LPTSTR args[2];
    args[1] = p_filter;

    PushDir(p_dirPath);
    GetFilesAux(2, args, p_filter, fileNames);
    PopDir();

    return fileNames;
}
/* Chapter 3. Windows lsW file list command. */
/* lsW[options][files]
    List the attributes of one or more files.
    Options:
        -R recursive
        -l longList listing(size and time of modification)
            Depending on the ProcessItem function, this will
            also list the owner and permissions(See Chapter 5 - security). */

/* This program illustrates:
        1. Search handles and directory traversal
        2. File attributes, including time
        3. Using generic string functions to output file information */
/* THIS PROGRAM IS NOT A FAITHFUL IMPLEMENATION OF THE POSIX ls COMMAND - IT IS INTENDED TO ILLUSRATE THE WINDOWS API */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
/* BEGIN BOILERPLATE CODE */

int FileManager::GetFilesAux(int argc, LPTSTR argv[], CHAR* filter, vector<string>& filePathes)
{
    BOOL flags[MAX_OPTIONS], ok = TRUE;
    TCHAR searchPattern[MAX_PATH + 1], parentPath[MAX_PATH_LONG+1];
    LPTSTR pSlash, pSearchPattern;
    int i, fileIndex;
    DWORD pathLength;

    flags[0] = 1;
    flags[1] = 0;
    fileIndex = 2; //Options(argc, argv, _T("Rl"), &flags[0], &flags[1], NULL);

    /* "Parse" the search pattern into two parts: the "parent"
        and the file name or wild card expression. The file name
        is the longest suffix not containing a slash.
        The parent is the remaining prefix with the slash.
        This is performed for all command line search patterns.
        If no file is specified, use * as the search pattern. */

    pathLength = _tcslen(currPath);

    if (pathLength == 0 || pathLength >= MAX_PATH_LONG) { /* pathLength >= MAX_PATH_LONG (32780) should be impossible */
        std::cerr << "GetCurrentDirectory failed";
        return FALSE;
    }

    if (argc < fileIndex + 1) 
        ok = TraverseDirectory(currPath, _T("*"), MAX_OPTIONS, flags, filter, filePathes);
    else for (i = fileIndex; i < argc; i++) {
        if (_tcslen(argv[i]) >= MAX_PATH) {
            std::cerr << "The command line argument is longer than the maximum this program supports";
            return FALSE;
        }
        _tcscpy(searchPattern, argv[i]);
        _tcscpy(parentPath, argv[i]);

        /* Find the rightmost slash, if any.
            Set the path and use the rest as the file name. */
        pSlash = _tstrrchr(parentPath, _T('\\')); 
        if (pSlash != NULL) {
            *pSlash = _T('\0');
            _tcscat(parentPath, _T("\\"));         
            SetCurrentDirectory(parentPath); /* Now restore searchPattern. */
            pSlash = _tstrrchr(searchPattern, _T('\\'));  
            pSearchPattern = pSlash + 1;
        } else {
            _tcscpy(parentPath, _T(".\\"));
            pSearchPattern = searchPattern;
        }
        ok = TraverseDirectory(parentPath, pSearchPattern, MAX_OPTIONS, flags, filter, filePathes) && ok;
        SetCurrentDirectory(currPath);  /* Restore working directory. */
    }

    return ok ? 0 : 1;
}
//----------------------------------------------------------------------------------------------
BOOL FileManager::TraverseDirectory(LPTSTR parentPath, LPTSTR searchPattern, DWORD numFlags, LPBOOL flags, CHAR* filter, vector<string>& filePathes)
/* Traverse a directory, carrying out an implementation specific "action" for every
    name encountered. The action in this version is "list, with optional attributes". */
/* searchPattern: Relative or absolute searchPattern to traverse in the parentPath.  */
/* On entry, the current directory is parentPath, which ends in a \ */
{
    HANDLE searchHandle;
    WIN32_FIND_DATA findData;
    BOOL recursive = flags[0];
    DWORD fType, iPass, lenParentPath;

    /* Open up the directory search handle and get the
        first file name to satisfy the path name.
        Make two passes. The first processes the files
        and the second processes the directories. */

    if ( _tcslen(searchPattern) == 0 ) {
        _tcscat(searchPattern, _T("*"));
    }
    /* Remove a backslash, if any, at the end of the parent path */
    if (parentPath[_tcslen(parentPath)-1] != _T('\\') ) { /* Add a \ to the end of the parent path, unless there already is one */
        _tcscat (parentPath, _T("\\"));
    }

    /* Open up the directory search handle and get the
        first file name to satisfy the path name. Make two passes.
        The first processes the files and the second processes the directories. */

    CHAR* szDirct = (CHAR*)malloc(MAX_PATH);
    GetCurrentDirectory(MAX_PATH, szDirct);

    for (iPass = 1; iPass <= 2; iPass++) {
        searchHandle = FindFirstFile("*", &findData);
        if (searchHandle == INVALID_HANDLE_VALUE) {
            std::cerr << "Error opening Search Handle.";
            return FALSE;
        }

        /* Scan the directory and its subdirectories for files satisfying the pattern. */
        do {

        /* For each file located, get the type. List everything on pass 1.
            On pass 2, display the directory name and recursively process
            the subdirectory contents, if the recursive option is set. */
            fType = FileType(&findData);
            if (iPass == 1) /* ProcessItem is "print attributes". */
            {
                if (CompareExtension(findData.cFileName, filter))
                {
                    string filePath = currPath;
                    filePath += findData.cFileName;
                    filePathes.push_back(filePath);
                }
            }

            lenParentPath = (DWORD)_tcslen(parentPath);
            /* Traverse the subdirectory on the second pass. */
            if (fType == TYPE_DIR && iPass == 2 && recursive) {
                _tprintf(_T("\n%s%s:"), parentPath, findData.cFileName);
                SetCurrentDirectory(findData.cFileName);
                if (_tcslen(parentPath) + _tcslen(findData.cFileName) >= MAX_PATH_LONG-1) {
                    std::cerr << "Path Name is too long";
                }
                _tcscat (parentPath, findData.cFileName); /* The parent path terminates with \ before the _tcscat call */
                TraverseDirectory(parentPath, _T("*"), numFlags, flags, filter, filePathes);
                /* Restore parent path - It will then terminate with \ */
                parentPath[lenParentPath] = _T('\0');
                SetCurrentDirectory(parentPath); /* SetCurrentDirectory(_T("..")); would also work */
            }

            /* Get the next file or directory name. */

        } while (FindNextFile(searchHandle, &findData));

        FindClose(searchHandle);
    }
    return TRUE;
}
//----------------------------------------------------------------------------------------------
DWORD FileManager::FileType(LPWIN32_FIND_DATA pFileData)

/* Return file type from the find data structure.
    Types supported:
        TYPE_FILE: If this is a file
        TYPE_DIR: If this is a directory other than . or ..
        TYPE_DOT: If this is . or .. directory */
{
    BOOL isDir;
    DWORD fType;
    fType = TYPE_FILE;
    isDir =(pFileData->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
    if (isDir)
        if (lstrcmp(pFileData->cFileName, _T(".")) == 0
                || lstrcmp(pFileData->cFileName, _T("..")) == 0)
            fType = TYPE_DOT;
        else fType = TYPE_DIR;
    return fType;
}
//----------------------------------------------------------------------------------------------
BOOL FileManager::CompareExtension(CHAR *filename, CHAR *extension)
{
    DWORD i;

    /* Sanity checks */
    if(filename == NULL || extension == NULL)
        return FALSE;

    if(strlen(filename) == 0 || strlen(extension) == 0)
        return FALSE;

    if(strchr(filename, '.') == NULL || strchr(extension, '.') == NULL)
        return FALSE;

    /* Iterate backwards through respective strings and compare each CHAR one at a time */
    for(i = 0; i < strlen(filename); i++)
    {
        if(tolower(filename[strlen(filename) - i - 1]) == tolower(extension[strlen(extension) - i - 1]))
        {
            if(i == strlen(extension) - 1)
                return TRUE;
        } else
            break;
    }

    return FALSE;
}
