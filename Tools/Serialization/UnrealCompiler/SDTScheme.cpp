#include "SDTScheme.h"
#include <sstream>
#include <fstream>

void SDTScheme::InitializeTables()
{
    m_actionsTable["mov"] = ACTION_Assign;
    m_actionsTable["add"] = ACTION_Add;
    m_actionsTable["mul"] = ACTION_Multiply;

    m_attributesTable["inhVal"] =  ATTRNAME_InhVal;
    m_attributesTable["synVal"] =  ATTRNAME_SynVal;
    m_attributesTable["lexVal"] =  ATTRNAME_LexVal;
}

SemanticActionType SDTScheme::GetActionByName( const string& p_name )
{
    map<string, SemanticActionType>::iterator where;
    where = m_actionsTable.find(p_name);
    assert(where != m_actionsTable.end());

    return where->second;
}

AttributeName SDTScheme::GetAttributeByName( const string& p_name )
{
    map<string, AttributeName>::iterator where;
    where = m_attributesTable.find(p_name);
    assert(where != m_attributesTable.end());

    return where->second;
}

void SDTScheme::ReadSemanticAction(ProductionBody& p_production, stringstream& p_eye)
{
    SemanticAction  action;
    string          actionName;
    string          parameter;

    p_eye >> actionName;
    action.Type = GetActionByName(actionName);

    while(p_eye >> parameter && parameter != "}")
    {
        action.Params.push_back(ParseParameter(parameter));
    }

    p_production.Actions.push_back(action);
}

LateBindingParameter SDTScheme::ParseParameter( const string& p_parameter )
{
    LateBindingParameter    parameter;
    unsigned                dotPos;

    dotPos = p_parameter.find('.', 0);
    assert(dotPos != string::npos);

    parameter.first = p_parameter.substr(0, dotPos);
    parameter.second = GetAttributeByName(p_parameter.substr(dotPos + 1, p_parameter.size() - (dotPos + 1)));

    return parameter;
}

void SDTScheme::ReadProductions(ifstream& p_eye)
{
    string                      line;
    stringstream                lineStream;
    ProductionHead              nonTerminal;
    string                      symbol;

    while(getline(p_eye, line))
    {
        if(line.empty())
            continue;

        lineStream.clear();
        lineStream.str(line);

        lineStream >> nonTerminal;

        if(!lineStream || nonTerminal == "#")
            break;

        ProductionBody production;

        while(lineStream >> symbol)
        {
            if(symbol == "{")
            {
                ReadSemanticAction(production, lineStream);
                production.Production.push_back(SemanticActionMarker);
            }
            else
                production.Production.push_back(symbol);
        }
        
        assert(!production.Production.empty());
        Productions[nonTerminal].push_back(production);
    }
}
