///> [Serializable]
#ifndef RESOURCEEXIST_H
#define RESOURCEEXIST_H

#include "ConditionEx.h"

namespace IStrategizer
{
    ///> class=ResourceExist
    ///> parent=ConditionEx
    class ResourceExist : public ConditionEx
    {
        OBJECT_SERIALIZABLE(ResourceExist);

    private:
        int _availableAmount;

    public:
        ResourceExist() {}
        ResourceExist(PlayerType p_player, int p_resourceId, int p_amount);
        int AvailableAmount() const { return _availableAmount; }
        bool Evaluate(RtsGame* pRtsGame);
        void Copy(IClonable* p_dest);
        bool Consume(int p_amount);
    };
}

#endif // RESOURCEEXIST_H
