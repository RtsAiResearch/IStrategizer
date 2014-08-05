#include "CharacterBuffer.h"
#include "Toolbox.h"

//////////////////////////////////////////////////////////////////////////
void CharacterBuffer::ExtractString( int p_offset, int p_count,  string& p_str )
{
    int end = p_offset + p_count;

    p_str.clear();
    p_str.resize(p_count);

    for(int i = 0; p_offset < end; p_offset++, i++)
    {
        p_str[i] = m_buffer[p_offset]->Char;
    }
}

//////////////////////////////////////////////////////////////////////////
CharacterBuffer::~CharacterBuffer()
{
    Toolbox::MemoryClean(m_buffer);
}
