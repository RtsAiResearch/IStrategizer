#ifndef TOOLBOX_H
#define TOOLBOX_H

#include "CharacterBuffer.h"
#include <cassert>

class Toolbox
{
public:
    static void GetCharacterBuffer(const string& p_text, CharacterBuffer* p_buffer);

    //////////////////////////////////////////////////////////////////////////
    template<class T>
    static void MemoryClean(vector<T>& p_vect)
    {
        for(vector<T>::iterator itr = p_vect.begin();
            itr != p_vect.end();
            itr++)
        {
            MemoryClean(*itr);
        }
        p_vect.clear();
    }

    //////////////////////////////////////////////////////////////////////////
    template<class T>
    static void MemoryClean(T*& p_ptr)
    {
        if(p_ptr == nullptr)
            return;

        delete p_ptr;
        p_ptr = nullptr;
    }
};

#endif // TOOLBOX_H
