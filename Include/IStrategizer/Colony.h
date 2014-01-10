///> [Serializable]
#ifndef COLONY_H
#define COLONY_H

#include "MathHelper.h"

#ifndef USEROBJECT_H
#include "UserObject.h"
#endif

#ifndef ICLONABLE_H
#include "IClonable.h"
#endif

namespace IStrategizer
{
    ///> class=Colony
    class Colony : public Serialization::UserObject, public IClonable
    {
        OBJECT_SERIALIZABLE(Colony);

    public:
        ///> type=int
        int Top;
        ///> type=int
        int Left;
        ///> type=int
        int Width;
        ///> type=int
        int Height;

        Colony()  : Top(0), Left(0), Width(0), Height(0) {}
        Colony(int p_top, int p_left, int p_width, int p_height) : Top(p_top), Left(p_left), Width(p_width), Height(p_height) {}
        bool Contains(int p_x, int p_y) const
        {
            return MathHelper::RectangleMembership(Top, Left, Width, Height, p_x, p_y);
        }

        IClonable*  Clone();
        void        Copy(IClonable* p_dest);
        

    protected:
        void InitializeAddressesAux() { AddMemberAddress(4, &Top, &Left, &Width, &Height); }
    };
}

#endif // COLONY_H