#include "Toolbox.h"

//////////////////////////////////////////////////////////////////////////
void Toolbox::GetCharacterBuffer(const string& p_text, CharacterBuffer* p_buffer)
{
    int row     = 0;
    int column  = 0;
    int pos     = 0;

    p_buffer->InnerBuffer().clear();
    p_buffer->InnerBuffer().reserve(p_text.size());

    for(string::const_iterator itr = p_text.begin(); itr != p_text.end(); itr++)
    {
        p_buffer->InnerBuffer().push_back(new PlainCharacter(*itr, row, column));

        if(*itr == '\n' || *itr == '\r')
        {
            row++;
            column = 0;
        }
        else
        {
            column++;
        }
        pos++;
    }
}
