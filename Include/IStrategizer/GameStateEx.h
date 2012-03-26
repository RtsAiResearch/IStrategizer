///> [Serializable]
#ifndef GAMESTATEEX_H
#define GAMESTATEEX_H

#include "WorldState.h"
#include "EngineData.h"

#ifndef USEROBJECT_H
	#include "UserObject.h"
#endif

#ifndef SVECTOR_H
	#include "SVector.h"
#endif
using namespace Serialization;

///> class=GameStateEx
class GameStateEx: public UserObject
{
private:
    ///> type=ShallowFeaturesEx
	ShallowFeaturesEx	_shallowFeatures;
    ///> type=DeepFeaturesEx
	DeepFeaturesEx		_deepFeatures;

public:
	GameStateEx();
	GameStateEx(const ShallowFeaturesEx& p_shallowFeatures, const DeepFeaturesEx& p_deepFeatures) : _shallowFeatures(p_shallowFeatures), _deepFeatures(p_deepFeatures){}
    //----------------------------------------------------------------------------------------------
	ShallowFeaturesEx&	ShallowFeatures() { return _shallowFeatures; }
	DeepFeaturesEx&		DeepFeatures() { return _deepFeatures; }
    //----------------------------------------------------------------------------------------------
    // Serialization
    string              TypeName()  { return "GameStateEx"; }
    int                 TypeSize()  { return sizeof(GameStateEx); }
    UserObject*         Prototype() { return new GameStateEx; }

protected:
    void                InitializeAddressesAux();
    //----------------------------------------------------------------------------------------------
};

#endif	// GAMESTATEEX_H