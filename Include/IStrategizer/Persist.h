#ifndef Persist_H
#define Persist_H

#include <fstream>
#include <string>
#include <vector>
#include <wchar.h>

using namespace std;

class Persist
{
public:
	static Persist& Instance();
	static void Dispose();
	void WriteToFile(string p_filePath, vector<pair<string, string>> p_data);
	void WriteToFile(string p_filePath, vector<string> p_data);
	void WriteToFile(string p_filePath, string p_data);
	void AppendToFile(string p_filePath, string p_data);
    string ReadFile(string p_filePath);
};

#define g_Persist Persist::Instance()

#endif // Persist_H