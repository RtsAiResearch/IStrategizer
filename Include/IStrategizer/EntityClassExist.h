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
        std::vector<TID> _entityObjectIds;
        std::map<EntityObjectAttribute, int> _entityObjectAttributes;
        int _similarityThreshold;
        bool _oneUse;
        bool _used;
        bool _blocking;

    protected:
        void InitializeAddressesAux();

    public:
        EntityClassExist() {}
        EntityClassExist(PlayerType p_player, EntityClassType p_unitClassId, int p_amount, 
                            std::map<EntityObjectAttribute,int> p_entityObjectAttributes, 
                            int p_similarityThreshold = ExactSimilarity, bool p_blocking = false);
        EntityClassExist(PlayerType p_player, EntityClassType p_unitClassId, int p_amount, bool p_oneUse = false, bool p_blocking = false);
        bool MatchesAttributes(int p_entityObjectId);
        vector<TID> EntityObjectIds() const { return _entityObjectIds; }
        int GetEntityIdByIndex(int p_index) const { return _entityObjectIds[p_index]; }
        bool Evaluate(RtsGame* pRtsGame);
        void Copy(IClonable* p_dest);
        bool Consume(int p_amount);
    };
}

#endif // ENTITYCLASSEXIST_H
