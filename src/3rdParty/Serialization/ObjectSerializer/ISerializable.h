#ifndef SERIALIZABLE_H
#define SERIALIZABLE_H

#ifndef TRAVERSABLE_H
#include "ITraversable.h"
#endif
#include <typeinfo>

namespace Serialization
{
    class ISerializable : public ITraversable
    {
    public:
        virtual std::string     TypeName()  const = 0;
        virtual int             TypeSize()  const = 0;
        virtual ISerializable*  Prototype() const = 0;
        std::string             CName() const { return typeid(*this).name(); }
    };

    extern size_t sm_lastSerializableObjID;
}

#define OBJECT_SERIALIZABLE(ClassName) \
    public: \
    std::string                 TypeName()  const { return #ClassName; } \
    int                         TypeSize()  const { return sizeof(ClassName); } \
    Serialization::UserObject*  Prototype() const { return new ClassName; }	

#endif // SERIALIZABLE_H
