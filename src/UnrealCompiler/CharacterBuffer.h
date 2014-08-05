#ifndef CHARACTERBUFFER_H
#define CHARACTERBUFFER_H

#include <vector>
#include <string>
using namespace std;

struct PlainCharacter
{
    char    Char;
    int     Row;
    int     Column;
    PlainCharacter(char p_char, int p_row, int p_column) : Char(p_char), Row(p_row), Column(p_column)
    { }
};

class CharacterBuffer
{
    vector<PlainCharacter*> m_buffer;
public:
    void ExtractString(int p_offset, int p_count,  string& p_str);
    vector<PlainCharacter*>& InnerBuffer();
    char& operator[](int p_index);
    
    ~CharacterBuffer();
};

//////////////////////////////////////////////////////////////////////////
inline vector<PlainCharacter*>& CharacterBuffer::InnerBuffer()
{
    return m_buffer;
}

//////////////////////////////////////////////////////////////////////////
inline char& CharacterBuffer::operator[]( int p_index )
{
    return m_buffer[p_index]->Char;
}
#endif // CHARACTERBUFFER_H
