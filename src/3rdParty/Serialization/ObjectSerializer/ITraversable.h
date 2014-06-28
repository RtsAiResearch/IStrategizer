#ifndef TRAVERSABLE_H
#define TRAVERSABLE_H

namespace Serialization
{
    class Iterator
    {
    public:
        virtual char*   Current() = 0;
        virtual bool    MoveNext() = 0;
        virtual void    Reset() = 0;
        virtual ~Iterator() {}
    };

    class ITraversable
    {
    public:
        virtual Iterator* GetIterator() = 0;
        virtual ~ITraversable() {}
    };
}

#endif // TRAVERSABLE_H
