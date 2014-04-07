#ifndef ICLONABLE_H
#define ICLONABLE_H

namespace IStrategizer
{
    class IClonable
    {
    public:
        ~IClonable() {}
        virtual IClonable* Clone() = 0;
        virtual void Copy(IClonable* pDest) = 0;
    };
}

#endif // ICLONABLE_H
