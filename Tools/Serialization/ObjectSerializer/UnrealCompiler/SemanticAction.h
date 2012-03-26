#ifndef SEMANTICACTION_H
#define SEMANTICACTION_H

#include "Attribute.h"
#include "GrammarDefinitions.h"

enum ActionType
{
    ACTION_Null,
    ACTION_Assign,
    ACTION_Add,
    ACTION_Multiply,
};

typedef pair<GrammarConstruct, AttributeName> LateBindingParameter;

struct SemanticAction
{
    ActionType Type;
    vector<LateBindingParameter> Params;
};

#endif // SEMANTICACTION_H
