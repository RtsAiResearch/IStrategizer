///> [Serializable]
#ifndef ENTITYCLASSEXIST_H
#define ENTITYCLASSEXIST_H

#include "ConditionEx.h"

#include <vector>
#include <map>

namespace IStrategizer
{
    ///> class=EntityClassExist
    ///> parent=ConditionEx
    class EntityClassExist : public ConditionEx
    {
        OBJECT_SERIALIZABLE(EntityClassExist);

    private:
        int _similarityThreshold;
        bool _oneUse;
        bool _used;

    public:
        EntityClassExist() {}
        EntityClassExist(PlayerType p_player, EntityClassType p_unitClassId, int p_amount, std::map<EntityObjectAttribute,int> p_entityObjectAttributes, int p_similarityThreshold = ExactSimilarity);
        EntityClassExist(PlayerType p_player, EntityClassType p_unitClassId, int p_amount, bool p_oneUse = false);
        EntityClassExist(PlayerType p_player, int p_amount, bool p_oneUse = false);
        EntityClassExist(PlayerType p_player);
        bool Evaluate(RtsGame& game);
        void Copy(IClonable* p_dest);
        bool Consume(int p_amount);
    };
}

#endif // ENTITYCLASSEXIST_H
