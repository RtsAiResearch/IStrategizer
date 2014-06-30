#ifndef PRODUCTIONBODY_H
#define PRODUCTIONBODY_H

#include "SemanticAction.h"
#include "GrammarDefinitions.h"

struct ProductionBody
{
    ProductionType          Production;
    vector<SemanticAction>  Actions;

    typedef ProductionType::iterator ProductionItr;
    typedef ProductionType::reverse_iterator ReverseProductionItr;

    typedef vector<SemanticAction>::iterator ActionItr;
    typedef vector<SemanticAction>::reverse_iterator ReverseActionItr;
};

#endif // PRODUCTIONBODY_H

