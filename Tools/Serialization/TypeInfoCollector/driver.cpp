#ifndef OBJECTFORMATTER_H
	#include "ObjectFormatter.h"
#endif

#ifndef FILEMANAGER_H
	#include "FileManager.h"
#endif

#include <Windows.h>

int _tmain(int argc, LPTSTR argv[])
{
    const TCHAR* sourceCodeDir;
    const TCHAR* workingDir;
    if(argc == 1)
    {
        sourceCodeDir = g_FileManager.GetCurrentDir();
    }
    else if(argc == 2)
    {
        sourceCodeDir = argv[1];
    }
    else if(argc == 3)
    {
        sourceCodeDir   = argv[1];
        workingDir      = argv[2];
        g_FileManager.PushDir(workingDir);
    }

    printf("TypeInfoCollector\n=================\n");
    printf("- Working Directory:\n\t%s\n", g_FileManager.GetCurrentDir());
    printf("- Source Code Directory:\n\t%s\n", sourceCodeDir);
    g_ObjectFormatter.WriteTypeTable(sourceCodeDir);
    printf("\n\nTypeInfo Collection Ended ...\n");

    if(argc == 3)
        g_FileManager.PopDir();
}