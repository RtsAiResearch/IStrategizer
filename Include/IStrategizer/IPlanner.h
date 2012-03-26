#ifndef IPLANNER_H
#define IPLANNER_H

class IPlanner
{
public:
	virtual void Update(unsigned long p_gameCycle) = 0;
};

#endif	// IPLANNER_H