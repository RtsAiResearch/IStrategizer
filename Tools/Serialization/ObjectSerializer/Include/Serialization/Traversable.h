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
        ~Iterator() {}
    };

    class Traversable
    {
    public:
        virtual Iterator* GetIterator() = 0;
        ~Traversable() {}
    };
}

#endif // TRAVERSABLE_H