#ifndef ICOMPARABLE_H
#define ICOMPARABLE_H

namespace IStrategizer
{
    class IComparable
    {
    public:
        virtual int Compare(IComparable* p_rhs) = 0;
    };
}

#endif // ICOMPARABLE_H
