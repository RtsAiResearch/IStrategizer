#ifndef WORLDSTATE_H
#define WORLDSTATE_H

#include <fstream>
#include <string>

using namespace std;

class WorldState
{
public:
	virtual void	ReadFromFile(ifstream& p_fileHandler) = 0;
	virtual string	ToString() = 0;
};

#endif	// WORLDSTATE_H