#ifndef USEROBJECT_H
#define USEROBJECT_H

#include <vector>
#include <string>
#include <stdarg.h>
#include <cassert>
#include <typeinfo>

#include "SVector.h"
#ifndef SERIALIZABLE_H
    #include "ISerializable.h"
#endif

namespace Serialization
{
    class UserObject : public ISerializable
    {
        enum ObjectState
        {
            Initialized     = 0xfeedbeef,
            Uninitialized   = 0xdeadbeef
        };
    
    public:
                        UserObject() : m_objectState(Uninitialized), m_objID(++sm_lastSerializableObjID) {}
        Iterator*       GetIterator()   { return new VectorIterator<char*>(&m_membersAddresses); }

        void InitializeAddresses()
        {
            // catch any type-casting failure by assuring a predefinded values
            _ASSERTE(Initialized == m_objectState || Uninitialized == m_objectState);
            m_membersAddresses.clear();
            InitializeAddressesAux();
            m_objectState = Initialized;
        }

    protected:
        void AddMemberAddress(unsigned nAddresses, ...)
        {
            void* memberAddress;
            va_list argList;
            // The va_start macro is usually equivalent to:
            // pArgList = (void*) &p_memberAddress + sizeof (p_memberAddress) ;
            va_start (argList, nAddresses);
            ++nAddresses;
            while(--nAddresses)
            {
                memberAddress = va_arg(argList, void*);
                m_membersAddresses.push_back(reinterpret_cast<char*>(memberAddress));
            }
            va_end (argList) ;
        }

        virtual void InitializeAddressesAux() {};

    private:
        ObjectState         m_objectState;
        size_t              m_objID;
        std::vector<char*>  m_membersAddresses;
    };

#define OBJECT_MEMBERS(N, ...) \
    protected: \
        void InitializeAddressesAux() { AddMemberAddress(N, __VA_ARGS__); }
#define OBJECT_MEMBERS_P(P, N, ...) \
    protected: \
        void InitializeAddressesAux() { P::InitializeAddressesAux(); AddMemberAddress(N, __VA_ARGS__); }

}
#endif // USEROBJECT_H
