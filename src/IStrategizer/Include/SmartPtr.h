#pragma once

#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace IStrategizer
{
    class RC
    {
    private:
        int count; // Reference count

    public:
        void AddRef()
        {
            // Increment the reference count
            count++;
        }

        int Release()
        {
            // Decrement the reference count and
            // return the reference count.
            return --count;
        }
    };

    template < typename T >
    class SmartPtr
    {
    private:
        T*    pData;       // pointer
        RC* reference; // Reference count

    public:
        SmartPtr() : pData(0), reference(0)
        {
            // Create a new reference 
            reference = (RC*)HeapAlloc(GetProcessHeap(), 0, sizeof(RC));
            ZeroMemory(&reference, sizeof(reference));
            // Increment the reference count
            reference->AddRef();
        }

        SmartPtr(T* pValue) : pData(pValue), reference(0)
        {
            // Create a new reference 
            reference = (RC*)HeapAlloc(GetProcessHeap(), 0, sizeof(RC));
            // Increment the reference count
            reference->AddRef();
        }

        SmartPtr(const SmartPtr<T>& sp) : pData(sp.pData), reference(sp.reference)
        {
            // Copy constructor
            // Copy the data and reference pointer
            // and increment the reference count
            reference->AddRef();
        }

        ~SmartPtr()
        {
            // Destructor
            // Decrement the reference count
            // if reference become zero delete the data
            if (reference->Release() == 0)
            {
                HeapFree(GetProcessHeap(), 0, pData);
                HeapFree(GetProcessHeap(), 0, reference);
            }
        }

        T& operator* ()
        {
            return *pData;
        }

        T* operator-> ()
        {
            return pData;
        }

        SmartPtr<T>& operator = (const SmartPtr<T>& sp)
        {
            // Assignment operator
            if (this != &sp) // Avoid self assignment
            {
                // Decrement the old reference count
                // if reference become zero delete the old data
                if (reference->Release() == 0)
                {
                    HeapFree(GetProcessHeap(), 0, pData);
                    HeapFree(GetProcessHeap(), 0, reference);
                }

                // Copy the data and reference pointer
                // and increment the reference count
                pData = sp.pData;
                reference = sp.reference;
                reference->AddRef();
            }
            return *this;
        }
    };
}