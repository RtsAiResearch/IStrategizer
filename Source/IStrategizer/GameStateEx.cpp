#include "GameStateEx.h"
#ifndef METADATA_H
#include "MetaData.h"
#endif

using namespace MetaData;

GameStateEx::GameStateEx()
{
	//_shallowFeatures.resize(COUNT(ShallowFeatureType), 0.0);
	//_deepFeatures.resize(COUNT(DeepFeatureType), 0.0);
}
//----------------------------------------------------------------------------------------------
void GameStateEx::InitializeAddressesAux()
{
    AddMemberAddress(2,
        &_shallowFeatures,
        &_deepFeatures);
}