#ifndef COLONY_H
	#include "Colony.h"
#endif

IClonable* Colony::Clone()
{
    Colony* m_clone = new Colony();
    Copy(m_clone);

    return m_clone;
}
//----------------------------------------------------------------------------------------------
void Colony::Copy(IClonable* p_dest)
{
    Colony* m_dest = static_cast<Colony*>(p_dest);

    m_dest->Height    = Height;
    m_dest->Width     = Width;
    m_dest->Top       = Top;
    m_dest->Left      = Left;
}
