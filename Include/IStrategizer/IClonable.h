#ifndef ICLONABLE_H
#define ICLONABLE_H

namespace IStrategizer
{
    class IClonable
    {
    public:
        virtual IClonable* Clone() = 0;
        virtual void Copy(IClonable* p_dest) = 0;
    };
}

#endif // ICLONABLE_H
