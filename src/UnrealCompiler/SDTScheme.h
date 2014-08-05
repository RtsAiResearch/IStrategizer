#ifndef SDTSCHEME_H
#define SDTSCHEME_H

#include "Grammar.h"

#define SemanticActionMarker "#"

class SDTScheme : public Grammar
{
    map<string, SemanticActionType> m_actionsTable;
    map<string, AttributeName> m_attributesTable;

    void InitializeTables();
    SemanticActionType GetActionByName(const string& p_name);
    AttributeName GetAttributeByName(const string& p_name);
    LateBindingParameter ParseParameter( const string& p_parameter );

protected:
    virtual void ReadSemanticAction(ProductionBody& p_production, stringstream& p_eye);
    void ReadProductions(ifstream& p_eye);

public:
    SDTScheme() { InitializeTables(); }
};

#endif // SDTSCHEME_H
