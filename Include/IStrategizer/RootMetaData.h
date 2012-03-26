#ifndef ROOTMETADATA_H
#define ROOTMETADATA_H

#include <cstdio>

#define DEFINE(ENAME)			Define_##ENAME
#define NAME(ENAME)				STRING(ENAME)
#define PREFIX(ENAME)			Prefix_##ENAME
#define START(ENAME)			(PREFIX(ENAME)##_START)
#define END(ENAME)				(PREFIX(ENAME)##_END)
#define COUNT(ENAME)			(END(ENAME) - START(ENAME))
#define INDEX(EMEMBER, ENAME)   (EMEMBER - START(ENAME))
#define GET(EINDEX, ENAME)		(EINDEX + START(ENAME))
#define BELONG(ENAME, EVALUE)	(EVALUE >= START(ENAME) && EVALUE < END(ENAME))
#define STRING(X)				#X
#define CATEGORY(X)				(X >> 24)
#define TYPE(X)					((X >> 16) && 0x000000ff)
#define IDX(X)					(X && 0x0000ffff)
#define ENUMS_SIZE				0xfffff

namespace MetaData
{
	/*
	ID format:
	[f       ][ff  ][ff  ]
	[Category][Type][Idx ]

	Example: 0x20200
	Category: 0x2 | Type: 0x02 | Idx = 0x00
	*/
	extern const char* Enums[ENUMS_SIZE];
}

#endif // ROOTMETADATA_H