#include "MathHelper.h"
#include "CheckEntityObjectSimilarity.h"


CheckEntityObjectSimilarity::CheckEntityObjectSimilarity(PlayerType p_player, int p_entityObjectId, const map<EntityObjectAttribute, int>& p_similarityFeatures, int p_similarityThreshold)
: ConditionEx(p_player, CONDEX_CheckEntityObjectSimilarity)
{
    _conditionParameters[PARAM_EntityObjectId] =  p_entityObjectId;
    
    // @fixme: how to add the corresponding parameters!
    //_conditionParameters.insert(p_similarityFeatures.begin(), p_similarityFeatures.end());

    _similarityThreshold = p_similarityThreshold;

    _obj1FeatureVector.reserve(p_similarityFeatures.size());
    _obj2FeatureVector.reserve(p_similarityFeatures.size());

    for(map<EntityObjectAttribute, int>::const_iterator itr = p_similarityFeatures.begin();
        itr != p_similarityFeatures.end();
        itr++)
    {
        _featureNames.insert(itr->first);
        _obj1FeatureVector.push_back(itr->second);
    }

}
//----------------------------------------------------------------------------------------------
void CheckEntityObjectSimilarity::InitializeAddressesAux()
{
    ConditionEx::InitializeAddressesAux();
    AddMemberAddress(4,
        &_featureNames,
        &_obj1FeatureVector,
        &_obj2FeatureVector,
        &_similarityThreshold);
}
//---------------------------------------------------------------------------------------------------
bool CheckEntityObjectSimilarity::Evaluate()
{
    ConditionEx::Evaluate();

    int m_returnValue = QuerySimilarityFeatures();

    if (m_returnValue == ERR_Success)
    {
		_isSatisfied = (MathHelper::EuclideanDistance(_obj1FeatureVector, _obj2FeatureVector) == ExactSimilarity - _similarityThreshold)? 1 : 0;

        _isEvaluated = true;
    }

    return _isEvaluated && _isSatisfied;
}
//---------------------------------------------------------------------------------------------------
int CheckEntityObjectSimilarity::QuerySimilarityFeatures()
{
    _obj2FeatureVector.clear();

    for(set<EntityObjectAttribute>::const_iterator itr = _featureNames.begin();
        itr != _featureNames.end();
        itr++)
    {
        int m_attributeValue;
        int m_returnValue = g_Assist.GetEntityObjectAttribute(_playerType, _conditionParameters[PARAM_EntityObjectId], *itr, m_attributeValue);

        if(m_returnValue != ERR_Success)
            return m_returnValue;

        _obj2FeatureVector.push_back(m_attributeValue);
    }

    return ERR_Success;
}
//---------------------------------------------------------------------------------------------------
void CheckEntityObjectSimilarity::Copy(IClonable* p_dest)
{
    ConditionEx::Copy(p_dest);

	CheckEntityObjectSimilarity* m_dest = static_cast<CheckEntityObjectSimilarity*>(p_dest);

	m_dest->_featureNames         = _featureNames;
	m_dest->_obj1FeatureVector    = _obj1FeatureVector;
	m_dest->_obj2FeatureVector    = _obj2FeatureVector;
	m_dest->_similarityThreshold  = _similarityThreshold;
}