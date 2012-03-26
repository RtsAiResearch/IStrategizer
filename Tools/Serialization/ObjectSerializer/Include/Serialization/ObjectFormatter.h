#ifndef OBJECTFORMATTER_H
#define OBJECTFORMATTER_H

#include <string>
#include <vector>
#include <map>
using namespace std;

#ifndef TYPEDECLARATIONPARSER_H
#include "TypeDeclarationParser.h"
#endif

#ifndef TYPENODE_H
#include "TypeNode.h"
#endif

#ifndef TYPETABLE_H
	#include "TypeTable.h"
#endif

#ifndef OBJECTFACTORY_H
	#include "ObjectFactory.h"
#endif

extern string g_TypeInfoPath;
extern string g_TypeLexerFilePath;
extern string g_TypeNamesFilePath;
extern string g_SerializationTag;

enum HeaderAttribute
{
    HATTR_Class,
    HATTR_Type,
    HATTR_Parent,
    HATTR_Alias
};

typedef map<HeaderAttribute, string> Attributes;

class ObjectFormatter
{
    LexicalAnalyzer*                m_lexer;
    TypeDeclarationParser*          m_parser;
    CharacterBuffer*                m_buffer;
    vector<string>                  m_sourceFiles;
    vector<string>                  m_typeNames;
    map<string, HeaderAttribute>    m_headerNameMap;

    void ReadTypeNames();
    void WriteTypeNames();
    void ReadSourceFiles(const string& p_sourceCodeDir);
    void InitializeParsing();
    void ParseFormat(string& p_formatLine, TypeTable& p_typeTable, TypeNode*& p_lastTypeRoot);
    void Split(string& p_str, char p_delim, vector<string>& p_tokens);
    void ExtractTypes(string& p_sourceFileName, TypeTable& p_typeTable);
    void GetAttributes(string& p_line, Attributes& p_attributes);
    void GetMemberNode(Attributes& p_attributes, TypeTable& p_typeTable, TypeNode*& p_lastTypeRoot);
    void GetNewTypeNode(Attributes& p_attributes, TypeTable& p_typeTable, TypeNode*& p_lastTypeRoot);
    void GetParentNode(Attributes& p_attributes,  TypeTable& p_typeTable, TypeNode*& p_lastTypeRoot);
    void GetAliasNode(Attributes& p_attributes, TypeTable& p_typeTable, TypeNode*& p_lastTypeRoot);
    bool IsFormatLine(string& p_line, string& p_format);
    void CollectTemplateSpecialization(TypeTable& p_typeTable, ObjectTable& p_objectTable);

public:
    ~ObjectFormatter();
    ObjectFormatter();
    
    void WriteTypeTable(const string& p_sourceCodeDir);
    void FinalizeTypeTable(TypeTable& p_typeTable, ObjectTable& p_objectTable);
    void ReadTypeTable(TypeTable& p_typeTable);
    static ObjectFormatter& Instance() { static ObjectFormatter instance; return instance; }

#define  g_ObjectFormatter ObjectFormatter::Instance()
};

#endif // OBJECTFORMATTER_H
