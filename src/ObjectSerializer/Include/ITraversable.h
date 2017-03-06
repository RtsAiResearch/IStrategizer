#ifndef TRAVERSABLE_H
#define TRAVERSABLE_H

namespace Serialization
{
    class Iterator
    {
    public:
        virtual ~Iterator() {}
        virtual char*   Current() = 0;
        virtual bool    MoveNext() = 0;
        virtual void    Reset() = 0;
    };

    class ITraversable
    {
    public:
        virtual ~ITraversable() {}
        virtual Iterator* GetIterator() const = 0;
    };
}

#endif // TRAVERSABLE_H
