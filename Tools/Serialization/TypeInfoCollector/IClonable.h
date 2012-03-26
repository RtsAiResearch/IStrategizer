#ifndef ICLONABLE_H
#define ICLONABLE_H

class IClonable
{
public:
	virtual IClonable* Clone() = 0;
};

#endif	// ICLONABLE_H