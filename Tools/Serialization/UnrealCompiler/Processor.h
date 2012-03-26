#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <cassert>
#include <vector>
#include "SemanticAction.h"
#include "InstructionSet.h"
using namespace std;

class Processor
{
public:
    static void Execute( SemanticActionType p_action, vector<pair<AttributeName, AttributeSet*> >& p_params ) 
    {
        switch(p_action)
        {
        case ACTION_Add:
            assert(p_params.size() == 3);
            InstructionSet::Add(
                p_params[0].second->operator [](p_params[0].first).Value,
                p_params[1].second->operator [](p_params[1].first).Value,
                p_params[2].second->operator [](p_params[2].first).Value);
            p_params[0].second->operator [](p_params[0].first).Evaluated = true;
            break;

        case ACTION_Assign:
            assert(p_params.size() == 2);
            InstructionSet::Mov(
                p_params[0].second->operator [](p_params[0].first).Value,
                p_params[1].second->operator [](p_params[1].first).Value);
            p_params[0].second->operator [](p_params[0].first).Evaluated = true;
            break;

        case ACTION_Multiply:
            assert(p_params.size() == 3);
            InstructionSet::Mul(
                p_params[0].second->operator [](p_params[0].first).Value,
                p_params[1].second->operator [](p_params[1].first).Value,
                p_params[2].second->operator [](p_params[2].first).Value);
            p_params[0].second->operator [](p_params[0].first).Evaluated = true;
            break;

        default:
            assert(false);
        }
    }
};

#endif // PROCESSOR_H