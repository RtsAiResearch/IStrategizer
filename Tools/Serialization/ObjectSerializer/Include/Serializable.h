#ifndef SERIALIZABLE_H
#define SERIALIZABLE_H

#ifndef TRAVERSABLE_H
	#include "Traversable.h"
#endif
#include <typeinfo>
using namespace std;

namespace Serialization
{
    class Serializable : public Traversable
    {
    public:
        virtual string          TypeName()  = 0;
        virtual int             TypeSize()  = 0;
        virtual Serializable*   Prototype() = 0;
        string                  CName() { return typeid(*this).name(); }
    };
}
#endif // SERIALIZABLE_H
