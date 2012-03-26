#include "SyntaxDirectedTranslator.h"
#include "SDTScheme.h"
#include "Processor.h"

//////////////////////////////////////////////////////////////////////////
void SyntaxDirectedTranslator::Translate(ParseTreeNode* p_parseTreeRoot)
{
    TranslateAux(p_parseTreeRoot);
}

//////////////////////////////////////////////////////////////////////////
void SyntaxDirectedTranslator::TranslateAux( ParseTreeNode* p_node )
{
    // Inductive Case:
    // The node is leaf, it is a terminal, mark the node as evaluated
    if(p_node->Children()->empty())
    {
        // FIXME: it is assumed that the nodes keys are strings and the attributes are integers
        assert(m_grammar->Terminals.find(p_node->Key()) != m_grammar->Terminals.end());

        //p_node->Value()->LexVal.Evaluated = true;
    }
    else
    {
        vector<SemanticAction>* actions = &m_grammar->Productions[p_node->Key()][p_node->Value()->ProductionRelativeIdx].Actions;
        SemanticAction currentAction;
        unsigned actionIdx = 0;

        for(vector<ParseTreeNode*>::iterator nodeItr = p_node->Children()->begin();
            nodeItr != p_node->Children()->end();
            nodeItr++)
        {
            if((*nodeItr)->Key() == SemanticActionMarker)
            {
                assert(actionIdx < actions->size());
                ExecuteAction(p_node, actions->at(actionIdx++));
            }
            else
            {
                TranslateAux(*nodeItr);
            }
        }
    }
}
//////////////////////////////////////////////////////////////////////////
void SyntaxDirectedTranslator::ExecuteAction( ParseTreeNode* p_node, SemanticAction& p_action )
{
    vector<pair<AttributeName, AttributeSet*> > bindingTable;
    BindAttributes(p_node, p_action.Params, bindingTable);
    
    Processor::Execute(p_action.Type, bindingTable);
}
//////////////////////////////////////////////////////////////////////////
void SyntaxDirectedTranslator::BindAttributes(ParseTreeNode* p_node, vector<LateBindingParameter>& p_params, vector<pair<AttributeName, AttributeSet*> >& p_bindingTable)
{
    for(vector<LateBindingParameter>::iterator paramItr = p_params.begin();
        paramItr != p_params.end();
        paramItr++)
    {
        if(p_node->Key() == paramItr->first)
        {
            p_bindingTable.push_back(make_pair(paramItr->second, p_node->Value()));
        }
        else
        {
            size_t instanceMarkPos = paramItr->first.find('#');
            if(instanceMarkPos != string::npos)
            {
                assert(instanceMarkPos < paramItr->first.size() - 1);
                int targetInstanceId = atoi(paramItr->first.substr(instanceMarkPos + 1, paramItr->first.size() - instanceMarkPos - 1).c_str());
                int currentInstanceId = 0;
                string productionSymbol = paramItr->first.substr(0, instanceMarkPos);
                    
                for(vector<ParseTreeNode*>::iterator nodeItr = p_node->Children()->begin();
                    nodeItr != p_node->Children()->end();
                    nodeItr++)
                {
                    if((*nodeItr)->Key() == SemanticActionMarker)
                        continue;

                    if((*nodeItr)->Key() == productionSymbol)
                    {
                        currentInstanceId++;
                        if(targetInstanceId == currentInstanceId)
                        {
                            p_bindingTable.push_back(make_pair(paramItr->second, (*nodeItr)->Value()));
                            break;
                        }
                    }
                }
            }
            else
            {
                for(vector<ParseTreeNode*>::iterator nodeItr = p_node->Children()->begin();
                    nodeItr != p_node->Children()->end();
                    nodeItr++)
                {
                    if((*nodeItr)->Key() == SemanticActionMarker)
                        continue;

                    if((*nodeItr)->Key() == paramItr->first)
                    {
                        p_bindingTable.push_back(make_pair(paramItr->second, (*nodeItr)->Value()));
                        break;
                    }
                }
            }
        }
    }
}