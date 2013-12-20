///> [Serializable]
#ifndef CHECKENTITYOBJECTSIMILARITY_H
#define CHECKENTITYOBJECTSIMILARITY_H
#include <vector>
#include "ConditionEx.h"

#ifndef SSET_H
	#include "SSet.h"
#endif

namespace IStrategizer
{
///> class=CheckEntityObjectSimilarity
///> parent=ConditionEx
class CheckEntityObjectSimilarity : public ConditionEx
{
	OBJECT_SERIALIZABLE(CheckEntityObjectSimilarity);

private:
    ///> type=set(int)
    Serialization::SSet<EntityObjectAttribute>  _featureNames;
    ///> type=vector(int)
    Serialization::SVector<int>                 _obj1FeatureVector;
    ///> type=vector(int)
    Serialization::SVector<int>                 _obj2FeatureVector;
    ///> type=int
    int                         _similarityThreshold;
public:
                CheckEntityObjectSimilarity() {}
				CheckEntityObjectSimilarity(PlayerType p_player, int p_entityObjectId, const std::map<EntityObjectAttribute, int>& p_similarityFeatures, int p_similarityThreshold = ExactSimilarity);
    bool		Evaluate();
    int			QuerySimilarityFeatures();
    void	    Copy(IClonable* p_dest);
	bool		Consume(int p_amount) { return true; }

protected:
	void InitializeAddressesAux();
	//----------------------------------------------------------------------------------------------
};
}

#endif	// CHECKENTITYOBJECTSIMILARITY_H